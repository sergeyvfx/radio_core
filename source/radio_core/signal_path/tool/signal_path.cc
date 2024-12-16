// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// A signal path processor which demodulates an input quadrature signal which is
// stored as a stereo channel wav file into an audio which is stored as a single
// channel wav file.
//
// The input file is expected to have I signal on channel 1 and Q signal on the
// channel 2.

#include <array>
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>

#include "radio_core/base/half.h"
#include "radio_core/base/scoped_timer.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/half_complex.h"
#include "radio_core/modulation/analog/info.h"
#include "radio_core/signal_path/simple_signal_path.h"
#include "radio_core/tool/buffered_wav_reader.h"
#include "radio_core/tool/log_util.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_audio_wav/tl_audio_wav_writer.h"
#include "tl_io/tl_io_file.h"

namespace radio_core::signal_path {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;

namespace audio_wav_reader = tiny_lib::audio_wav_reader;
namespace audio_wav_writer = tiny_lib::audio_wav_writer;

// NOTE: This is a part of a work-in-progress half-float signal processing.
// While the core math supports half-floats, the signal path configuration
// needs work as there are overflows caused by high sample rate values.
#if RADIO_CORE_HAVE_HALF && 0
using DSPReal = Half;
using DSPComplex = BaseComplex<DSPReal>;
#else
using DSPReal = float;
using DSPComplex = Complex;
#endif

struct CLIOptions {
  inline static constexpr int kDefaultAudioSampleRate{48000};

  std::filesystem::path input_iq_filepath;
  std::filesystem::path output_audio_filepath;

  std::string modulation_str;

  // Value of 0 means the default bandwidth for the modulation type.
  int filter_bandwidth{0};
  int filter_transition{0};

  int audio_sample_rate = kDefaultAudioSampleRate;
  float audio_volume{1.0f};
};

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "signal_path", "0.1", argparse::default_arguments::help);

  program.add_description("Demodulate quadrature signal into audio.");

  program.add_argument("input_iq")
      .help("Path to input WAV file with quadrature signal");

  program.add_argument("output_audio")
      .help("Path to output audio WAV file (- to disable output)");

  program.add_argument("--filter-bandwidth")
      .default_value(0)
      .help(
          "Receive filter bandwidth, Hz (0 for the default modulation type "
          "bandwidth)")
      .scan<'i', int>();

  program.add_argument("--filter-transition")
      .default_value(0)
      .help("Receive filter transition band, Hz")
      .scan<'i', int>();

  program.add_argument("--modulation")
      .help("Modulation type (AM, NFM, WFM, USB, LSB, CW)")
      .required();

  program.add_argument("--audio-rate")
      .default_value(CLIOptions::kDefaultAudioSampleRate)
      .required()
      .help("Sample rate of the output audio WAV file")
      .scan<'i', int>();

  program.add_argument("--audio-volume")
      .default_value(100)
      .required()
      .help("Audio volume, in percentage")
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

  options.input_iq_filepath = program.get<std::string>("input_iq");
  options.output_audio_filepath = program.get<std::string>("output_audio");

  options.filter_bandwidth = program.get<int>("--filter-bandwidth");
  options.filter_transition = program.get<int>("--filter-transition");

  options.modulation_str = program.get<std::string>("--modulation");

  options.audio_sample_rate = program.get<int>("--audio-rate");
  options.audio_volume = float(program.get<int>("--audio-volume")) / 100.0f;

  return options;
}

// Check that the command line options are valid.
//
// Returns true if the options are valid and can be used.
// Reports error and returns false otherwise.
auto CheckCLIOptionsValidOrReport(const CLIOptions& cli_options) -> bool {
  if (cli_options.audio_sample_rate <= 0) {
    cerr << "Invalid audio sample rate." << endl;
    return false;
  }

  if (cli_options.filter_bandwidth < 0) {
    cerr << "Invalid receive filter bandwidth." << endl;
    return false;
  }

  modulation::analog::Type modulation_type;
  if (!modulation::analog::TypeFromName(cli_options.modulation_str,
                                        modulation_type)) {
    cerr << "Unknown or unsupported modulation type "
         << cli_options.modulation_str << endl;
    return false;
  }

  return true;
}

