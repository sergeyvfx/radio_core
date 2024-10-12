// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Calculates the squared magnitude (norm) of a complex number.

#pragma once

#include <span>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/internal/norm_vectorized.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

#include "radio_core/math/kernel/internal/norm_neon.h"

namespace radio_core::kernel {

// The output buffer must have at least same number of elements as the input
// samples buffer. It is possible to have the output buffer bigger than input
// in which case the output buffer will only be partially written (only
// number of input samples will be written to the output).
//
// Returns subspan of the output buffer where values has actually been
// written.
template <class T>
inline auto Norm(const std::span<const BaseComplex<T>>& samples,
                 const std::span<T>& arg) -> std::span<T> {
  assert(samples.size() <= arg.size());

  const size_t num_samples = samples.size();

  const BaseComplex<T>* __restrict samples_ptr = samples.data();
  T* __restrict arg_ptr = arg.data();

  const BaseComplex<T>* samples_end = samples_ptr + num_samples;

  while (samples_ptr < samples_end) {
    *arg_ptr = Norm(*samples_ptr);

    ++samples_ptr;
    ++arg_ptr;
  }

  return arg.subspan(0, num_samples);
}

// Specialization for single floating point precision complex values.
template <>
inline auto Norm(const std::span<const Complex>& samples,
                 const std::span<float>& arg) -> std::span<float> {
  return norm_internal::Kernel<float, true>::Execute(samples, arg);
}

#if RADIO_CORE_HAVE_HALF
// Specialization for half floating point precision complex values.
template <>
inline auto Norm(const std::span<const HalfComplex>& samples,
                 const std::span<Half>& arg) -> std::span<Half> {
  return norm_internal::Kernel<Half, true>::Execute(samples, arg);
}
#endif

}  // namespace radio_core::kernel
