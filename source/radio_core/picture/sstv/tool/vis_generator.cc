// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Generator of VIS test signal.
//
// Generates VIS sequences with an increasing noise level. The SNR is encoded as
// VIS code. The very first generated VIS is a perfect signal with no noise and
// specified VIS code (default is 0x7f).

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>

#include "radio_core/base/algorithm.h"
#include "radio_core/picture/sstv/vis_encoder.h"
#include "radio_core/signal/awgn_noise_injector.h"
#include "radio_core/signal/generator.h"
#include "tl_audio_wav/tl_audio_wav_writer.h"
#include "tl_io/tl_io_file.h"

namespace radio_core::picture::sstv {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;

namespace audio_wav_writer = tiny_lib::audio_wav_writer;

// Pause between VIS transmissions, in milliseconds.
static float kPauseMilliseconds{250.0f};

// TIme to configure the output for a new SNR level, in milliseconds.
static float kWarmUpMilliseconds{50.0f};

struct CLIOptions {
  inline static constexpr int kDefaultSampleRate{44100};
  inline static constexpr int kDefaultIdealCode{0x7f};
  inline static constexpr int kDefaultNumPackets{4};
  inline static constexpr bool kDefaultNoNoise{false};
  inline static constexpr float kDefaultStartSNR{30.0f};
  inline static constexpr float kDefaultSNRDelta{5.0f};

  std::filesystem::path output_wav_filepath;

  int sample_rate{kDefaultSampleRate};

  int ideal_code{kDefaultIdealCode};

  int num_packets{kDefaultNumPackets};

  bool no_noise{kDefaultNoNoise};
  float start_snr{kDefaultStartSNR};
  float snr_delta{kDefaultSNRDelta};
};

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "VIS Generator", "0.1", argparse::default_arguments::help);

  program.add_description("Generate a sequence of VIS codes for debugging.");

  program.add_argument("output_audio")
      .help("Path to output WAV file with encoded VIS sequences");

  program.add_argument("--rate")
      .default_value(CLIOptions::kDefaultSampleRate)
      .help("Sample rate of the output WAV file")
      .scan<'i', int>();

  program.add_argument("--ideal-code")
      .default_value(CLIOptions::kDefaultIdealCode)
      .help("Code to be used for VIS with ideal SNR")
      .scan<'i', int>();

  program.add_argument("--no-noise")
      .default_value(false)
      .implicit_value(true)
      .help("Disable noise and only generate packets with ideal SNR");

  program.add_argument("--start-snr")
      .default_value(CLIOptions::kDefaultStartSNR)
      .help("SNR of the first noisy transmission")
      .scan<'g', float>();

  program.add_argument("--snr-delta")
      .default_value(CLIOptions::kDefaultSNRDelta)
      .help("Delta in SNR value with every next VIS transmission.")
      .scan<'g', float>();

  program.add_argument("--number")
      .default_value(CLIOptions::kDefaultNumPackets)
      .help(
          "Number of VIS packets to generate, including the ideal transmission")
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

  options.output_wav_filepath = program.get<std::string>("output_audio");
  options.sample_rate = program.get<int>("--rate");
  options.ideal_code = program.get<int>("--ideal-code");
  options.num_packets = program.get<int>("--number");
  options.no_noise = program.get<bool>("--no-noise");
  options.start_snr = program.get<float>("--start-snr");
  options.snr_delta = program.get<float>("--snr-delta");

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

  // Validate number of packets.
  if (cli_options.num_packets <= 0) {
    cerr << "Invalid number of packets." << endl;
    return false;
  }

  return true;
}

struct Pipeline {
  File file;
  audio_wav_writer::Writer<File> wav_writer;
  signal::AWGNNoiseInjector<float> noise_injector;
  signal::Generator<float> frequency_generator;
  VISEncoder<float> vis_encoder;

  // High value, so that there is a smooth transition from non-noisy first
  // VIS and the following noisy VIS.
  float current_snr_db{120};
  bool inject_noise{false};
};

