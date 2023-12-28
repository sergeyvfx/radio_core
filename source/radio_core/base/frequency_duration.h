// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// A simple definition of a signal which is denoted bu its constant frequency
// and duration in time.
//
// The frequency is measured in Hertz.
//
// The duration is measured in milliseconds. The choice of such unit of time is
// based on an idea that the durations are typically low and it makes sense to
// give more precision on the typically used values.

#pragma once

#include <ostream>

#include "radio_core/base/frequency.h"

namespace radio_core {

template <class Time>
class FrequencyDuration {
 public:
  FrequencyDuration() = default;

  constexpr FrequencyDuration(const Frequency new_frequency,
                              const Time new_duration_ms)
      : frequency(new_frequency), duration_ms(new_duration_ms) {}

  constexpr auto operator==(const FrequencyDuration& other) const -> bool {
    return frequency == other.frequency && duration_ms == other.duration_ms;
  }
  constexpr auto operator!=(const FrequencyDuration& other) const -> bool {
    return !(*this == other);
  }

  friend auto operator<<(std::ostream& os, const FrequencyDuration& tone)
      -> std::ostream& {
    os << "(" << tone.frequency << ", " << tone.duration_ms << ")";
    return os;
  }

  // Frequency in Hertz.
  Frequency frequency{0};

  // Duration of the frequency in milliseconds.
  Time duration_ms{0};
};

}  // namespace radio_core
