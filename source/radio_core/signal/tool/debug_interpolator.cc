// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Debug tool which prints original and interpolated signals.
//
// The signals and interpolation ratio is hard-coded.
//
// This tool is only supposed to be used when working on the interpolator
// implementation to ease visualization of signals.
//
// Can be used together with the plot_debug_signal.py tool:
//
//   ./bin/tool/debug_interpolator && ../source/tool/plot_debug_signal.py

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <vector>

#include <argparse/argparse.hpp>

#include "radio_core/signal/debug_writer.h"
#include "radio_core/signal/interpolator.h"
#include "radio_core/signal/local_oscillator.h"

namespace radio_core::signal {
namespace {

using std::cerr;
using std::cout;
using std::endl;

struct CLIOptions {
  std::string output_filepath;
};

// Parse command line arguments and return parsed result.
// MOTE: No sanity check on the options is done here.
auto ParseCLIAndGetOptions(const int argc, char** argv) -> CLIOptions {
  argparse::ArgumentParser program(
      "debug_interpolator", "0.1", argparse::default_arguments::help);

  program.add_argument("output_file")
      .default_value(std::string(""))
      .required()
      .help("Path to the output file (empty to output to STDOUT)");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    cerr << err.what() << endl;
    cerr << endl << program;
    ::exit(1);
  }

  CLIOptions options;

  options.output_filepath = program.get<std::string>("output_file");

  return options;
}

// Check that the command line options are valid.
//
// Returns true if the options are valid and can be used.
// Reports error and returns false otherwise.
auto CheckCLIOptionsValidOrReport(const CLIOptions& /*cli_options*/) -> bool {
  return true;
}

auto Main(int argc, char** argv) -> int {
  // Parse command line argument and validate them.
  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);
  if (!CheckCLIOptionsValidOrReport(cli_options)) {
    return EXIT_FAILURE;
  }

  // Open the output file if needed.
  DebugWriter signal_writer;
  if (!cli_options.output_filepath.empty()) {
    if (!signal_writer.Open(cli_options.output_filepath)) {
      return EXIT_FAILURE;
    }
  } else {
    signal_writer.Open(stdout);
  }

  // Configuration.
  constexpr float kFrequency = 1000;
  constexpr int kInputSampleRate = 12000;
  constexpr int kNumInputSamples = 2500;
  constexpr int kInterpolationRatio = 5;

  // Generate input samples.
  std::vector<float> input_samples;
  {
    LocalOscillator<float> local_oscillator;
    local_oscillator.Configure(kFrequency, kInputSampleRate);

    input_samples.reserve(kNumInputSamples);
    for (int i = 0; i < kNumInputSamples; ++i) {
      input_samples.push_back(local_oscillator.Sine());
    }
  }

  signal_writer.WriteSignal("input", input_samples, kInputSampleRate);

  std::vector<float> interpolated_samples_storage;
  std::span<float> interpolated_samples;
  {
    Interpolator<float> interpolator;
    interpolator.SetRatio(kInterpolationRatio);

    interpolated_samples_storage.resize(
        interpolator.CalcNeededOutputBufferSize(input_samples.size()));

    interpolated_samples =
        interpolator(input_samples, interpolated_samples_storage);
  }

  signal_writer.WriteSignal("interpolated",
                            interpolated_samples,
                            kInputSampleRate * kInterpolationRatio);

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::signal

auto main(int argc, char** argv) -> int {
  return radio_core::signal::Main(argc, argv);
}
