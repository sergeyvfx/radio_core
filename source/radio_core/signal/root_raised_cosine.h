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

namespace radio_core::signal {

namespace rrc_internal {

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

  if (beta > T(0)) {
    const T sps_4beta = sps / (T(4) * beta);
    if (t == sps_4beta || t == -sps_4beta) {
      const T pi_4beta = pi / (4 * beta);

      const T s = (T(1) + T(2) / pi) * Sin(pi_4beta);
      const T c = (T(1) - T(2) / pi) * Cos(pi_4beta);

      return beta / (sps * Sqrt(T(2))) * (s + c);
    }
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

}  // namespace rrc_internal

// Design root-raised-cosine (RRC) filter which acts as a low-pass filter.
// The designed filter has requested gain at the DC.
//
// It matches the root_raised_cosine() filter design from the GNU Radio:
//   >>> firdes.root_raised_cosine(
//   ...    gain=gain,
//   ...    sampling_freq=samples_per_symbol,
//   ...    symbol_rate=1,
//   ...    alpha=beta,
//   ...    ntaps=h.size())
//
// It is similar to Matlab's rcosdesign():
//
//   rcosdesign(beta, h.size() / samples_per_symbol, samples_per_symbol)
//   rcosdesign(beta, h.size() / samples_per_symbol, samples_per_symbol, "sqrt")
//
// The difference is that Matlab's function provides filter which has unity
// energy, while this designed provides filter with the desired gain.
//
// Conversion of filter designed by Matlab to match this function:
//   h = rcosdesign(...)
//   ScaleFilterToGainAtDC(h, gain)
//
// Conversion of filter designed by this function to match the Matlab:
//   DesignRRCFilter(h, 1, ...)
//   ScaleFilterToUnityEnergy(h)
template <class T>
inline void DesignRRCFilter(std::span<T> h,
                            const T gain,
                            const T samples_per_symbol,
                            const T beta) {
  const int num_taps = int(h.size());

  Verify((num_taps & 1) == 1, "Required odd number of taps");

  const size_t order = num_taps - 1;
  const T half_order = T(order) / 2;

  T scale{0};
  for (size_t n = 0; n <= order; ++n) {
    const T n_center = T(n) - half_order;
    h[n] = rrc_internal::RRCWeight(n_center, samples_per_symbol, beta);
    scale += h[n];
  }

  const T multiplier = gain / scale;
  for (T& h_k : h) {
    h_k *= multiplier;
  }
}

}  // namespace radio_core::signal
