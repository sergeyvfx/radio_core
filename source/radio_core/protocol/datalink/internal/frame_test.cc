// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/datalink/frame.h"

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink {

using testing::Eq;
using testing::Pointwise;

TEST(FrameBytes, Construct) {
  {
    const FrameBytes<2> frame_bytes;
    EXPECT_EQ(frame_bytes.size(), 0);
    EXPECT_EQ(frame_bytes.capacity(), 2);
  }

  {
    const FrameBytes<2> frame_bytes({FrameByte(std::byte{18})});
    EXPECT_EQ(frame_bytes.size(), 1);
    EXPECT_EQ(frame_bytes.capacity(), 2);
    EXPECT_THAT(
        frame_bytes,
        Pointwise(Eq(), std::to_array<FrameByte>({FrameByte(std::byte{18})})));
  }

  {
    const FrameBytes<2> frame_bytes({FrameByte(FrameMarker::kEnd)});
    EXPECT_EQ(frame_bytes.size(), 1);
    EXPECT_EQ(frame_bytes.capacity(), 2);
    EXPECT_THAT(
        frame_bytes,
        Pointwise(Eq(),
                  std::to_array<FrameByte>({FrameByte(FrameMarker::kEnd)})));
  }

  {
    const FrameBytes<3> frame_bytes({FrameByte(FrameMarker::kBegin),
                                     FrameByte(std::byte{18}),
                                     FrameByte(FrameMarker::kEnd)});
    EXPECT_EQ(frame_bytes.size(), 3);
    EXPECT_EQ(frame_bytes.capacity(), 3);
    EXPECT_THAT(
        frame_bytes,
        Pointwise(Eq(),
                  std::to_array<FrameByte>({FrameByte(FrameMarker::kBegin),
                                            FrameByte(std::byte{18}),
                                            FrameByte(FrameMarker::kEnd)})));
  }
}

TEST(FrameBytes, ElementAccess) {
  const FrameBytes<3> frame_bytes(
      {FrameByte(std::byte{10}), FrameByte(FrameMarker::kBegin)});

  EXPECT_EQ(frame_bytes[0], FrameByte(std::byte{10}));
  EXPECT_EQ(frame_bytes.at(0), FrameByte(std::byte{10}));

  EXPECT_EQ(frame_bytes[1], FrameByte(FrameMarker::kBegin));
  EXPECT_EQ(frame_bytes.at(1), FrameByte(FrameMarker::kBegin));
}

TEST(FrameBytes, Comparison) {
  EXPECT_NE(FrameBytes<2>({FrameByte(std::byte{10})}), FrameBytes<2>());
  EXPECT_NE(FrameBytes<2>(), FrameBytes<2>({FrameByte(std::byte{10})}));

  EXPECT_EQ(FrameBytes<2>({FrameByte(std::byte{10})}),
            FrameBytes<2>({FrameByte(std::byte{10})}));
  EXPECT_NE(FrameBytes<2>({FrameByte(std::byte{20})}),
            FrameBytes<2>({FrameByte(std::byte{10})}));

  EXPECT_EQ(FrameBytes<2>({FrameByte(std::byte{10})}),
            FrameBytes<8>({FrameByte(std::byte{10})}));
}

}  // namespace radio_core::protocol::datalink
