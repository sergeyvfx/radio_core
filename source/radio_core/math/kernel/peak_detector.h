// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Peak detector using different strategies.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/base/half.h"
#include "radio_core/math/kernel/internal/peak_detector_vectorized.h"
#include "radio_core/math/math.h"

namespace radio_core {

// Detect peaks using linear interpolation between current state of the peak
// detector (stored in the `peak`) and the new samples. The peak is detected
// on the per-point basis.
//
// The weight of interpolation is different for cases when new value is higher
// and lower than the detected peak. This allows to cover situations when a new
// peak is to be detected quickly, but be slowly decaying to make it easier to
// see in the interface.
//
// The output buffer must have at least same number of elements as the input
// samples buffer. It is possible to have the output buffer bigger than input
// in which case the output buffer will only be partially written (only
// number of input samples will be written to the output).
//
// Returns subspan of the output buffer where values has actually been
// written.
template <class T>
inline auto PerPointLerpPeakDetector(const std::span<const T> samples,
                                     const std::span<T> peak,
                                     const T charge_rate,
                                     const T discharge_rate) -> std::span<T> {
  assert(samples.size() == peak.size());

  const size_t num_samples = samples.size();

  const T* __restrict samples_ptr = samples.data();
  T* __restrict peak_ptr = peak.data();

  const T* samples_end = samples_ptr + num_samples;

  while (samples_ptr < samples_end) {
    if (*samples_ptr > *peak_ptr) {
      *peak_ptr = Lerp(*peak_ptr, *samples_ptr, charge_rate);
    } else {
      *peak_ptr = Lerp(*peak_ptr, *samples_ptr, discharge_rate);
    }

    ++samples_ptr;
    ++peak_ptr;
  }

  return peak.subspan(0, num_samples);
}

// Vectorized and optimized version of PerPointLerpPeakDetector<float>.
template <>
inline auto PerPointLerpPeakDetector(const std::span<const float> samples,
                                     const std::span<float> peak,
                                     const float charge_rate,
                                     const float discharge_rate)
    -> std::span<float> {
  return peak_detector_kernel_internal::Kernel<float, true>::Execute(
      samples, peak, charge_rate, discharge_rate);
}

#if RADIO_CORE_HAVE_HALF

// Vectorized and optimized version of PerPointLerpPeakDetector<Half>.
template <>
inline auto PerPointLerpPeakDetector(const std::span<const Half> samples,
                                     const std::span<Half> peak,
                                     const Half charge_rate,
                                     const Half discharge_rate)
    -> std::span<Half> {
  return peak_detector_kernel_internal::Kernel<Half, true>::Execute(
      samples, peak, charge_rate, discharge_rate);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
