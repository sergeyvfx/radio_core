// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/datalink/hdlc/decoder.h"

#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink::hdlc {

using Result = Decoder::Result;
using Error = Decoder::Error;
using FrameBytes = datalink::FrameBytes<Decoder::kMaxFrameBytes>;

// Steram of frame markers without any frame data.
// Nothing is expected to be pushed into the decoder's output.
TEST(Decoder, MarkerStream) {
  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
  }
}

TEST(Decoder, DataOutsizeMarkers) {
  {
    Decoder decoder;

    EXPECT_EQ(decoder(std::byte{0b01011010}), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01111000}), Result(FrameBytes{}));
  }
}

TEST(Decoder, SimpleData) {
  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01011010}),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01011010}),
              })));
    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));
  }

  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01011010}),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01011010}),
              })));
    EXPECT_EQ(decoder(std::byte{0b01111000}),
              Result(FrameBytes({FrameByte(std::byte{0b01111000})})));
    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));
  }
}

TEST(Decoder, MultipleFrames) {
  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01011010}),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01011010}),
              })));
    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01011010}),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01011010}),
              })));
    EXPECT_EQ(decoder(std::byte{0b01111000}),
              Result(FrameBytes({FrameByte(std::byte{0b01111000})})));
    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));
  }

  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01011010}),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01011010}),
              })));

    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));

    EXPECT_EQ(decoder(std::byte{0b01111000}),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01111000}),
              })));
    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));
  }
}

// Test to make sure the decoder is not using an initial reset state as part of
// the marker decoding.
TEST(Decoder, ShiftedStartMarker) {
  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker >> 1), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b01011010}), Result(FrameBytes{}));
    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
  }
}

TEST(Decoder, BitStuffing) {
  {
    Decoder decoder;

    EXPECT_EQ(decoder(Spec::kFrameMarker), Result(FrameBytes{}));
    EXPECT_EQ(decoder(std::byte{0b10111110}), Result(FrameBytes{}));
    EXPECT_EQ(decoder(false),
              Result(FrameBytes({
                  FrameByte(FrameMarker::kBegin),
                  FrameByte(std::byte{0b01111110}),
              })));
    EXPECT_EQ(decoder(Spec::kFrameMarker),
              Result(FrameBytes({FrameByte(FrameMarker::kEnd)})));
  }
}

}  // namespace radio_core::protocol::datalink::hdlc
