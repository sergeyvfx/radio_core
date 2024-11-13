// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Processor which encodes VIS of SSTV transmission.
// This includes leader tones and the digital code.
//
// The input is a VIS code, and generated samples are frequencies of specific
// length stored as FrequencyDuration.
//
// See the timing diagram in the `vis.h` header.

#pragma once

#include <cstdint>
#include <functional>

#include "radio_core/picture/sstv/vis.h"

namespace radio_core::picture::sstv {

template <class RealType>
class VISEncoder {
 public:
  // Encode the VIS with the given digital code.
  //
  // NOTE: The MSB of the digital code it ignored and replaced with parity, as
  // per SSTV protocol specification.
  //
  // Encoding happens into a sequence of frequencies of specific duration.
  //
  // The frequency and duration is passed to the callback as an object of
  // FrequencyDuration<RealType>. The given list of args... is passed to the
  // callback after the frequency duration sample. This makes the required
  // callback signature to be:
  //
  //   callback(const FrequencyDuration<RealType>& sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const uint8_t digital_code, F&& callback, Args&&... args) {
    // Leader tones.
    for (const FrequencyDuration<RealType>& tone :
         VISTones<RealType>::kLeaderSequence) {
      std::invoke(std::forward<F>(callback), tone, std::forward<Args>(args)...);
    }

    // Start bit.
    std::invoke(std::forward<F>(callback),
                VISTones<RealType>::kStartBit,
                std::forward<Args>(args)...);

    // 7 bits of the digital code.
    int num_ones = 0;
    for (int i = 0; i < 7; ++i) {
      if (digital_code & (1 << i)) {
        ++num_ones;
        std::invoke(std::forward<F>(callback),
                    VISTones<RealType>::kBitValueOne,
                    std::forward<Args>(args)...);
      } else {
        std::invoke(std::forward<F>(callback),
                    VISTones<RealType>::kBitValueZero,
                    std::forward<Args>(args)...);
      }
    }

    // Parity.
    //
    // SSTV uses even parity. This means that the number of logical ones must
    // be even in the whole transmitted 8bit code. If the number of
    // transmitted ones in the user usable part of the digital code is odd
    // transmit parity bit of one. Otherwise transmit parity bit of zero.
    if (num_ones & 1) {
      std::invoke(std::forward<F>(callback),
                  VISTones<RealType>::kBitValueOne,
                  std::forward<Args>(args)...);
    } else {
      std::invoke(std::forward<F>(callback),
                  VISTones<RealType>::kBitValueZero,
                  std::forward<Args>(args)...);
    }

    // Stop bit.
    std::invoke(std::forward<F>(callback),
                VISTones<RealType>::kStopBit,
                std::forward<Args>(args)...);
  }
};

}  // namespace radio_core::picture::sstv
