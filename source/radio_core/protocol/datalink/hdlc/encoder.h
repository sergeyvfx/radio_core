// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// High-Level Data Link Control (HDLC) encoder implementation.
//
// Common information of HDLC can be found in the the `spec.h` file.
//
// The encoder receives frame data, wraps it in-between frame markers, performs
// bit stuffing when needed, and sends the encoded bits to a writer.
//
// NOTE: The encoder does not take care of NRZ encoding, allowing it to
// optionally use or replace it with something else.

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

#include "radio_core/protocol/datalink/frame.h"
#include "radio_core/protocol/datalink/hdlc/spec.h"

namespace radio_core::protocol::datalink::hdlc {

class Encoder {
 public:
  Encoder() = default;

  // Process frame marker and write its encoded bits via the given callback..
  //
  // The callback will be called with the given list of arguments and the
  // encoded bit as the last argument. The bit is passed as a boolean value.
  //
  // The encoded bits are passed to the callback, one per the callback
  // invocation. The given list of args... is passed to the callback after the
  // bit. This makes the required callback signature to be:
  //
  //   callback(bool bit, <optional arguments>)
  template <class F, class... Args>
  inline void operator()(const FrameMarker /*marker*/,
                         F&& callback,
                         Args&&... args) {
    WriteMarker(std::forward<F>(callback), std::forward<Args>(args)...);
  }

  // Process byte of data and write its encoded bits via the given callback..
  //
  // The encoded bits are passed to the callback, one per the callback
  // invocation. The given list of args... is passed to the callback after the
  // bit. This makes the required callback signature to be:
  //
  //   callback(bool bit, <optional arguments>)
  template <class F, class... Args>
  inline void operator()(const std::byte new_data_byte,
                         F&& callback,
                         Args&&... args) {
    const auto data_byte_value = std::to_integer<uint8_t>(new_data_byte);
    for (int i = 0; i < 8; ++i) {
      const bool bit = data_byte_value & (1 << i);
      PushDataBit(bit, std::forward<F>(callback), std::forward<Args>(args)...);
    }
  }

  // Process frame byte and write its encoded bits via the given callback.
  //
  // The encoded bits are passed to the callback, one per the callback
  // invocation. The given list of args... is passed to the callback after the
  // bit. This makes the required callback signature to be:
  //
  //   callback(bool bit, <optional arguments>)
  template <class F, class... Args>
  inline void operator()(const FrameByte& frame_byte,
                         F&& callback,
                         Args&&... args) {
    if (frame_byte.IsMarker()) {
      return (*this)(frame_byte.GetMarker(),
                     std::forward<F>(callback),
                     std::forward<Args>(args)...);
    }

    if (frame_byte.IsData()) {
      return (*this)(frame_byte.GetData(),
                     std::forward<F>(callback),
                     std::forward<Args>(args)...);
    }
  }

 private:
  template <class F, class... Args>
  inline void WriteMarker(F&& callback, Args&&... args) {
    const auto byte_value = std::to_integer<uint8_t>(Spec::kFrameMarker);
    for (int i = 0; i < 8; ++i) {
      const bool bit = byte_value & (1 << i);
      std::invoke(std::forward<F>(callback), bit, std::forward<Args>(args)...);
    }
  }

  // Push data byte, taking care of bit stuffing.
  template <class F, class... Args>
  inline void PushDataBit(const bool bit, F&& callback, Args&&... args) {
    std::invoke(std::forward<F>(callback), bit, std::forward<Args>(args)...);

    if (bit) {
      ++num_sequential_ones_;
    } else {
      num_sequential_ones_ = 0;
    }

    if (num_sequential_ones_ == Spec::kMaxConsecutiveOnes) {
      std::invoke(
          std::forward<F>(callback), false, std::forward<Args>(args)...);
      num_sequential_ones_ = 0;
    }
  }

  // Number of binary ones pushed in a row (counted on the bit level).
  //
  // Bit stuffing: once this counter reaches `Spec::kMaxConsecutiveOnes` a
  // forced 0 is transmitted.
  int num_sequential_ones_ = 0;
};

}  // namespace radio_core::protocol::datalink::hdlc
