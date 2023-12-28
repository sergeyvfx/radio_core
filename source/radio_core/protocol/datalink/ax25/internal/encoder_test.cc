// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// There is an online AX.25 frame generator:
//
//   https://notblackmagic.com/bitsnpieces/ax.25/
//
// It doesn't do CRC, but it can be calculated using another online calculator:
//
//   http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//
// using CRC16_X_25 parametrization preset.

#include "radio_core/protocol/datalink/ax25/encoder.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include "radio_core/base/byte_util.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink::ax25 {

using testing::Eq;
using testing::Pointwise;

using FrameData = std::vector<std::byte>;

class BitReceiver {
 public:
  std::vector<FrameData> frames;

  void operator()(const FrameByte& frame_byte) {
    if (frame_byte.IsMarker()) {
      switch (frame_byte.GetMarker()) {
        case FrameMarker::kBegin:
          EXPECT_FALSE(is_frame_open_);
          is_frame_open_ = true;
          break;

        case FrameMarker::kEnd:
          EXPECT_TRUE(is_frame_open_);
          is_frame_open_ = false;
          frames.push_back(current_data_);
          current_data_.clear();
          break;
      }
    }

    if (frame_byte.IsData()) {
      EXPECT_TRUE(is_frame_open_);
      current_data_.push_back(frame_byte.GetData());
    }
  }

 private:
  FrameData current_data_;
  bool is_frame_open_ = false;
};

TEST(Encoder, Simple) {
  Message message;
  message.address.source = Address("N7LEM");
  message.address.destination = Address("NJ7P");
  message.control = ControlBits::Unnumbered::kUI;
  message.pid = PID::kNoLayer3;
  message.information = "Hello, World!";

  Encoder encoder;

  BitReceiver receiver;
  encoder(message, receiver);

  EXPECT_EQ(receiver.frames.size(), 1);

  // clang-format off
  EXPECT_THAT(receiver.frames.at(0),
              Pointwise(Eq(),
                        ToBytesArray({
                          // Destination.
                          0x9c, 0x94, 0x6e, 0xa0, 0x40, 0x40, 0x60,
                          // Source.
                          0x9c, 0x6e, 0x98, 0x8a, 0x9a, 0x40, 0x61,
                          // Control.
                          0x03,
                          // PID.
                          0xf0,
                          // Information.
                          0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
                          0x6f, 0x72, 0x6c, 0x64, 0x21,
                          // FCS.
                          0xff, 0x31,
                        })));
  // clang-format on
}

TEST(Encoder, SSID) {
  Message message;
  message.address.source = Address("N7LEM", 11);
  message.address.destination = Address("NJ7P", 13);
  message.control = ControlBits::Unnumbered::kUI;
  message.pid = PID::kNoLayer3;
  message.information = "Hello, World!";

  Encoder encoder;

  BitReceiver receiver;
  encoder(message, receiver);

  EXPECT_EQ(receiver.frames.size(), 1);

  // clang-format off
  EXPECT_THAT(receiver.frames.at(0),
              Pointwise(Eq(),
                        ToBytesArray({
                          // Destination.
                          0x9c, 0x94, 0x6e, 0xa0, 0x40, 0x40, 0x7a,
                          // Source.
                          0x9c, 0x6e, 0x98, 0x8a, 0x9a, 0x40, 0x77,
                          // Control.
                          0x03,
                          // PID.
                          0xf0,
                          // Information.
                          0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
                          0x6f, 0x72, 0x6c, 0x64, 0x21,
                          // FCS.
                          0xb3, 0x05,
                        })));
  // clang-format on
}

TEST(Encoder, Repeater) {
  Message message;
  message.address.source = Address("N7LEM");
  message.address.destination = Address("NJ7P");
  message.address.repeaters.TryAppend(Address("NA1SS"));
  message.control = ControlBits::Unnumbered::kUI;
  message.pid = PID::kNoLayer3;
  message.information = "Hello, World!";

  Encoder encoder;

  BitReceiver receiver;
  encoder(message, receiver);

  EXPECT_EQ(receiver.frames.size(), 1);

  // clang-format off
  EXPECT_THAT(receiver.frames.at(0),
              Pointwise(Eq(),
                        ToBytesArray({
                          // Destination.
                          0x9c, 0x94, 0x6e, 0xa0, 0x40, 0x40, 0x60,
                          // Source.
                          0x9c, 0x6e, 0x98, 0x8a, 0x9a, 0x40, 0x60,
                          // Repeater.
                          0x9c, 0x82, 0x62, 0xa6, 0xa6, 0x40, 0x61,
                          // Control.
                          0x03,
                          // PID.
                          0xf0,
                          // Information.
                          0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57,
                          0x6f, 0x72, 0x6c, 0x64, 0x21,
                          // FCS.
                          0x12, 0x6d,
                        })));
  // clang-format on
}

}  // namespace radio_core::protocol::datalink::ax25
