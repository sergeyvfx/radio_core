// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cassert>

namespace radio_core {

// Convert time measured in milliseconds to number of samples with given
// samples per second rate. Uses rounding behavior: if fractional part of the
// number of samples is lower than 0.5 then the number of samples is rounded
// down, otherwise up.
template <class T>
constexpr auto MillisecondsToNumSamples(const T sample_rate, const T time_ms)
    -> T {
  assert(sample_rate >= 0);
  assert(time_ms >= 0);
  // Use +0.5f instead of lround for the performance reasons. It is known that
  // the values are not negative, so simplified code is fine.
  return sample_rate * (time_ms / 1000) + T(0.5);
}

// Convert time measured in number of samples to milliseconds.
template <class T>
constexpr auto NumSamplesToMilliseconds(const T sample_rate,
                                        const T num_samples) -> T {
  assert(sample_rate >= 0);
  assert(num_samples >= 0);
  return num_samples * 1000 / sample_rate;
}

}  // namespace radio_core