// Configure signal path for the requested command line arguments and the
// configuration of the input IQ WAV file.
//
// Returns true if the configuration succeeded, false otherwise.
//
// The configuration might fail, for example, if there is no supported
// combination of downsamplers to achieve downsampling at different stages.
// The details about it will be logged to the stderr.
auto ConfigureSignalPath(const CLIOptions cli_options,
                         const audio_wav_reader::FormatSpec iq_format_spec,
                         SimpleSignalPath<DSPReal>& signal_path) -> bool {
  if (iq_format_spec.sample_rate % cli_options.audio_sample_rate) {
    cerr << "Non-integer ratio of sample rates at the input and audio stages"
         << endl;
    return false;
  }

  modulation::analog::Type modulation_type;
  if (!modulation::analog::TypeFromName(cli_options.modulation_str,
                                        modulation_type)) {
    // NOTE: The error should have been detected and reported in the
    // CheckCLIOptionsValidOrReport().
    return false;
  }

  SimpleSignalPath<DSPReal>::Options options;

  options.input.sample_rate = iq_format_spec.sample_rate;
  options.input.frequency_shift = 0;

  if (cli_options.filter_bandwidth == 0) {
    options.receive_filter.bandwidth =
        modulation::analog::GetDefaultBandwidth(modulation_type);
    cout << "Using receive filter bandwidth "
         << options.receive_filter.bandwidth << " hertz." << endl;
  } else {
    options.receive_filter.bandwidth = cli_options.filter_bandwidth;
  }

  if (cli_options.filter_transition != 0) {
    options.receive_filter.transition_band_factor =
        float(cli_options.filter_transition) / options.receive_filter.bandwidth;
  }

  options.demodulator.modulation_type = modulation_type;

  const DSPReal fm_deviation = options.receive_filter.bandwidth / 2;
  options.demodulator.nfm.deviation = fm_deviation;
  options.demodulator.wfm.deviation = fm_deviation;

  options.audio.sample_rate = cli_options.audio_sample_rate;
  options.audio.agc.charge_rate = DSPReal(0.007f);
  options.audio.agc.discharge_rate = DSPReal(0.00003f);

  signal_path.Configure(options);

  return true;
}

// Sink of samples to a single-channel WAV file.
template <class WAVWriter, class T>
class WAVFileSink : public SimpleSignalPath<T>::AFSink {
 public:
  WAVFileSink(WAVWriter& wav_writer, const T volume)
      : wav_writer_(&wav_writer), volume_{volume} {}

  void PushSamples(std::span<const T> samples) override {
    for (const T& sample : samples) {
      const float scaled_sample = float(sample * volume_);
      wav_writer_->WriteSingleSample(std::span(&scaled_sample, 1));
    }
  }

 private:
  WAVWriter* wav_writer_{nullptr};
  T volume_{1.0};
};

auto Main(int argc, char** argv) -> int {
  // clang-format off
  cout
    << "**********************************************************************"
    << endl
    << "** Radio Signal Path" << endl
    << "**********************************************************************"
    << endl;
  // clang-format on

  // Parse command line argument and validate them.
  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);
  if (!CheckCLIOptionsValidOrReport(cli_options)) {
    return EXIT_FAILURE;
  }

  // Open input IQ WAV file for read.
  File iq_file;
  if (!iq_file.Open(cli_options.input_iq_filepath, File::kRead)) {
    cerr << "Error opening IQ WAV file for read." << endl;
    return EXIT_FAILURE;
  }

  // Open input IQ WAV reader to access format of the file.
  audio_wav_reader::Reader<File> iq_wav_file_reader;
  if (!iq_wav_file_reader.Open(iq_file)) {
    cerr << "Error reading input IQ WAV file." << endl;
    return EXIT_FAILURE;
  }

  // Open input IQ file and print its information.
  const audio_wav_reader::FormatSpec iq_format_spec =
      iq_wav_file_reader.GetFormatSpec();
  const float iq_file_duration_in_seconds =
      iq_wav_file_reader.GetDurationInSeconds();

  cout << endl;
  cout << "Input file specification" << endl;
  cout << "========================" << endl;
  cout << iq_format_spec.sample_rate << " samples per second, "
       << iq_format_spec.bit_depth << " bits depth, "
       << iq_format_spec.num_channels << " audio channel(s)." << endl;

  cout << "File duration: " << iq_file_duration_in_seconds << " seconds."
       << endl;

  // Validate the channel configuration.
  if (iq_format_spec.num_channels < 2) {
    cerr << "The processor requires at least 2 channels in the IQ WAV file."
         << endl;
    return EXIT_FAILURE;
  }

  // Configure the signal processing path.
  SimpleSignalPath<DSPReal> signal_path;
  if (!ConfigureSignalPath(cli_options, iq_format_spec, signal_path)) {
    return false;
  }

  // Print derived configuration.
  cout << endl;

  cout << "Signal path configuration" << endl;
  cout << "=========================" << endl;

  cout << endl;
  cout << "Sample rate at stages (samples per second)" << endl;
  cout << "------------------------------------------" << endl;
  cout << "  Input : " << signal_path.GetInputSampleRate() << endl;
  cout << "     IF : " << signal_path.GetIFSampleRate() << endl;
  cout << "     AF : " << signal_path.GetAFSampleRate() << endl;

  cout << endl;
  cout << "Receive filter" << endl;
  cout << "--------------" << endl;
  cout << "  Decimation ratio   : "
       << signal_path.GetReceiveFilterDecimationRatio() << endl;
  cout << "  Number of taps     : " << signal_path.GetReceiveFilterKernelSize()
       << endl;
  cout << "     Bandwidth       : " << signal_path.GetReceiveFilterBandwidth()
       << " Hz" << endl;
  cout << "     Transition band : "
       << signal_path.GetReceiveFilterTransitionBand() << " Hz" << endl;

  // Open WAV file for write.
  //
  // NOTE: Only do it after all verification is done, so that we don't override
  // an existing file with 0 size if there is an error in the command line.
  File audio_file;
  audio_wav_writer::Writer<File> audio_wav_writer;
  WAVFileSink<audio_wav_writer::Writer<File>, DSPReal> audio_sink(
      audio_wav_writer, cli_options.audio_volume);
  bool is_output_open = false;
  if (cli_options.output_audio_filepath != "-") {
    if (!audio_file.Open(cli_options.output_audio_filepath,
                         File::kWrite | File::kCreateAlways)) {
      cerr << "Error opening audio WAV file for write." << endl;
      return EXIT_FAILURE;
    }

    // Configure WAV writer.
    const audio_wav_writer::FormatSpec audio_format_spec = {
        .num_channels = 1,
        .sample_rate = cli_options.audio_sample_rate,
        .bit_depth = 16,
    };
    if (!audio_wav_writer.Open(audio_file, audio_format_spec)) {
      cerr << "Error opening audio WAV stream." << endl;
      return EXIT_FAILURE;
    }

    signal_path.AddAFSink(audio_sink);

    is_output_open = true;
  }

  const ScopedTimer scoped_timer;

  float dsp_time = 0;
  tool::ReadWAVBuffered<DSPComplex, 65536>(
      iq_wav_file_reader,
      [](const std::span<const float> frame_samples) -> DSPComplex {
        assert(frame_samples.size() >= 2);
        return {frame_samples[0], frame_samples[1]};
      },
      [&](const std::span<const DSPComplex> samples) {
        const ScopedTimer dsp_scoped_timer;
        signal_path.PushSamples(samples);
        dsp_time += dsp_scoped_timer.GetElapsedTimeInSeconds();
      });

  // Close the output stream, it needed
  if (is_output_open) {
    if (!audio_wav_writer.Close()) {
      cerr << "Error closing audio WAV stream." << endl;
      return EXIT_FAILURE;
    }
  }

  cout << endl;
  cout << "Statistics" << endl;
  cout << "==========" << endl;
  cout << "Processing took "
       << tool::LogTimeWithRealtimeComparison(
              scoped_timer.GetElapsedTimeInSeconds(),
              iq_file_duration_in_seconds)
       << endl;
  cout << "  DSP took "
       << tool::LogTimeWithRealtimeComparison(dsp_time,
                                              iq_file_duration_in_seconds)
       << endl;

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::signal_path

int main(int argc, char** argv) {
  return radio_core::signal_path::Main(argc, argv);
}
