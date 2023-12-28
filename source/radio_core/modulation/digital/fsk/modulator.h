// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Demodulator of frequency-shift keying (FSK).
//
// Such modulation represents binary data with changes in frequency.
//
// This modulator converts binary data to changes in frequency denoted by tones.
// The input of the modulator is a stream of binary bits, the output is tones.

#pragma once

#include <functional>

#include "radio_core/base/frequency_duration.h"
#include "radio_core/modulation/digital/fsk/tones.h"

namespace radio_core::modulation::digital::fsk {

template <class RealType>
class Modulator {
 public:
  struct Options {
    // Tones of modulated mark and space symbols.
    Tones tones{0, 0};

    // Baud rate: symbols per second in the data stream.
    int data_baud{0};
  };

  Modulator() = default;

  constexpr explicit Modulator(const Options& options) { Configure(options); }

  constexpr void Configure(const Options& options) {
    tones_ = options.tones;
    bit_duration_ms_ = RealType(1000) / options.data_baud;
  }

  // Process bit of an input data.
  //
  // Returns frequency and duration which defines signal representing the bit.
  inline auto operator()(const bool sample) -> FrequencyDuration<RealType> {
    if (sample) {
      return {tones_.mark, bit_duration_ms_};
    }
    return {tones_.space, bit_duration_ms_};
  }

  // Process bit of an input data.
  //
  // The modulated frequency and duration is passed to the callback as an object
  // of FrequencyDuration<RealType>. The given list of args... is passed to the
  // callback after the frequency duration sample. This makes the required
  // callback signature to be:
  //
  //   callback(const FrequencyDuration<RealType>& sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const bool sample, F&& callback, Args&&... args) {
    const FrequencyDuration<RealType> frequency_duration = (*this)(sample);

    std::invoke(std::forward<F>(callback),
                frequency_duration,
                std::forward<Args>(args)...);
  }

 private:
  Tones tones_;
  RealType bit_duration_ms_{0};
};

}  // namespace radio_core::modulation::digital::fsk
