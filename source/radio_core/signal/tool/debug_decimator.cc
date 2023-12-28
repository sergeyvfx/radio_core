// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Debug tool which prints original and decimated signals.
//
// The signals and decimation ratio is hard-coded.
//
// This tool is only supposed to be used when working on the decimator
// implementation to ease visualization of signals.
//
// Can be used together with the plot_debug_signal.py tool:
//
//   ./bin/tool/debug_decimator && ../source/tool/plot_debug_signal.py

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <vector>

#include <argparse/argparse.hpp>

#include "radio_core/signal/debug_writer.h"
#include "radio_core/signal/decimator.h"
#include "radio_core/signal/local_oscillator.h"

namespace radio_core::signal {
namespace {

using std::cerr;
using std::cout;
using std::endl;

struct CLIOptions {
  std::string output_filepath;
};

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "debug_decimator", "0.1", argparse::default_arguments::help);

  program.add_argument("output_file")
      .default_value(std::string(""))
      .required()
      .help("Path to the output file (empty to output to STDOUT)");

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
  constexpr float kFrequency = 5000;
  constexpr int kInputSampleRate = 240000;
  constexpr int kNumInputSamples = 2500;
  constexpr int kDecimationRatio = 5;

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

  std::vector<float> decimated_samples_storage;
  std::span<float> decimated_samples;
  {
    Decimator<float> decimator;
    decimator.SetRatio(kDecimationRatio);

    decimated_samples_storage.resize(
        decimator.CalcNeededOutputBufferSize(input_samples.size()));

    decimated_samples = decimator(input_samples, decimated_samples_storage);
  }

  signal_writer.WriteSignal(
      "decimated", decimated_samples, kInputSampleRate / kDecimationRatio);

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::signal

auto main(int argc, char** argv) -> int {
  return radio_core::signal::Main(argc, argv);
}
