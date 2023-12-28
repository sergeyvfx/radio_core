// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// A root-raised-cosine filter (RRC), sometimes known as
// square-root-raised-cosine filter (SRRC) filter.
//
// https://en.wikipedia.org/wiki/Root-raised-cosine_filter
//
// The Ts parameter from the canonical equations is called samples_per_symbol.

#pragma once

#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/base/verify.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/filter_gain.h"

namespace radio_core::signal {

// Calculate impulse response h(t) of the RRC filter.
template <class T>
inline auto RRCWeight(const T t, const T samples_per_symbol, const T beta)
    -> T {
  const T pi = T(constants::pi);

  const T sps = samples_per_symbol;
  const T sps_inv = T(1) / samples_per_symbol;

  if (t == T(0)) {
    return sps_inv * (T(1) + beta * (T(4) / pi - T(1)));
  }

  const T sps_4beta = sps / (T(4) * beta);
  if (t == sps_4beta || t == -sps_4beta) {
    const T pi_4beta = pi / (4 * beta);

    const T s = (T(1) + T(2) / pi) * Sin(pi_4beta);
    const T c = (T(1) - T(2) / pi) * Cos(pi_4beta);

    return beta / (sps * Sqrt(T(2))) * (s + c);
  }

  const T t_sps = t * sps_inv;
  const T pi_t_sps = pi * t_sps;
  const T t_sps_4_beta = 4 * beta * t_sps;

  const T s = Sin(pi_t_sps * (T(1) - beta));
  const T c = Cos(pi_t_sps * (T(1) + beta));

  const T num = s + t_sps_4_beta * c;
  const T den = pi_t_sps * (T(1) - t_sps_4_beta * t_sps_4_beta);

  return sps_inv * num / den;
}

// Design RRC filter which acts as a low-pass filter.
template <class T>
inline void DesignLowpassRRCFilter(std::span<T> h,
                                   const T samples_per_symbol,
                                   const T beta) {
  const int num_taps = int(h.size());

  // Odd number of taps is required.
  VERIFY((num_taps & 1) == 1);

  const size_t order = num_taps - 1;
  const T half_order = T(order) / 2;

  for (size_t n = 0; n <= order; ++n) {
    const T n_center = T(n) - half_order;
    h[n] = RRCWeight(n_center, samples_per_symbol, beta);
  }

  ScaleFilterToUnityGainAtFrequency<T>(h, 0);
}

}  // namespace radio_core::signal
