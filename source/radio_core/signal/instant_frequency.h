// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// This processor receives signal phase and calculates its instantaneous
// frequency in hertz.

#pragma once

#include <cassert>

#include "radio_core/base/constants.h"
#include "radio_core/signal/frequency.h"

namespace radio_core::signal {

template <class RealType>
class InstantFrequency {
 public:
  InstantFrequency() = default;
  explicit InstantFrequency(const RealType sample_rate)
      : sample_rate_(sample_rate) {}

  inline void SetSampleRate(const RealType sample_rate) {
    sample_rate_ = sample_rate;
  }
  inline auto GetSampleRate() const -> RealType { return sample_rate_; }

  inline auto operator()(const RealType phase) -> RealType {
    assert(sample_rate_ != 0);

    constexpr RealType kPi = constants::pi_v<RealType>;

    // Differentiate the phase into instant frequency in radians per sample.
    const RealType instant_frequency =
        WrapInstantFrequency(phase - prev_diff_phase_);

    prev_diff_phase_ = phase;

    // Return frequency in Hz.
    return instant_frequency * sample_rate_ / (2 * kPi);
  }

 private:
  RealType sample_rate_{0};
  RealType prev_diff_phase_{0};
};

}  // namespace radio_core::signal
