// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Calculate per-element absolute value of input signal.

#pragma once

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/internal/abs_vectorized.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

#include "radio_core/math/kernel/internal/abs_neon.h"

namespace radio_core::kernel {

// The output buffer must have at least same number of elements as the input
// samples buffer. It is possible to have the output buffer bigger than input
// in which case the output buffer will only be partially written (only
// number of input samples will be written to the output).
//
// Returns subspan of the output buffer where values has actually been
// written.
template <class T, class ResultType = decltype(Abs(std::declval<T>()))>
inline auto Abs(const std::span<const T>& samples,
                const std::span<ResultType>& absolute_values)
    -> std::span<ResultType> {
  assert(samples.size() <= absolute_values.size());

  const size_t num_samples = samples.size();
  for (size_t i = 0; i < num_samples; ++i) {
    absolute_values[i] = radio_core::Abs(samples[i]);
  }

  return absolute_values.subspan(0, num_samples);
}

// Optimized specialization for Complex input sample type.
template <>
inline auto Abs(const std::span<const Complex>& samples,
                const std::span<float>& absolute_values) -> std::span<float> {
  return abs_internal::Kernel<Complex, true>::Execute(samples, absolute_values);
}

#if RADIO_CORE_HAVE_HALF

// Optimized specialization for HalfComplex input sample type.
template <>
inline auto Abs(const std::span<const HalfComplex>& samples,
                const std::span<Half>& absolute_values) -> std::span<Half> {
  return abs_internal::Kernel<HalfComplex, true>::Execute(samples,
                                                          absolute_values);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel
