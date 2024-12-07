// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/base/constants.h"

namespace radio_core::signal {

// Calculate normalized angular frequency of a frequency in Hertz and sample
// rate. The output unit is radians per sample.
//
// References:
//   https://wikipedia.org/wiki/Normalized_frequency_(unit)
//   https://wikipedia.org/wiki/Angular_frequency
template <class T>
inline constexpr auto NormalizedAngularFrequency(const T frequency,
                                                 const T sample_rate) -> T {
  return 2 * T(constants::pi) * frequency / sample_rate;
}

// Unwrap the frequency, so that it stays positive when the phase crosses 0
// radians.
// Note that the wrapping only happens once.
template <class T>
inline auto UnwrapInstantFrequency(const T instant_frequency) -> T {
  if (instant_frequency > T(constants::pi)) {
    return instant_frequency - T(2) * T(constants::pi);
  }

  if (instant_frequency < -T(constants::pi)) {
    return instant_frequency + T(2) * T(constants::pi);
  }

  return instant_frequency;
}

}  // namespace radio_core::signal
