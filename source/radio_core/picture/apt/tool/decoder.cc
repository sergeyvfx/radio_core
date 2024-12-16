// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of APT messages from a WAV file.
// Decoded images are stored in a specified folder.

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
#include "radio_core/picture/apt/decoder.h"
#include "radio_core/picture/apt/info.h"
#include "radio_core/tool/log_util.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace radio_core::picture::apt {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;

namespace audio_wav_reader = tiny_lib::audio_wav_reader;

struct CLIOptions {
  inline static constexpr int kDefaultChannel = 1;
  inline static constexpr const char* kDefaultFormatStr = "PNG";

  std::filesystem::path input_audio_filepath;
  int audio_channel = kDefaultChannel;

  std::filesystem::path output_directory;

  std::string format_str = kDefaultFormatStr;
};

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "apt_decoder", "0.1", argparse::default_arguments::help);

  program.add_description(
      "Decode APT transmissions from WAV file and store them as files.");

  program.add_argument("input_audio")
      .help("Path to input audio containing APT transmissions");

  program.add_argument("output_directory")
      .help("Path to output directory to store images in");

  program.add_argument("--channel")
      .default_value(CLIOptions::kDefaultChannel)
      .help("Channel of audio file to use in 1-based indexing")
      .scan<'i', int>();

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
  options.format_str = program.get<std::string>("--format");

  return options;
}

// Check that the command line options are valid.
//
// Returns true if the options are valid and can be used.
// Reports error and returns false otherwise.
auto CheckCLIOptionsValidOrReport(const CLIOptions& cli_options) -> bool {
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

// Processor of result from the APT decoder.
//
// Takes care of assembling the individual result to images which are stored on
// disk.
class ResultProcessor {
 public:
  explicit ResultProcessor(const CLIOptions& options)
      : output_directory_(options.output_directory) {}

  void Process(const Decoder<float>::Result& result) {
    if (!result.Ok()) {
      return;
    }

    for (const DecodedVariant& variant : result.GetValue()) {
      variant | match{
                    [&](const Line& line) { AppendImageLine(line.pixels); },
                    [&](const LineSynchronization& /*sync*/) {},
                };
    }
  }

  void Flush() { EndImage(); }

  inline auto GetNumDecodedImages() const -> int { return num_decoded_images_; }

 private:
  void AppendImageLine(const std::span<const Color1ub> pixels) {
    if (!is_decoding_image_) {
      pixels_storage_.clear();
      is_decoding_image_ = true;
    }

    pixels_storage_.insert(pixels_storage_.end(), pixels.begin(), pixels.end());
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

    const int image_width = Info::kNumPixelsPerLine;

    // TODO(sergey): Use File from TinyCore to help dealing with non-ASCII
    // paths.
    stbi_write_png(filepath.generic_string().c_str(),
                   image_width,
                   pixels_storage_.size() / image_width,
                   1,
                   pixels_storage_.data(),
                   image_width);
  }

  std::filesystem::path output_directory_;

  bool is_decoding_image_{false};

  // Storage of pixel data.
  std::vector<Color1ub> pixels_storage_;

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
       << tool::LogTimeWithRealtimeComparison(decode_time_in_seconds,
                                              file_duration_in_seconds)
       << endl;

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::picture::apt

int main(int argc, char** argv) {
  return radio_core::picture::apt::Main(argc, argv);
}
