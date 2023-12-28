// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of images into APT transmission.
//
// A pair of images is read from disk, encoded into APT, and stored into a
// WAV file.

#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <argparse/argparse.hpp>

#include "radio_core/math/color.h"
#include "radio_core/picture/apt/encoder.h"
#include "radio_core/picture/apt/info.h"
#include "radio_core/picture/memory_pixel_accessor.h"
#include "tl_audio_wav/tl_audio_wav_writer.h"
#include "tl_io/tl_io_file.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace radio_core::picture::apt {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;

namespace audio_wav_writer = tiny_lib::audio_wav_writer;

struct CLIOptions {
  inline static constexpr int kDefaultSampleRate = 44100;

  std::filesystem::path input_image_a_filepath;
  std::filesystem::path input_image_b_filepath;
  std::filesystem::path output_wav_filepath;

  int sample_rate = kDefaultSampleRate;
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

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "apt_encoder", "0.1", argparse::default_arguments::help);

  program.add_description(
      "Encode image into APT transmission stored in a WAV file.");

  program.add_argument("input_image_a").help("Image A of the APT transmission");
  program.add_argument("input_image_b").help("Image B of the APT transmission");

  program.add_argument("output_audio")
      .help("Path to output WAV file with encoded image transmission");

  program.add_argument("--rate")
      .default_value(CLIOptions::kDefaultSampleRate)
      .required()
      .help("Sample rate of the output WAV file")
      .scan<'i', int>();

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

  options.input_image_a_filepath = program.get<std::string>("input_image_a");
  options.input_image_b_filepath = program.get<std::string>("input_image_b");
  options.output_wav_filepath = program.get<std::string>("output_audio");
  options.sample_rate = program.get<int>("--rate");

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
auto ReadCheckedImagePixels(const std::filesystem::path& filepath)
    -> std::optional<ImageData> {
  // TODO(sergey): Use File from TinyCore to help dealing with non-ASCII paths.

  ImageData image_data;

  image_data.pixels_data = stbi_load(filepath.generic_string().c_str(),
                                     &image_data.width,
                                     &image_data.height,
                                     &image_data.num_channels,
                                     STBI_default);

  if (!image_data.pixels_data) {
    cerr << "Error reading " << filepath << endl;
    return {};
  }

  image_data.pixels = std::span(
      image_data.pixels_data,
      size_t(image_data.width) * image_data.height * image_data.num_channels);

  // Make sure image has correct resolution.
  // Allow bigger images and let the encoder to crop them.

  if (image_data.width < Info::kImageWidth) {
    cerr << "Invalid image width " << image_data.width
         << ": the APT encoder expects at least " << Info::kImageWidth << endl;
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
  const std::optional<ImageData> image_a_data =
      ReadCheckedImagePixels(cli_options.input_image_a_filepath);
  if (!image_a_data) {
    return EXIT_FAILURE;
  }
  const std::optional<ImageData> image_b_data =
      ReadCheckedImagePixels(cli_options.input_image_b_filepath);
  if (!image_b_data) {
    return EXIT_FAILURE;
  }

  // Check height.
  if (image_a_data->height != image_b_data->height) {
    cerr << "The images expected to have matched height" << endl;
    return EXIT_FAILURE;
  }

  // Construct accessors for the in-memory pixels.
  using PixelAccessor = ConstMemoryPixelAccessor<Color1ub>;
  PixelAccessor pixel_accessor_a(image_a_data->pixels,
                                 {
                                     .width = image_a_data->width,
                                     .height = image_a_data->height,
                                     .num_channels = image_a_data->num_channels,
                                 });
  PixelAccessor pixel_accessor_b(image_b_data->pixels,
                                 {
                                     .width = image_b_data->width,
                                     .height = image_b_data->height,
                                     .num_channels = image_b_data->num_channels,
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

  // Construct APT encoder.
  const typename Encoder<float>::Options encoder_options{
      .sample_rate = float(cli_options.sample_rate),
  };
  Encoder<float> apt_encoder;
  apt_encoder.Configure(encoder_options);

  // Construct a message and encode it.
  Message message;
  message.pixel_accessor_a = &pixel_accessor_a;
  message.pixel_accessor_b = &pixel_accessor_b;

  cout << "Generating transmission ..." << endl;

  auto write_float_sample_cb = [&](const float sample) {
    const float wav_sample[1] = {sample};
    wav_writer.WriteSingleSample<float>(wav_sample);
  };

  apt_encoder(message,
              [&](const float sample) { write_float_sample_cb(sample); });

  wav_writer.Close();

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::picture::apt

int main(int argc, char** argv) {
  return radio_core::picture::apt::Main(argc, argv);
}
