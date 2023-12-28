// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of SSTV messages from a WAV file.
// Decoded images are stored in a specified folder.

#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <argparse/argparse.hpp>

#include "radio_core/base/scoped_timer.h"
#include "radio_core/base/string_util.h"
#include "radio_core/base/variant.h"
#include "radio_core/picture/sstv/decoder.h"
#include "radio_core/picture/sstv/mode.h"
#include "radio_core/picture/sstv/mode_limits.h"
#include "radio_core/picture/sstv/mode_spec.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace radio_core::picture::sstv {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;

namespace audio_wav_reader = tiny_lib::audio_wav_reader;

// TODO(sergey): Look into making the list of named modes available for
// all applications.

struct NamedMode {
  std::string_view name;
  Mode mode;
};

inline static constexpr auto kNamedModes = std::to_array<NamedMode>({
    {"PD90", Mode::kPD90},
    {"PD120", Mode::kPD120},
    {"PD160", Mode::kPD160},
    {"PD180", Mode::kPD180},
    {"PD240", Mode::kPD240},
    {"PD290", Mode::kPD290},
});

struct CLIOptions {
  inline static constexpr int kDefaultChannel = 1;
  inline static constexpr const char* kDefaultModeStr = "PD120";
  inline static constexpr const char* kDefaultFormatStr = "PNG";

  std::filesystem::path input_audio_filepath;
  int audio_channel = kDefaultChannel;

  std::filesystem::path output_directory;

  std::string mode_str = kDefaultModeStr;
  std::string format_str = kDefaultFormatStr;
};

// Get a single string which contains comma-separated list of all supported
// encoding methods.
auto GetCommaSeparatedAllModes() -> std::string {
  std::string result;

  for (const NamedMode& named_mode : kNamedModes) {
    if (!result.empty()) {
      result += ", ";
    }
    result += std::string(named_mode.name);
  }

  return result;
}

// Get mode from its name.
// If the mode name is unknown `Mode::kUnknown` is returned.
auto GetModeFromName(const std::string_view name) -> Mode {
  for (const NamedMode& named_mode : kNamedModes) {
    if (named_mode.name == name) {
      return named_mode.mode;
    }
  }

  return Mode::kUnknown;
}

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "sstv_decoder", "0.1", argparse::default_arguments::help);

  program.add_description(
      "Decode SSTV transmissions from WAV file and store them as files.");

  program.add_argument("input_audio")
      .help("Path to input audio containing SSTV transmissions");

  program.add_argument("output_directory")
      .help("Path to output directory to store images in");

  program.add_argument("--channel")
      .default_value(CLIOptions::kDefaultChannel)
      .help("Channel of audio file to use in 1-based indexing")
      .scan<'i', int>();

  program.add_argument("--mode")
      .default_value(std::string{CLIOptions::kDefaultModeStr})
      .help("Encoding scheme (" + GetCommaSeparatedAllModes() +
            "). Used in a case message format was not detected fro its VIS.");

  program.add_argument("--format")
      .default_value(std::string{CLIOptions::kDefaultFormatStr})
      .help("Image format (PNG)");

  return program;
}

// Parse command line arguments and return parsed result.
// MOTE: No sanity check on the options is done here.
auto ParseCLIAndGetOptions(int argc, char** argv) -> CLIOptions {
  argparse::ArgumentParser program = ConstructCLIParser();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    cerr << err.what() << endl;
    cerr << endl << program;
    ::exit(1);
  }

  CLIOptions options;

  options.input_audio_filepath = program.get<std::string>("input_audio");
  options.output_directory = program.get<std::string>("output_directory");
  options.audio_channel = program.get<int>("--channel");
  options.mode_str = program.get<std::string>("--mode");
  options.format_str = program.get<std::string>("--format");

  return options;
}

// Check that the command line options are valid.
//
// Returns true if the options are valid and can be used.
// Reports error and returns false otherwise.
auto CheckCLIOptionsValidOrReport(const CLIOptions& cli_options) -> bool {
  // Validate encoding mode.
  if (GetModeFromName(cli_options.mode_str) == Mode::kUnknown) {
    cerr << "Unknown mode." << endl;
    return false;
  }

  if (cli_options.format_str != "PNG") {
    // TODO(sergey): Support other formats from the STB.
    cerr << "Unknown image format." << endl;
    return false;
  }

  if (!std::filesystem::exists(cli_options.output_directory)) {
    cerr << "Output directory does not exist." << endl;
    return false;
  }
  if (!std::filesystem::is_directory(cli_options.output_directory)) {
    cerr << "Output directory is not a directory." << endl;
    return false;
  }

  return true;
}

// Processor of result from the SSTV decoder.
//
// Takes care of assembling the individual result to images which are stored on
// disk.
class ResultProcessor {
 public:
  explicit ResultProcessor(const CLIOptions& options)
      : output_directory_(options.output_directory) {
    static constexpr int kMaxNumPixels =
        ModeLimits::kMaxImageWidth * ModeLimits::kMaxImageHeight;
    pixels_storage_.resize(kMaxNumPixels);
  }

  void Process(const Decoder<float>::Result& result) {
    if (!result.Ok()) {
      return;
    }

    for (const DecodedVariant& variant : result.GetValue()) {
      variant | match{
                    [](const DecodedVISCode& decoded) {
                      const Mode mode = GetModeFromVISCode(decoded.vis_code);
                      cout << "Found VIS " << int(decoded.vis_code) << " ("
                           << mode << ")" << endl;
                    },

                    [](const LineSynchronization& /*decoded*/) {},

                    [&](const ImagePixelsBegin& decoded) {
                      cout << "Begin of pixels in " << decoded.mode << endl;
                      EndImage();
                      BeginImage(decoded.mode);
                    },

                    [&](const ImagePixelsRow& decoded) {
                      AppendImageRow(decoded.pixels);
                    },

                    [&](const ImagePixelsEnd& /*decoded*/) {
                      cout << "End of pixels" << endl;
                      EndImage();
                    },
                };
    }
  }

