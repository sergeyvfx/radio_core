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

// Calculate rational frequency from normalized angular frequency.
// A reverse of NormalizedAngularFrequency().
template <class T>
inline constexpr auto RationalFrequency(const T angular_frequency,
                                        const T sample_rate) -> T {
  return (angular_frequency * sample_rate) / (2 * T(constants::pi));
}

// Wrap instant normalized instant frequency to the range (pi, pi] by adding or
// subtracting 2*pi
// Note that the wrapping only happens once..
//
// It is possible to apply this wrapping on any value that needs to be within
// this range and follow the same rules. The name is kept less generic to make
// it somewhat a policy about the range.
template <class T>
inline auto WrapInstantFrequency(const T instant_frequency) -> T {
  if (instant_frequency > T(constants::pi)) {
    return instant_frequency - T(2) * T(constants::pi);
  }

  if (instant_frequency <= -T(constants::pi)) {
    return instant_frequency + T(2) * T(constants::pi);
  }

  return instant_frequency;
}

}  // namespace radio_core::signal
