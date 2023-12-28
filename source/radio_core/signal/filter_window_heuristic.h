// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/math/math.h"
#include "radio_core/signal/frequency.h"

namespace radio_core::signal {

// Estimate filter transition bandwidth from the filter size.
// The bandwidth is measured in the normalized frequency.
//
// Transition Bandwidth section of the article:
//   https://tomroelandts.com/articles/how-to-create-a-simple-low-pass-filter
template <class RealType>
inline auto EstimateNormalizedTransitionBandwidth(const size_t filter_size)
    -> RealType {
  return RealType(4) / filter_size;
}

// Estimate filter size needed to reach desired transition bandwidth.
// The bandwidth is measured in the normalized frequency.
//
// https://tomroelandts.com/articles/the-transition-bandwidth-of-a-filter-depends-on-the-window-type
template <class RealType>
inline auto EstimateFilterSizeForNormalizedTransitionBandwidth(
    const RealType transition_bandwidth) -> size_t {
  return RealType(4) / transition_bandwidth;
}
template <class RealType>
inline auto EstimateFilterSizeForTransitionBandwidth(
    const RealType transition_bandwidth, const RealType sample_rate) -> size_t {
  return size_t(RealType(4) /
                NormalizedAngularFrequency(transition_bandwidth, sample_rate));
}

// Formulas to calculate Kaiser window parameters to achieve a relative sidelobe
// attenuation of –α (alpha) dB and transition width Δω (dw) rad/sample.
//
// References:
//   Kaiser Window
//   https://mathworks.com/help/signal/ug/kaiser-window.html

// Calculate the β (beta) parameter for desired sidelobe attenuation α (a).
template <class RealType>
inline constexpr auto CalculateKaiserBeta(const RealType alpha) -> RealType {
  if (alpha > RealType(50)) {
    return RealType(0.1102) * (alpha - RealType(8.7));
  }

  if (alpha >= 21) {
    const RealType alpha_21 = alpha - 21;
    return RealType(0.5842) * Pow(alpha_21, RealType(0.4)) +
           RealType(0.07886) * alpha_21;
  }

  return 0;
}

// Calculate window length (number of taps) for a transition of Δω rad/sample
// (dw) and attenuation α (a).
// The Δω is a normalized angular frequency.
template <class RealType>
inline constexpr auto CalculateKaiserSize(const RealType alpha,
                                          const RealType dw) -> size_t {
  return (alpha - RealType(8)) / (RealType(2.285) * dw) + RealType(1);
}

}  // namespace radio_core::signal
