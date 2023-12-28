// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of Automatic Packet Reporting System (APRS) messages.
//
// Encodes messages using AX.25 framing, FSK modulation, and NRZS coding, at
// 1200 baud. The encoded message is written to a WAV file.

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <argparse/argparse.hpp>

#include "radio_core/base/ctype.h"
#include "radio_core/modulation/digital/fsk/tones_bell.h"
#include "radio_core/protocol/datalink/ax25/message.h"
#include "radio_core/protocol/packet/aprs/encoder.h"
#include "tl_audio_wav/tl_audio_wav_writer.h"
#include "tl_io/tl_io_file.h"

namespace radio_core::protocol::packet::aprs {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;
using datalink::ax25::Address;
using datalink::ax25::Message;

namespace audio_wav_writer = tiny_lib::audio_wav_writer;

struct CLIOptions {
  inline static constexpr int kDefaultSampleRate{44100};

  std::filesystem::path output_wav_filepath;

  std::string source;
  std::string destination;
  std::vector<std::string> repeaters;
  std::string message;
  int sample_rate = kDefaultSampleRate;
};

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "aprs_encoder", "0.1", argparse::default_arguments::help);

  program.add_description(
      "Encode message into AX.25 1200 bps packet transmission stored in a WAV "
      "file.");

  program.add_argument("output_audio")
      .help("Path to output WAV file with encoded image transmission");

  program.add_argument("--source")
      .required()
      .help("Callsign and SSID of the source station (<callsign>-<ssid>)");

  program.add_argument("--destination")
      .required()
      .help("Callsign and SSID of the destination station (<callsign>-<ssid>)");

  program.add_argument("--repeater")
      .help(
          "Repeater address in the <callign>-<ssid>[*] format. "
          "The asterisk symbol denotes that the message has been repeated. "
          "Multiple repeater arguments are allowed.")
      .append();

  program.add_argument("--message").required().help("Message to transmit");

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

  options.output_wav_filepath = program.get<std::string>("output_audio");

  options.source = program.get<std::string>("--source");
  options.destination = program.get<std::string>("--destination");

  if (program.is_used("--repeater")) {
    options.repeaters = program.get<std::vector<std::string>>("--repeater");
  }

  options.message = program.get<std::string>("--message");
  options.sample_rate = program.get<int>("--rate");

  return options;
}

auto AddressFromString(std::string_view str, bool use_repeater = false)
    -> Address {
  Address address;

  if (str.ends_with('*')) {
    if (use_repeater) {
      address.has_been_repeated = true;
    }
    str.remove_suffix(1);
  }

  const std::string_view::size_type dash_pos = str.find('-');
  if (dash_pos == std::string_view::npos) {
    address.callsign = str;
    return address;
  }

  const std::string_view callsign = str.substr(0, dash_pos);
  const std::string_view ssid_str = str.substr(dash_pos + 1);
  const std::string_view::size_type ssid_str_len = ssid_str.size();

  int ssid = 0;
  if (ssid_str_len && IsDigit(ssid_str[0])) {
    ssid = ssid_str[0] - '0';
    if (ssid_str_len > 1 && IsDigit(ssid_str[1])) {
      ssid = (ssid_str[1] - '0') + ssid * 10;
    }
  }
  ssid &= 0b1111;

  address.callsign = callsign;
  address.ssid = ssid;

  return address;
}

auto MessageFromOptions(const CLIOptions& options) {
  Message message;
  message.address.source = AddressFromString(options.source);
  message.address.destination = AddressFromString(options.destination);

  for (const std::string& repeater : options.repeaters) {
    message.address.repeaters.TryAppend(AddressFromString(repeater, true));
  }

  message.control = datalink::ax25::ControlBits::Unnumbered::kUI;
  message.pid = datalink::ax25::PID::kNoLayer3;
  message.information = options.message;

  return message;
}

auto Main(int argc, char** argv) -> int {
  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);

  // Validate sample rate.
  if (cli_options.sample_rate <= 0) {
    cerr << "Invalid sample rate." << endl;
    return EXIT_SUCCESS;
  }

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

  // Configure WAV writer.
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

  // Configure the encoder.
  const Encoder<float>::Options encoder_options = {
      .tones = modulation::digital::fsk::kBell202Tones,
      .sample_rate = float(cli_options.sample_rate),
      .data_baud = 1200,
  };
  Encoder<float> encoder(encoder_options);

  // Create the message.
  const Message message = MessageFromOptions(cli_options);

  // Encode the message.
  encoder(message, [&wav_writer](const float sample) {
    float wav_sample[1] = {sample};
    wav_writer.WriteSingleSample<float>(wav_sample);
  });

  // Close the stream.
  if (!wav_writer.Close()) {
    cerr << "Error closing WAV stream." << endl;
    return EXIT_FAILURE;
  }

  cout << "Successfully wrote message to file." << endl;

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::protocol::packet::aprs

auto main(int argc, char** argv) -> int {
  return radio_core::protocol::packet::aprs::Main(argc, argv);
}