void Write(const float sample, Pipeline& pipeline) {
  const float wav_sample[1] = {sample};
  pipeline.wav_writer.WriteSingleSample<float>(wav_sample);
}

void InjectNoiseIfNeededAndWrite(const float sample, Pipeline& pipeline) {
  float sample_to_write = sample;
  if (pipeline.inject_noise) {
    sample_to_write = pipeline.noise_injector(sample_to_write);
  }
  Write(sample_to_write, pipeline);
}

// Generate pause with the current state of pipeline configuration.
void GeneratePause(Pipeline& pipeline) {
  pipeline.frequency_generator(FrequencyDuration(0.0f, kPauseMilliseconds),
                               InjectNoiseIfNeededAndWrite,
                               pipeline);
}

// Configure noise injector for the new VIS generation and push samples to the
// output to gradually increase the noise if needed.
void ConfigureAndWarmUpNoise(const CLIOptions& cli_options,
                             Pipeline& pipeline,
                             const int vis_index) {
  if (vis_index == 0 || cli_options.no_noise) {
    return;
  }

  // Configure the noise injector.
  signal::AWGNNoiseInjector<float>::Options options = {
      .signal_db = 0.0f,
      .signal_peak_amplitude = 1.0f,
  };

  const float start_snr_db = pipeline.current_snr_db;
  const float target_snr_db =
      cli_options.start_snr + cli_options.snr_delta * (vis_index - 1);

  const int num_steps =
      Max(kWarmUpMilliseconds / 1000.0f * cli_options.sample_rate, 2.0f);
  const float snr_delta_per_step =
      (target_snr_db - start_snr_db) / (num_steps - 1);

  for (int i = 0; i < num_steps; ++i) {
    options.snr_db = start_snr_db + snr_delta_per_step * i;
    pipeline.noise_injector.Configure(options);
    Write(pipeline.noise_injector(0.0f), pipeline);
  }

  pipeline.current_snr_db = target_snr_db;
}

void GenerateNoisyVIS(const CLIOptions& cli_options,
                      Pipeline& pipeline,
                      const int vis_index) {
  uint8_t vis_code = 0;

  if (cli_options.no_noise || vis_index == 0) {
    vis_code = cli_options.ideal_code;

    cout << "Generating VIS without noise" << endl;
    pipeline.inject_noise = false;
  } else {
    ConfigureAndWarmUpNoise(cli_options, pipeline, vis_index);
    vis_code = pipeline.current_snr_db;

    cout << "Generating VIS with " << (pipeline.current_snr_db < 0 ? "" : "+")
         << pipeline.current_snr_db << " db SNR" << endl;
    pipeline.inject_noise = true;
  }

  GeneratePause(pipeline);

  // Generate VIS.
  pipeline.vis_encoder(vis_code,
                       [&pipeline](const FrequencyDuration<float>& sample) {
                         pipeline.frequency_generator(
                             sample, InjectNoiseIfNeededAndWrite, pipeline);
                       });

  // Fade the waveform to 0.
  pipeline.frequency_generator.FadeToZero(Write, pipeline);
}

auto Main(int argc, char** argv) -> int {
  // Parse command line argument and validate them.
  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);
  if (!CheckCLIOptionsValidOrReport(cli_options)) {
    return EXIT_FAILURE;
  }

  Pipeline pipeline;

  // Open WAV file for write.
  //
  // NOTE: Only do it after all verification is done, so that we don't override
  // an existing file with 0 size if there is an error in the command line.
  if (!pipeline.file.Open(cli_options.output_wav_filepath,
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
  if (!pipeline.wav_writer.Open(pipeline.file, format_spec)) {
    cerr << "Error opening WAV stream." << endl;
    return EXIT_FAILURE;
  }

  // Configure frequency generator.
  pipeline.frequency_generator.Configure(cli_options.sample_rate);

  for (int i = 0; i < cli_options.num_packets; ++i) {
    GenerateNoisyVIS(cli_options, pipeline, i);
  }

  GeneratePause(pipeline);

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::picture::sstv

int main(int argc, char** argv) {
  return radio_core::picture::sstv::Main(argc, argv);
}
