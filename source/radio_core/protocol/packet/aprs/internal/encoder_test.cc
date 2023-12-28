// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/packet/aprs/encoder.h"

#include <vector>

#include "radio_core/modulation/digital/fsk/tones_bell.h"
#include "radio_core/protocol/packet/aprs/decoder.h"
#include "radio_core/unittest/test.h"

namespace radio_core::protocol::packet::aprs {

using datalink::ax25::Address;
using datalink::ax25::Message;

// Test encodes an AX.25 message into audio samples and decodes those samples.
//
// This makes the test to rely on the decoder to work properly, which is ensured
// by testing the decoder against pre-recorded audio files. On another hand,
// such a dependency makes the test body more straightforward..
TEST(aprs, Encoder) {
  Message encoded_message;

  encoded_message.address.source = Address("SRC");
  encoded_message.address.destination = Address("DST");
  encoded_message.address.repeaters.TryAppend(Address("RPTR", 12, true));
  encoded_message.information =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
      "eiusmod tempor incididunt ut labore et dolore magna aliqua.";

  std::vector<float> audio_signal;

  {
    Encoder<float>::Options options;
    options.tones = modulation::digital::fsk::kBell202Tones;
    options.sample_rate = 11025;
    options.data_baud = 1200;

    Encoder<float> encoder(options);

    encoder(encoded_message, [&audio_signal](const float sample) {
      audio_signal.push_back(sample);
    });

    // Push extra samples to ensure all encoded samples will go through the
    // filters in the decoder.
    for (int i = 0; i < 1000; ++i) {
      audio_signal.push_back(0);
    }
  }

  // Decode the message from audio data.

  std::vector<Message> messages;

  {
    Decoder<float>::Options options;
    options.tones = modulation::digital::fsk::kBell202Tones;
    options.sample_rate = 11025;
    options.data_baud = 1200;

    Decoder<float> decoder(options);

    for (const float sample : audio_signal) {
      const Decoder<float>::Result result = decoder(sample);
      if (result.Ok()) {
        messages.push_back(result.GetValue());
      }
    }
  }

  // Ensure the message matches the one which was transmitted.

  EXPECT_EQ(messages.size(), 1);
  if (messages.size() == 1) {
    const Message& decoded_message = messages.front();

    EXPECT_EQ(decoded_message.address.source, Address("SRC"));
    EXPECT_EQ(decoded_message.address.destination, Address("DST"));
    EXPECT_EQ(decoded_message.address.repeaters.size(), 1);
    EXPECT_EQ(decoded_message.address.repeaters[0], Address("RPTR", 12, true));
    EXPECT_EQ(
        decoded_message.information,
        std::string_view(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua."));
  }
}

}  // namespace radio_core::protocol::packet::aprs
