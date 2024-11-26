// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// High-Level Data Link Control (HDLC) decoder implementation.
//
// Common information of HDLC can be found in the `spec.h` file.
//
// This is a streamed decoder which does not require having a frame-sized buffer
// to operate. Instead it provides information about frame start/end markers,
// and data byte when those are available.
//
// Some protocol implementations require a constant transmission of HDLC marker
// when there is no data to be transmitted. This decoder will ignore those and
// will only provide frame start marker when frame contains actual data.
//
// The protocol allows to use the same marker to indicate both end of the
// previous frame and beginning of the next one. This decoder converts such
// markers to explicit frame-end and frame-start markers.
//
// The HDLC specification allows he 0-bit at the end of a frame delimiter to be
// shared with the start of the next frame delimiter, i.e. "011111101111110".
// This is not implemented by this decoder.

#pragma once

#include <cstddef>
#include <cstdint>

#include "radio_core/base/result.h"
#include "radio_core/protocol/datalink/frame.h"
#include "radio_core/protocol/datalink/hdlc/spec.h"

namespace radio_core::protocol::datalink::hdlc {

class Decoder {
 public:
  enum class Error {
    // Generic error of an unknown origin.
    kError,
  };

  // Statically sized storage of frame bytes in the result.
  //
  // The result can never have more than 2 frame bytes. The 2 bytes are seen in
  // the output when a first byte of data has been decoded. At that point it
  // becomes obvious that the frame actually contains data (and it is not just
  // a stream of frame markers in the media which is required by some protocols
  // or which is seem in the beginning of many transmissions from the air).
  static constexpr int kMaxFrameBytes = 2;
  using FrameBytes = datalink::FrameBytes<kMaxFrameBytes>;

  using Result = radio_core::Result<FrameBytes, Error>;

  Decoder() = default;

  // Process new bit of an incoming transmission.
  virtual auto operator()(const bool bit) -> Result {
    // Push new bit into the raw buffer.
    raw_bit_buffer_ >>= 1;
    if (bit) {
      raw_bit_buffer_ |= std::byte{0b10000000};
    }

    if (raw_bit_buffer_ == Spec::kFrameMarker) {
      Result result{FrameBytes{}};

      // Got frame marker.

      // When frame marker is found while being inside frame data it indicates
      // end of the frame. Inform the output that the frame has ended.
      if (is_inside_frame_) {
        is_inside_frame_ = false;

        result.GetValue().emplace_back(FrameMarker::kEnd);
      }

      // Bit staffing is only happening for the frame data. Since the marker
      // is found and frame is open (or closed) the staffing counters are to
      // start from 0.
      num_raw_sequential_ones_ = 0;

      data_bit_buffer_ = std::byte{0};
      num_bits_in_data_buffer_ = 0;

      need_open_frame_on_next_data_bit_ = true;

      return result;
    }

    return ProcessDataBit(bit);
  }

  // Process single byte of an incoming transmission.
  auto operator()(const std::byte new_byte) -> Result {
    Result result{FrameBytes{}};

    const auto byte_value = std::to_integer<uint8_t>(new_byte);

    for (int i = 0; i < 8; ++i) {
      const bool bit = byte_value & (1 << i);

      const Result bit_result = (*this)(bit);

      if (!bit_result.Ok()) {
        return bit_result;
      }

      result = bit_result;
    }

    return result;
  }

 private:
  inline auto ProcessDataBit(const bool bit) -> Result {
    Result result{FrameBytes{}};

    // Bit un-stuffing.
    // Ignore forced 0 transition bit after `Spec::kMaxConsecutiveOnes` ones.
    if (bit) {
      // Saturated increment.
      // Avoids integer overflow on a badly formed data stream (i.e. in a very
      // noisy environment).
      if (num_raw_sequential_ones_ < Spec::kMaxConsecutiveOnes) {
        ++num_raw_sequential_ones_;
      }
    } else {
      if (num_raw_sequential_ones_ == Spec::kMaxConsecutiveOnes) {
        // Zero after `Spec::kMaxConsecutiveOnes` was transmitted to force a
        // transition (helping with timing recovery).
        num_raw_sequential_ones_ = 0;
        return result;
      }
      num_raw_sequential_ones_ = 0;
    }

    // Push bit to the data buffer.
    data_bit_buffer_ >>= 1;
    if (bit) {
      data_bit_buffer_ |= std::byte{0b10000000};
    }
    ++num_bits_in_data_buffer_;

    if (num_bits_in_data_buffer_ == 8) {
      if (need_open_frame_on_next_data_bit_) {
        need_open_frame_on_next_data_bit_ = false;
        is_inside_frame_ = true;

        result.GetValue().emplace_back(FrameMarker::kBegin);
      }

      if (is_inside_frame_) {
        result.GetValue().emplace_back(std::byte{data_bit_buffer_});
      }

      data_bit_buffer_ = std::byte{0};
      num_bits_in_data_buffer_ = 0;
    }

    return result;
  }

  // Sliding window of the raw incoming bits.
  // The raw means that bit un-stuffing is not performed on bits written to this
  // buffer.
  // The new bits are written to the most significant bit of this buffer, which
  // means this buffer is stored in native endian.
  //
  // The initial value os chosen such that it is not attempted to be used as a
  // marker until 8 bits were pushed.
  std::byte raw_bit_buffer_{0b11111111};

  // At list this many bits were written to the `raw_bit_buffer_`.
  // int num_at_least_bits_in_raw_buffer_ = 0;

  // Flag of deferred request to open frame.
  // Forces to start a new frame when next data bit is encountered.
  bool need_open_frame_on_next_data_bit_{false};

  // True when the data stream currently belongs to the frame data.
  bool is_inside_frame_{false};

  // Number of binary ones pushed in a row (counted on the bit level) into the
  // raw buffer.
  //
  // Used for bit un-staffing: a zero bit after `Spec::kMaxConsecutiveOnes` ones
  // is ignored as it is a stuffing bit.
  int num_raw_sequential_ones_{0};

  // A buffer for decoded (un-stuffed) bits of frame. Once there is 8 bit in the
  // buffer they are pushed to the output processor.
  std::byte data_bit_buffer_{0};
  int num_bits_in_data_buffer_{0};
};

}  // namespace radio_core::protocol::datalink::hdlc