  void Flush() { EndImage(); }

  inline auto GetNumDecodedImages() const -> int { return num_decoded_images_; }

 private:
  void BeginImage(const Mode mode) {
    assert(mode != Mode::kUnknown);

    std::fill(
        pixels_storage_.begin(), pixels_storage_.end(), Color3ub(0, 0, 0));

    mode_spec_ = ModeSpec<float>::Get(mode);
    is_decoding_image_ = true;
    row_index_ = 0;
  }

  void AppendImageRow(const std::span<const Color3ub> pixels) {
    assert(is_decoding_image_);

    const size_t offset = row_index_ * mode_spec_.image_width;
    std::copy(pixels.begin(), pixels.end(), pixels_storage_.begin() + offset);

    ++row_index_;
  }

  void EndImage() {
    if (!is_decoding_image_) {
      return;
    }
    is_decoding_image_ = false;

    SaveImage();
  }

  void SaveImage() {
    // TODO(sergey): Support other formats supported by STB.
    const char* extension = "png";

    char filename[32];
    StringPrintFormat(filename,
                      sizeof(filename),
                      "%06d.%s",
                      num_decoded_images_ + 1,
                      extension);

    // Rely on the fact that the decoder uses our storage for pixels to store
    // them in the exact way matching the spec, filling it in continuously.
    // This allows to avoid re-sampling the image to get a continuous block of
    // memory suitable for writing.

    const std::filesystem::path filepath = output_directory_ / filename;

    // TODO(sergey): Use File from TinyCore to help dealing with non-ASCII
    // paths.
    stbi_write_png(filepath.generic_string().c_str(),
                   mode_spec_.image_width,
                   mode_spec_.image_height,
                   3,
                   pixels_storage_.data(),
                   3 * mode_spec_.image_width);
  }

  std::filesystem::path output_directory_;

  ModeSpec<float> mode_spec_;
  bool is_decoding_image_{false};

  size_t row_index_;

  // Storage of pixel data.
  //
  // NOTE: Heap-allocated to lower stack memory usage. Using stack memory leads
  // to stack overflow error in the Main() when using MSVC.
  std::vector<Color3ub> pixels_storage_;

  int num_decoded_images_{0};
};

auto Main(int argc, char** argv) -> int {
  // Parse command line argument and validate them.
  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);
  if (!CheckCLIOptionsValidOrReport(cli_options)) {
    return EXIT_FAILURE;
  }

  // Open WAV file for read.
  File file;
  if (!file.Open(cli_options.input_audio_filepath, File::kRead)) {
    cerr << "Error opening WAV file for read." << endl;
    return EXIT_FAILURE;
  }

  // Open WAV reader to access format of the file.
  audio_wav_reader::Reader<File> wav_file_reader;
  if (!wav_file_reader.Open(file)) {
    cerr << "Error reading WAV file." << endl;
    return EXIT_FAILURE;
  }

  // Open audio file and print its information.
  const audio_wav_reader::FormatSpec format_spec =
      wav_file_reader.GetFormatSpec();
  const float file_duration_in_seconds = wav_file_reader.GetDurationInSeconds();

  cout << format_spec.sample_rate << " samples per second, "
       << format_spec.bit_depth << " bits depth, " << format_spec.num_channels
       << " audio channel(s)." << endl;

  cout << "File duration: " << file_duration_in_seconds << " seconds." << endl;

  // Validate the audio channel.
  if (cli_options.audio_channel < 1 ||
      cli_options.audio_channel > format_spec.num_channels) {
    cerr << "Invalid requested audio channel " << cli_options.audio_channel
         << "." << endl;
    return EXIT_FAILURE;
  }

  // Configure the decoder.
  const Decoder<float>::Options decoder_options = {
      .sample_rate = float(format_spec.sample_rate),
      .mode = GetModeFromName(cli_options.mode_str),
  };
  Decoder<float> decoder;
  decoder.Configure(decoder_options);

  const ScopedTimer scoped_timer;

  ResultProcessor result_processor(cli_options);

  auto sample_processor = [&](const float sample) {
    Decoder<float>::Result result = decoder(sample);
    result_processor.Process(result);
  };

  wav_file_reader.ReadAllSamples<float, 16>(
      [&](const std::span<const float> sample) {
        sample_processor(sample[cli_options.audio_channel - 1]);
      });

  // Make sure all samples from file are processed and are not being stuck in
  // the filter delays.
  for (int i = 0; i < 1000; ++i) {
    sample_processor(0.0f);
  }

  result_processor.Flush();

  // Decode statistics.
  const float decode_time_in_seconds = scoped_timer.GetElapsedTimeInSeconds();
  cout << endl;
  cout << result_processor.GetNumDecodedImages() << " images decoded in "
       << decode_time_in_seconds << " seconds";
  if (decode_time_in_seconds != 0) {
    cout << " (" << (file_duration_in_seconds / decode_time_in_seconds)
         << "x realtime)";
  }
  cout << endl;

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::picture::sstv

int main(int argc, char** argv) {
  return radio_core::picture::sstv::Main(argc, argv);
}
