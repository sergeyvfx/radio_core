// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/base/verify.h"
#include "radio_core/math/math.h"

namespace radio_core::signal {

// An equation of Hilbert transformer's FIR weights.
//
//   Understanding Digital Signal Processing
//   Richard G. Lyons
//
//   Impulse Response of a Hilbert Transformer chapter, formula 9-12.
template <class RealType>
inline auto HilbertTransformerWeight(const int n) -> RealType {
  if (n == 0) {
    return RealType(0);
  }

  const RealType sine_term = Sin(constants::pi_v<RealType> * n / 2);
  const RealType sine_term2 = sine_term * sine_term;

  return 2 / (constants::pi_v<RealType> * n) * sine_term2;
}

// Generate weights of a windowed Hilbert transformer.
//
// The number of taps equals to the size of the span.
//
// NOTE: The number of taps must be odd for valid transformer behavior and it
// is Verify()-ed.
template <class RealType, class WindowPredicateType>
inline void GenerateWindowedHilbertTransformer(
    std::span<RealType> h, const WindowPredicateType& window_equation) {
  const int num_taps = int(h.size());

  Verify((num_taps & 1) == 1, "Hilbert requires odd number of taps");

  const int order = num_taps - 1;
  const RealType half_order = RealType(order) / 2.0f;

  for (int n = 0; n <= order; ++n) {
    const int n_center = int(RealType(n) - half_order);
    h[n] = HilbertTransformerWeight<RealType>(n_center);
    h[n] *= window_equation(order, n);
  }
}

}  // namespace radio_core::signal
