// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Tones used to represent one (the "mark") and zero (the "space") in frequency
// shift keying (FSK) modulation.

#pragma once

#include "radio_core/base/frequency.h"

namespace radio_core::modulation::digital::fsk {

class Tones {
 public:
  Tones() = default;

  constexpr Tones(const Frequency& new_mark, const Frequency& new_space)
      : mark(new_mark), space(new_space) {}

  // Comparison of two tones.
  constexpr auto operator<=>(const Tones& rhs) const = default;

  // Tone typically representing mark and space (typically binary 1 and binary 0
  // respectively).
  Frequency mark;
  Frequency space;
};

}  // namespace radio_core::modulation::digital::fsk
