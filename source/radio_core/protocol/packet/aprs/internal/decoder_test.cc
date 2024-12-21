// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/packet/aprs/decoder.h"

#include <algorithm>
#include <filesystem>
#include <set>
#include <span>
#include <vector>

#include "gflags/gflags.h"
#include "tl_audio_wav/tl_audio_wav_reader.h"
#include "tl_io/tl_io_file.h"

#include "radio_core/base/convert.h"
#include "radio_core/modulation/digital/fsk/tones_bell.h"
#include "radio_core/unittest/test.h"

namespace radio_core::protocol::packet::aprs {

using datalink::ax25::Address;
using datalink::ax25::Message;
using Path = std::filesystem::path;

// Base fixture for all APRS tests which use AX.25 framing.
//
// Takes care of the boilerplate needed to configure decoder and feed audio
// samples to it.
class BaseAX25Test : public ::testing::Test {
 protected:
  // Get path to a file from the data folder of this test suit.
  static auto GetDataFilepath(const Path& filename) -> Path {
    return testing::TestFileAbsolutePath(Path("aprs") / filename);
  }

  // Configure decoder with the given options and decode all possible messages
  // form the given file.
  //
  // The sample rate is used the file and is ignored from the given options.
  //
  // File name is expected to be relative to the test_srcdir.
  //
  // Returns an ordered collection of all decoded messages.
  auto DecodeAllMessagesFromFile(
      const Decoder<float>::Options& options_template, const Path& filename)
      -> std::vector<Message> {
    using tiny_lib::io_file::File;
    using WAVReader = tiny_lib::audio_wav_reader::Reader<File>;
    using tiny_lib::audio_wav_reader::FormatSpec;

    File file;
    EXPECT_TRUE(file.Open(GetDataFilepath(filename), File::kRead));

    WAVReader wav_reader;
    EXPECT_TRUE(wav_reader.Open(file));

    const FormatSpec& format_spec = wav_reader.GetFormatSpec();

    Decoder<float>::Options options = options_template;
    options.sample_rate = float(format_spec.sample_rate);

    Decoder<float> decoder(options);

    std::vector<Message> messages;

    const bool read_result = wav_reader.ReadAllSamples<float, 2>(
        [&messages, &decoder](const std::span<float> sample) {
          const Decoder<float>::Result result = decoder(sample[0]);
          if (result.Ok()) {
            const Message& message = result.GetValue();
            messages.push_back(message);
          }
        });
    EXPECT_TRUE(read_result);

    return messages;
  }
};

// Base class for APRS protocol which uses Bell 202 tone and 1200 baud.
class BaseAX25Bell202Tone1200bdTest : public BaseAX25Test {
 protected:
  auto DecodeAllMessagesFromFile(const Path& filename) -> std::vector<Message> {
    Decoder<float>::Options options;
    options.tones = modulation::digital::fsk::kBell202Tones;
    options.sample_rate = 0;
    options.data_baud = 1200;

    return BaseAX25Test::DecodeAllMessagesFromFile(options, filename);
  }
};

////////////////////////////////////////////////////////////////////////////////
// Tests for messages generated with the `aprs_encoder` tool from this
// repository with the following arguments:
//
//   aprs_encoder
//       --source SRC
//       --destination DST
//       --repeater 'RPTR-12*'
//       --message "<lorem ipsum first sentence>"
//       --rate <rate>
//       <output.wav>

class AX25Bell202Tone1200bdLoremTest : public BaseAX25Bell202Tone1200bdTest {
 protected:
  void Run(const Path& filename) {
    const std::vector<Message> messages = DecodeAllMessagesFromFile(filename);

    EXPECT_EQ(messages.size(), 1);

    if (messages.size() != 1) {
      return;
    }

    const Message& message = messages[0];

    EXPECT_EQ(message.address.source, Address("SRC"));
    EXPECT_EQ(message.address.destination, Address("DST"));
    EXPECT_EQ(message.address.repeaters.size(), 1);
    EXPECT_EQ(message.address.repeaters[0], Address("RPTR", 12, true));
    EXPECT_EQ(
        message.information,
        std::string_view(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua."));
  }
};

TEST_F(AX25Bell202Tone1200bdLoremTest, sps11025) {
  Run("ax25_bell202_1200bd_lorem_11025.wav");
}

TEST_F(AX25Bell202Tone1200bdLoremTest, sps44100) {
  Run("ax25_bell202_1200bd_lorem_44100.wav");
}

////////////////////////////////////////////////////////////////////////////////
// Tests for messages generated with the `gen_packets` tool from DireWolf with
// the following parameters:
//
//   gen_packets -n 100 -r <sample rate> -o <output.wav>
//
// Check that the exact set of messages has been decoded (as opposite to, for
// example, count) to detect cases when some change in the modem allowed to
// decode some ne messages, but did not decode some previously decoded messages.

class AX25Bell202Tone1200bdDireWolfTest : public BaseAX25Bell202Tone1200bdTest {
 protected:
  void Run(const Path& filename, const std::span<const int> expected_indices) {
    const std::vector<Message> messages = DecodeAllMessagesFromFile(filename);

    EXPECT_EQ(messages.size(), expected_indices.size());

    std::set<int> decoded_indiced;
    for (const datalink::ax25::Message& message : messages) {
      using datalink::ax25::Address;
      using datalink::ax25::Callsign;

      EXPECT_EQ(message.address.source, Address("WB2OSZ", 15));
      EXPECT_EQ(message.address.destination, Address("TEST", 0));
      EXPECT_EQ(message.address.repeaters.size(), 0);

      const std::string_view info = message.information.GetCleanView();

      EXPECT_TRUE(
          info.starts_with(",The quick brown fox jumps over the lazy dog!"));

      const std::string_view info_suffix = info.substr(info.size() - 12);
      const std::string_view index_str = info_suffix.substr(0, 4);

      const int index = StringToInt<int>(index_str);

      const bool is_message_expected =
          std::find(expected_indices.begin(), expected_indices.end(), index) !=
          expected_indices.end();

      EXPECT_TRUE(is_message_expected)
          << "Decoded unexpected message index " << index;

      decoded_indiced.insert(index);
    }

    for (const int expected_index : expected_indices) {
      EXPECT_TRUE(decoded_indiced.contains(expected_index))
          << "Did not decode message with index " << expected_index;
    }
  }
};

TEST_F(AX25Bell202Tone1200bdDireWolfTest, sps11025) {
  Run("ax25_bell202_1200bd_dw_11025.wav",
      {{
          1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
          18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 38, 40,
      }});
}

TEST_F(AX25Bell202Tone1200bdDireWolfTest, sps44100) {
  Run("ax25_bell202_1200bd_dw_44100.wav",
      {{
          1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
          16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
          31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
          46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 61,
          62, 63, 64, 65, 66, 67, 69, 70, 71, 73, 75, 79, 80, 83,
      }});
}

////////////////////////////////////////////////////////////////////////////////
// Off-the-air recordings.

class AX25Bell202Tone1200bdTest : public BaseAX25Bell202Tone1200bdTest {
 protected:
  auto ReadMessage(const Path& filename) -> std::optional<Message> {
    const std::vector<Message> messages = DecodeAllMessagesFromFile(filename);

    EXPECT_EQ(messages.size(), 1);

    if (messages.size() != 1) {
      return std::nullopt;
    }

    return messages[0];
  }
};

TEST_F(AX25Bell202Tone1200bdTest, Supervisory) {
  std::optional<Message> message =
      ReadMessage("ax25_bell202_1200bd_supervisory.wav");
  if (!message.has_value()) {
    return;
  }

  EXPECT_EQ(message->address.source, Address("SRC"));
  EXPECT_EQ(message->address.source.command_response_bit, 1);

  EXPECT_EQ(message->address.destination, Address("DST"));
  EXPECT_EQ(message->address.destination.command_response_bit, 0);

  EXPECT_EQ(message->address.repeaters.size(), 1);
  EXPECT_EQ(message->address.repeaters[0], Address("RPTR", 12, true));

  EXPECT_EQ(message->control, 17);
  EXPECT_EQ(message->pid, 0);
}

////////////////////////////////////////////////////////////////////////////////
// TNC Test CD.

class TNCTestCDTest : public BaseAX25Bell202Tone1200bdTest {
 protected:
  void SetUp() override {
#if !defined(WITH_TESTS_TNC_CD)
    GTEST_SKIP() << "Skip TNC Test CD tests per build configuration";
#endif
  }

  void Run(const Path& filename, const int num_expected_messages) {
    const Path relative_filepath = Path("tnc_test_cd") / filename;

    const Path filepath = GetDataFilepath(relative_filepath);
    EXPECT_TRUE(std::filesystem::exists(filepath));

    const std::vector<Message> messages =
        DecodeAllMessagesFromFile(relative_filepath);

    EXPECT_EQ(messages.size(), num_expected_messages);

    // TODO(sergey): Somehow test for the decoded messages as well, to help
    // figuring out situations when a change in the decoder decoded some extra
    // messages but did not decode previously decodable messages.
  }

 private:
};

TEST_F(TNCTestCDTest, Track_01_11025) { Run("track_01_11025.wav", 1008); }

TEST_F(TNCTestCDTest, Track_02_11025) { Run("track_02_11025.wav", 981); }

TEST_F(TNCTestCDTest, Track_01_44100) { Run("track_01_44100.wav", 1011); }

TEST_F(TNCTestCDTest, Track_02_44100) { Run("track_02_44100.wav", 963); }

}  // namespace radio_core::protocol::packet::aprs
