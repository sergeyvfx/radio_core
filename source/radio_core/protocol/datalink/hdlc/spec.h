// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// High-Level Data Link Control (HDLC).
//
// Some basic information about HDLC:
//
//  - Frame beginning and ending is indicated by 0b01111110 sequence.
//    Note that this is the only place where 6 ones can be seen in a row.
//
//  - HDLC transmits bytes of data with the least significant bit first.
//
//  - Data is encoded so that it never contains more than 5 ones in a row.
//    Bit stuffing is sued for this: any time when five consecutive 1-bits
//    appear in the transmitted data, the data is paused and a 0-bit is
//    transmitted.
//
// In networking the HDLC is a network level protocol, but in practice many
// other protocols are using HDLC syntax, making it more of a method than a
// concrete implementation. This is why this HDLC encoding/decoding logic is
// implemented on the data link level and is only taking care of framing.
//
// The semantic of the frame content is left up to a specific protocol derived
// from HDLC such as AX.25.
//
// This file contains constants HDLC specification needed by both encoding and
// decoding process.

#pragma once

#include <cstddef>

namespace radio_core::protocol::datalink::hdlc {

struct Spec {
  // Marker used to indicate beginning and ending of a frame.
  inline static constexpr std::byte kFrameMarker{0b01111110};

  inline static constexpr int kMaxConsecutiveOnes{5};
};

}  // namespace radio_core::protocol::datalink::hdlc
