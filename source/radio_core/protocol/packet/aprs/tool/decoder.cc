// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of Automatic Packet Reporting System (APRS) messages.
//
// Decodes messages from WAV file using AX.25 framing, FSK modulation, and NRZS
// coding, at 1200 baud.

#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

#include <argparse/argparse.hpp>

#include "radio_core/base/scoped_timer.h"
#include "radio_core/base/string_util.h"
#include "radio_core/modulation/digital/fsk/tones_bell.h"
#include "radio_core/protocol/datalink/ax25/message.h"
#include "radio_core/protocol/datalink/ax25/print.h"
#include "radio_core/protocol/packet/aprs/decoder.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

namespace radio_core::protocol::packet::aprs {
namespace {

using std::cerr;
using std::cout;
using std::endl;

using File = tiny_lib::io_file::File;
using datalink::ax25::Address;
using datalink::ax25::Message;

namespace audio_wav_reader = tiny_lib::audio_wav_reader;

struct CLIOptions {
  inline static constexpr int kDefaultChannel{1};
  inline static constexpr bool kDefaultTerse{false};

  std::filesystem::path input_audio_filepath;
  int audio_channel{kDefaultChannel};

  bool terse{kDefaultTerse};
};

// Configure command line argument parser.
// No actual parsing is done in this function.
auto ConstructCLIParser() -> argparse::ArgumentParser {
  argparse::ArgumentParser program(
      "aprs_decoder", "0.1", argparse::default_arguments::help);

  program.add_description("Message message from AX.25 1200 bps transmission.");

  program.add_argument("input_audio")
      .help("Path to input WAV file with encoded image transmission");

  program.add_argument("--channel")
      .default_value(CLIOptions::kDefaultChannel)
      .help("Channel of audio file to use in 1-based indexing")
      .scan<'i', int>();

  program.add_argument("--terse")
      .default_value(CLIOptions::kDefaultTerse)
      .implicit_value(true)
      .help("Terse output: only summary");

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
  options.audio_channel = program.get<int>("--channel");
  options.terse = program.get<bool>("--terse");

  return options;
}

// TODO(sergey): Consider making these utilities more reusable.

inline void AX25AddressToString(const Address& address,
                                std::span<char> buffer) {
  const std::string_view clean_callsign = address.callsign.GetCleanView();

  if (address.ssid == 0) {
    StringCopy(buffer.data(),
               clean_callsign.data(),
               std::min(clean_callsign.length() + 1, buffer.size()));
    return;
  }

  const int ssid = address.ssid;
  snprintf(buffer.data(),
           buffer.size(),
           "%.*s-%d",
           int(clean_callsign.size()),
           clean_callsign.data(),
           ssid);
}

void PrintMessage(const Message& message) {
  static constexpr int kAddressStrSize = 10;

  std::array<char, kAddressStrSize> src_address;
  AX25AddressToString(message.address.source, src_address);

  std::array<char, kAddressStrSize> dst_address;
  AX25AddressToString(message.address.destination, dst_address);

  std::array<char, 32> encoded_indo;
  datalink::ax25::EncodeMessageInfo(message, encoded_indo);

  printf("\nFm:%s To:%s <%s>\n",
         src_address.data(),
         dst_address.data(),
         encoded_indo.data());

  for (const Address& address : message.address.repeaters) {
    std::array<char, kAddressStrSize> repeater_address;
    AX25AddressToString(address, repeater_address);
    printf("Via:%s%s\n",
           repeater_address.data(),
           address.has_been_repeated ? "*" : "");
  }

  const std::string_view information = message.information.GetCleanView();
  printf("%*s\n\n", int(information.size()), information.data());
}

class AX25MessagePrinter {
 public:
  explicit AX25MessagePrinter(bool terse) : terse_(terse) {}

  void operator()(const Message& message) {
    if (!terse_) {
      PrintMessage(message);
    }

    ++num_messages_;
  }

  inline auto GetNumMessages() const -> int { return num_messages_; }

 private:
  bool terse_ = true;
  int num_messages_ = 0;
};

auto Main(int argc, char** argv) -> int {
  // TODO(sergey): Find a way to break down this function into a separate steps.
  // Need to find a nice way of re-configuring processors without allocating
  // them sequentially.

  const CLIOptions cli_options = ParseCLIAndGetOptions(argc, argv);

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

  const Decoder<float>::Options decoder_options = {
      .tones = modulation::digital::fsk::kBell202Tones,
      .sample_rate = float(format_spec.sample_rate),
      .data_baud = 1200,
  };

  // Decoding pipeline.

  Decoder<float> decoder(decoder_options);

  AX25MessagePrinter message_printer(cli_options.terse);

  const ScopedTimer scoped_timer;

  auto processor = [&](const float sample) {
    const Decoder<float>::Result result = decoder(sample);
    if (result.Ok()) {
      message_printer(result.GetValue());
    }
  };

  wav_file_reader.ReadAllSamples<float, 16>(
      [&](const std::span<const float> sample) {
        processor(sample[cli_options.audio_channel - 1]);
      });

  // Make sure all samples from file are processed and are not being stuck in
  // the filter delays.
  for (int i = 0; i < 1000; ++i) {
    processor(0.0f);
  }

  const float decode_time_in_seconds = scoped_timer.GetElapsedTimeInSeconds();
  cout << endl;
  cout << message_printer.GetNumMessages() << " packets decoded in "
       << decode_time_in_seconds << " seconds";
  if (decode_time_in_seconds != 0) {
    cout << " (" << (file_duration_in_seconds / decode_time_in_seconds)
         << "x realtime)";
  }
  cout << endl;

  return EXIT_SUCCESS;
}

}  // namespace
}  // namespace radio_core::protocol::packet::aprs

auto main(int argc, char** argv) -> int {
  return radio_core::protocol::packet::aprs::Main(argc, argv);
}
