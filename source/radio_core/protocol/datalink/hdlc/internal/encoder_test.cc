// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/datalink/hdlc/encoder.h"

#include <array>
#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink::hdlc {

using testing::Eq;
using testing::Pointwise;

class BitReceiver {
 public:
  std::vector<bool> bits;

  void operator()(const bool bit) { bits.push_back(bit); }
};

// Transmit empty frame.
// Expected to only have frame markers indication begiining/end of the frame.
TEST(Encoder, Empty) {
  Encoder encoder;

  BitReceiver receiver;
  encoder(FrameMarker::kBegin, receiver);
  encoder(FrameMarker::kEnd, receiver);

  // clang-format off
  EXPECT_THAT(receiver.bits,
              Pointwise(Eq(),
                        std::to_array<bool>({
                            0, 1, 1, 1, 1, 1, 1, 0,  // Frame start marker.
                            0, 1, 1, 1, 1, 1, 1, 0,  // Frame end marker.
                        })));
  // clang-format on
}

TEST(Encoder, SimpleByte) {
  Encoder encoder;

  BitReceiver receiver;
  encoder(FrameMarker::kBegin, receiver);
  encoder(std::byte(0xa5), receiver);
  encoder(FrameMarker::kEnd, receiver);

  // clang-format off
  EXPECT_THAT(receiver.bits,
              Pointwise(Eq(),
                        std::to_array<bool>({
                            0, 1, 1, 1, 1, 1, 1, 0,  // Frame start marker.
                            1, 0, 1, 0, 0, 1, 0, 1,  // Binary for 0xa5.
                            0, 1, 1, 1, 1, 1, 1, 0,  // Frame end marker.
                        })));
  // clang-format on
}

TEST(Encoder, StuffedByte) {
  Encoder encoder;

  BitReceiver receiver;
  encoder(FrameMarker::kBegin, receiver);
  encoder(std::byte(0xff), receiver);
  encoder(FrameMarker::kEnd, receiver);

  // clang-format off
  EXPECT_THAT(receiver.bits,
              Pointwise(Eq(),
                        std::to_array<bool>({
                            0, 1, 1, 1, 1, 1, 1, 0,     // Frame start marker.
                            1, 1, 1, 1, 1, 0, 1, 1, 1,  // Binary for 0xff with
                                                        // bit stuffed.
                            0, 1, 1, 1, 1, 1, 1, 0,     // Frame end marker.
                        })));
  // clang-format on
}

}  // namespace radio_core::protocol::datalink::hdlc
