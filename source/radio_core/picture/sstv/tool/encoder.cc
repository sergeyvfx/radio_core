// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of images into SSTV transmission. The image is read from disk,
// encoded into SSTV toned which are stored into a WAV file.

#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <argparse/argparse.hpp>

#include "radio_core/picture/memory_pixel_accessor.h"
#include "radio_core/picture/sstv/encoder.h"
#include "radio_core/picture/sstv/message.h"
#include "radio_core/picture/sstv/mode.h"
#include "radio_core/picture/sstv/mode_spec.h"
#include "radio_core/signal/generator.h"
#include "tl_audio_wav/tl_audio_wav_writer.h"
#include "tl_io/tl_io_file.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace radio_core::picture::sstv {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;

namespace audio_wav_writer = tiny_lib::audio_wav_writer;

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
  inline static constexpr int kDefaultSampleRate = 44100;
  inline static constexpr const char* kDefaultModeStr = "PD120";
  inline static constexpr bool kDefaultVOX = false;

  std::filesystem::path input_image_filepath;
  std::filesystem::path output_wav_filepath;

  int sample_rate = kDefaultSampleRate;

  bool generate_vox = kDefaultVOX;
  std::string mode_str = kDefaultModeStr;
};

// Thin wrapper around the image data returned by STB.
class ImageData {
 public:
  ImageData() = default;

  ImageData(ImageData&& other) noexcept
      : width(other.width),
        height(other.height),
        num_channels(other.num_channels),
        pixels_data(other.pixels_data),
        pixels(other.pixels) {
    other.pixels_data = nullptr;
  }

  auto operator=(ImageData&& other) -> ImageData& {
    if (this == &other) {
      return *this;
    }

    width = other.width;
    height = other.height;
    num_channels = other.num_channels;
    pixels_data = other.pixels_data;
    pixels = other.pixels;

    other.pixels_data = nullptr;

    return *this;
  }

  ImageData(const ImageData& other) = delete;
  auto operator=(const ImageData& other) -> ImageData = delete;

  ~ImageData() {
    if (pixels_data) {
      stbi_image_free(pixels_data);
    }
  }

  int width{0}, height{0};
  int num_channels{0};
  unsigned char* pixels_data{nullptr};

  std::span<unsigned char> pixels;
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

// Parse command line arguments and return parsed result.
// MOTE: No sanity check on the options is done here.
auto ParseCLIAndGetOptions(const int argc, char** argv) -> CLIOptions {
  argparse::ArgumentParser program(
      "sstv_encoder", "0.1", argparse::default_arguments::help);

  program.add_description(
      "Encode image into SSTV transmission stored in a WAV file.");

  program.add_argument("input_image")
      .help("Path to input image to encode into SSTV transmission");

  program.add_argument("output_audio")
      .help("Path to output WAV file with encoded image transmission");

  program.add_argument("--rate")
      .default_value(CLIOptions::kDefaultSampleRate)
      .required()
      .help("Sample rate of the output WAV file")
      .scan<'i', int>();

  program.add_argument("--vox")
      .default_value(CLIOptions::kDefaultVOX)
      .implicit_value(true)
      .help("Generate VOX tones before VIS");

  program.add_argument("--mode")
      .default_value(std::string{CLIOptions::kDefaultModeStr})
      .required()
      .help("Encoding scheme (" + GetCommaSeparatedAllModes() + ")");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    cerr << err.what() << endl;
    cerr << endl << program;
    ::exit(1);
  }

  CLIOptions options;

  options.input_image_filepath = program.get<std::string>("input_image");
  options.output_wav_filepath = program.get<std::string>("output_audio");
  options.sample_rate = program.get<int>("--rate");
  options.generate_vox = program.get<bool>("--vox");
  options.mode_str = program.get<std::string>("--mode");

  return options;
}

// Check that the command line options are valid.
//
// Returns true if the options are valid and can be used.
// Reports error and returns false otherwise.
auto CheckCLIOptionsValidOrReport(const CLIOptions& cli_options) -> bool {
  // Validate sample rate.
  if (cli_options.sample_rate <= 0) {
    cerr << "Invalid sample rate." << endl;
    return false;
  }

  // Validate encoding mode.
  if (GetModeFromName(cli_options.mode_str) == Mode::kUnknown) {
    cerr << "Unknown mode." << endl;
    return false;
  }

  return true;
}

