// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// FIlter gain calculation and normalization.

#pragma once

#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/math/math.h"

namespace radio_core::signal {

// Calculate gain of the given filter at the DC.
template <class T>
inline auto CalculateFilterGainAtDC(const std::span<const T> h) -> T {
  T gain(0);
  for (const T h_k : h) {
    gain += h_k;
  }
  return gain;
}

// Calculate gain of the given filter at the given frequency.
template <class T>
inline auto CalculateFilterGain(const std::span<const T> h, const T frequency)
    -> T {
  if (frequency == 0) {
    // Early output for gain calculation at the DC: can use cheaper calculation.
    return CalculateFilterGainAtDC(h);
  }

  // Calculation of a FIR filter based on the discussion on StackExchange (using
  // formula from the original post:
  //
  //   https://dsp.stackexchange.com/questions/4693/fir-filter-gain
  //
  // This is also how SciPy calculates the gain:
  //
  //   https://github.com/scipy/scipy/blob/v1.7.1/scipy/signal/fir_filter_design.py#L476

  const int num_taps = int(h.size());
  const int order = num_taps - 1;
  const T half_window = T(order) / 2;
  const T angular_freq = 2 * T(constants::pi) * frequency;

  T gain(0);
  for (int n = 0; n <= order; ++n) {
    gain += h[n] * Cos(angular_freq * (n - half_window));
  }

  return gain;
}

// Scale filter to have an unity gain at the given frequency.
template <class T>
inline void ScaleFilterToUnityGainAtFrequency(const std::span<T> h,
                                              const T frequency) {
  const T gain = CalculateFilterGain<T>(h, frequency);
  const T gain_inv = T(1) / gain;
  for (T& h_k : h) {
    h_k *= gain_inv;
  }
}

}  // namespace radio_core::signal
