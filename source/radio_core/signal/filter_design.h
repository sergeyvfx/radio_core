// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Utility functions to design various filters.

#pragma once

#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/base/verify.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/filter_gain.h"

namespace radio_core::signal {

// Design filter which will pass frequencies below the given cutoff frequency
// and will reject frequencies above that.
//
// NOTE: The cutoff frequency must be in [0 .. sampling_frequency/2] range and
// it is `VERIFY()`-ed.
//
// The filter is denoted by its kernel coefficients with the number of taps
// equal to `h.size()`.
//
// The window is denoted by its equation.
//
// This call is similar to SciPy:
//
//   scipy.signal.firwin(h.size(),
//                       cutoff_frequency,
//                       window=<window>,
//                       fs=sampling_frequency,
//                       pass_zero=True)
//
// TODO(sergey): Support kernels with complex element type.
template <class T, class WindowPredicateType>
inline void DesignLowPassFilter(std::span<T> h,
                                const WindowPredicateType& window_equation,
                                const T cutoff_frequency,
                                const T sampling_frequency = 2) {
  Verify(cutoff_frequency <= sampling_frequency / 2,
         "Nyquest requirement for cutoff_frequency");

  // Calculate filter coefficients.
  const T ft = cutoff_frequency / sampling_frequency;
  const int num_taps = int(h.size());
  const int order = num_taps - 1;
  const T half_order = T(order) / 2;
  const int half_order_int = int(half_order);
  for (int n = 0; n <= order; ++n) {
    if (n == half_order_int) {
      h[n] = 2 * ft;
    } else {
      const T pi_n_half_order = T(constants::pi) * (n - half_order);
      const T pi_n_half_order2 = pi_n_half_order * 2;
      const T denum_inv = T(1) / pi_n_half_order;

      h[n] = Sin(pi_n_half_order2 * ft) * denum_inv;
    }

    h[n] *= window_equation(order, n);
  }

  // Scale the filter to have unity gain at the DC.
  ScaleFilterToUnityGainAtFrequency<T>(h, 0);
}

// Design filter which passes frequencies within the cutoff start/end range and
// rejects filters outside of the range.
//
// The filter is denoted by its FIR coefficients with the number of taps equal
// to `h.size()`.
//
// The number of taps must be odd and the cutoff frequencies must be within the
// [0 .. sampling_frequency/2] range. This is `VERIFY)`-ed.
//
// The window is denoted by its equation.
//
// This call is similar to SciPy:
//
//   scipy.signal.firwin(h.size(),
//                       [cutoff_frequency_start, cutoff_frequency_end],
//                       window=<window>,
//                       fs=sampling_frequency,
//                       pass_zero=False)
//
// TODO(sergey): Support kernels with complex element type.
template <class T, class WindowPredicateType>
inline void DesignBandPassFilter(std::span<T> h,
                                 const WindowPredicateType& window_equation,
                                 const T cutoff_frequency_start,
                                 const T cutoff_frequency_end,
                                 const T sampling_frequency = 2) {
  // Validate frequencies.
  Verify(cutoff_frequency_start <= sampling_frequency / 2,
         "Nyquest requirement for cutoff_frequency_start");
  Verify(cutoff_frequency_end <= sampling_frequency / 2,
         "Nyquest requirement for cutoff_frequency_end");

  const T ft1 = cutoff_frequency_start / sampling_frequency;
  const T ft2 = cutoff_frequency_end / sampling_frequency;

  const int num_taps = int(h.size());
  const int order = num_taps - 1;

  // Bandpass requires even order of filter (an odd filter length).
  Verify((order & 1) == 0, "Filter order is expected to be odd");

  // Calculate filter coefficients.
  const T half_order = T(order) / 2;
  const int half_order_int = half_order;
  for (int n = 0; n <= order; ++n) {
    if (n == half_order_int) {
      h[n] = T(2) * (ft2 - ft1);
    } else {
      const T pi_n_half_order = constants::pi_v<T> * (n - half_order);
      const T pi_n_half_order2 = pi_n_half_order * T(2);
      const T denum_inv = T(1) / pi_n_half_order;

      h[n] = Sin(pi_n_half_order2 * ft2) * denum_inv -
             Sin(pi_n_half_order2 * ft1) * denum_inv;
    }

    h[n] *= window_equation(order, n);
  }

  // Scale the filter to have unity gain at the center frequency.
  const T f_center = (ft1 + ft2) * T(0.5);
  ScaleFilterToUnityGainAtFrequency(h, f_center);
}

// Design filter which delays signal by a fractional number of samples.
// For example, allows t odelay input signal by 0.3 samples.
//
// References:
//
//   https://www.dsprelated.com/showarticle/1327.php formula (3)
//   https://tomroelandts.com/articles/how-to-create-a-fractional-delay-filter
//
// TODO(sergey): Support kernels with complex element type.
template <class T, class WindowPredicateType>
inline void DesignFractionalDelayFilter(
    std::span<T> h,
    const WindowPredicateType& window_equation,
    const T num_fractional_samples) {
  const int num_taps = int(h.size());
  const int order = num_taps - 1;
  const T half_order = T(order) / 2;

  for (int n = 0; n <= order; ++n) {
    const T n_center = T(n) - half_order;
    h[n] = Sinc(n_center - num_fractional_samples);
    h[n] *= window_equation(order, n);
  }

  // Scale the filter to have unity gain at the DC.
  ScaleFilterToUnityGainAtFrequency<T>(h, 0);
}

}  // namespace radio_core::signal