// Read pixels from the input image.
//
// Will do all checks while reading, ensuring that the image file exists and is
// valid image, image resolution is usable by the encoding mode, and that pixels
// are successfully read from the image.
//
// Returns an image on success.
// If an error is detected it is reported and nullopt is returned.
auto ReadCheckedImagePixels(const CLIOptions& cli_options)
    -> std::optional<ImageData> {
  // TODO(sergey): Use File from TinyCore to help dealing with non-ASCII paths.

  ImageData image_data;

  image_data.pixels_data =
      stbi_load(cli_options.input_image_filepath.generic_string().c_str(),
                &image_data.width,
                &image_data.height,
                &image_data.num_channels,
                STBI_default);

  if (!image_data.pixels_data) {
    cerr << "Error reading " << cli_options.input_image_filepath << endl;
    return {};
  }

  image_data.pixels = std::span(
      image_data.pixels_data,
      size_t(image_data.width) * image_data.height * image_data.num_channels);

  // Make sure image has correct resolution.
  // Allow bigger images and let the encoder to crop them.

  const Mode mode = GetModeFromName(cli_options.mode_str);
  const ModeSpec<float> mode_spec = ModeSpec<float>::Get(mode);

  if (image_data.width < mode_spec.image_width ||
      image_data.height < mode_spec.image_height) {
    cerr << "Invalid image resolution " << image_data.width << "x"
         << image_data.height << ": the mode expects at least "
         << mode_spec.image_width << "x" << mode_spec.image_height << endl;
    return {};
  }

  if (image_data.num_channels < mode_spec.num_channels) {
    cerr << "Invalid image number of channels " << image_data.num_channels
         << ": the mode expects at least " << mode_spec.num_channels << endl;
    return {};
  }

  return image_data;
}

auto Main(int argc, char** argv) -> bool {
  // Parse command line argument and validate them.
  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);
  if (!CheckCLIOptionsValidOrReport(cli_options)) {
    return EXIT_FAILURE;
  }

  // Read image data.
  const std::optional<ImageData> image_data =
      ReadCheckedImagePixels(cli_options);
  if (!image_data) {
    return EXIT_FAILURE;
  }

  // Construct accessor for the in-memory pixels.
  using PixelAccessor = ConstMemoryPixelAccessor<Color3ub>;
  PixelAccessor pixel_accessor(image_data->pixels,
                               {
                                   .width = image_data->width,
                                   .height = image_data->height,
                                   .num_channels = image_data->num_channels,
                               });

  // Open WAV file for write.
  //
  // NOTE: Only do it after all verification is done, so that we don't override
  // an existing file with 0 size if there is an error in the command line.
  File file;
  if (!file.Open(cli_options.output_wav_filepath,
                 File::kWrite | File::kCreateAlways)) {
    cerr << "Error opening WAV file for write." << endl;
    return EXIT_FAILURE;
  }

  // Configure WAV output.
  const audio_wav_writer::FormatSpec format_spec = {
      .num_channels = 1,
      .sample_rate = cli_options.sample_rate,
      .bit_depth = 16,
  };
  audio_wav_writer::Writer<File> wav_writer;
  if (!wav_writer.Open(file, format_spec)) {
    cerr << "Error opening WAV stream." << endl;
    return EXIT_FAILURE;
  }

  // Configure frequency generator.
  signal::Generator<float> frequency_generator;
  frequency_generator.Configure(cli_options.sample_rate);

  // Construct SSTV encoder.
  const typename Encoder<float>::Options encoder_options{
      .generate_vox = cli_options.generate_vox,
  };
  Encoder<float> sstv_encoder;
  sstv_encoder.Configure(encoder_options);

  // Construct a message and encode it.
  Message message;
  message.mode = GetModeFromName(cli_options.mode_str);
  message.pixel_accessor = &pixel_accessor;

  cout << "Generating " << cli_options.mode_str << " transmission";
  if (encoder_options.generate_vox) {
    cout << " with VOX";
  }
  cout << " ..." << endl;

  auto write_float_sample_cb = [&](const float sample) {
    const float wav_sample[1] = {sample};
    wav_writer.WriteSingleSample<float>(wav_sample);
  };

  sstv_encoder(message, [&](const FrequencyDuration<float>& sample) {
    frequency_generator(sample, write_float_sample_cb);
  });

  frequency_generator.FadeToZero(write_float_sample_cb);

  wav_writer.Close();

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::picture::sstv

int main(int argc, char** argv) {
  return radio_core::picture::sstv::Main(argc, argv);
}
