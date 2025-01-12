// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

// A raised-cosine filter (RC) filter.
//
// https://en.wikipedia.org/wiki/Raised-cosine_filter
//
// The T parameter from the canonical equations is called samples_per_symbol.

#pragma once

#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/base/verify.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/filter_gain.h"

namespace radio_core::signal {

// Calculate impulse response h(t) of the RC filter.
template <class T>
inline auto RCWeight(const T t, const T samples_per_symbol, const T beta) -> T {
  const T pi = T(constants::pi);

  const T sps = samples_per_symbol;

  const T sps_2beta = sps / (T(2) * beta);
  if (t == sps_2beta || t == -sps_2beta) {
    return pi / (T(4) * sps) * Sinc(T(1) / (T(2) * beta));
  }

  const T betaT_T = beta * t / sps;

  return (T(1) / sps) * Sinc(t / sps) *
         (Cos(pi * betaT_T) / (T(1) - 4 * betaT_T * betaT_T));
}

// Design raised-cosine (RC) filter which acts as a low-pass filter.
// The designed filter has unity gain at the DC.
//
// It is similar to Matlab's rcosdesign():
//
//   rcosdesign(
//       beta, h.size() / samples_per_symbol, samples_per_symbol, "normal")
//
// The difference is that Matlab's function provides filter which has unity
// energy, while this designed provides filter with unity gain.
//
// Conversion of filter designed by Matlab to match this function:
//   h = rcosdesign(...)
//   ScaleFilterToUnityGainAtFrequency(h, 0)
//
// Conversion of filter designed by this function to match the Matlab:
//   DesignLowpassRCFilter(h, ...)
//   ScaleFilterToUnityEnergy(h)
template <class T>
inline void DesignLowpassRCFilter(std::span<T> h,
                                  const T samples_per_symbol,
                                  const T beta) {
  const int num_taps = int(h.size());

  Verify((num_taps & 1) == 1, "Required odd number of taps");

  const size_t order = num_taps - 1;
  const T half_order = T(order) / 2;

  for (size_t n = 0; n <= order; ++n) {
    const T n_center = T(n) - half_order;
    h[n] = RCWeight(n_center, samples_per_symbol, beta);
  }

  ScaleFilterToUnityGainAtFrequency<T>(h, 0);
}

}  // namespace radio_core::signal
