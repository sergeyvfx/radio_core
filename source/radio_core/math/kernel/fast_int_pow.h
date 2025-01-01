// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

// Calculates the power of the input samples to the given integer power.
// The accuracy is similar to naive multiplication for the positive power.
//
// For thr zero or negative exponent the exact rule is not specified. This is
// why its considered "fast" as it is a deviation from the more standard Pow()
// which allows to optimize calculation.

#pragma once

#include <span>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/internal/fast_int_pow_vectorized.h"
#include "radio_core/math/math.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::kernel {

// The output buffer must have at least same number of elements as the input
// samples buffer. It is possible to have the output buffer bigger than input
// in which case the output buffer will only be partially written (only
// number of input samples will be written to the output).
//
// Returns subspan of the output buffer where values has actually been
// written.
template <class T>
inline auto FastIntPow(const std::span<const T>& base,
                       const int exp,
                       const std::span<T>& pow) -> std::span<T> {
  assert(base.size() <= pow.size());

  const size_t size = base.size();

  const T* __restrict base_ptr = base.data();
  T* __restrict pow_ptr = pow.data();

  const T* base_end = base_ptr + size;

  while (base_ptr < base_end) {
    *pow_ptr = FastIntPow(*base_ptr, exp);

    ++base_ptr;
    ++pow_ptr;
  }

  return pow.subspan(0, size);
}

// Specialization for single floating point precision complex values.
template <>
inline auto FastIntPow(const std::span<const Complex>& base,
                       const int exp,
                       const std::span<Complex>& pow) -> std::span<Complex> {
  return fast_int_pow_internal::Kernel<float, true>::Execute(base, exp, pow);
}

#if RADIO_CORE_HAVE_HALF
// Specialization for half floating point precision complex values.
template <>
inline auto FastIntPow(const std::span<const HalfComplex>& base,
                       const int exp,
                       const std::span<HalfComplex>& pow)
    -> std::span<HalfComplex> {
  return fast_int_pow_internal::Kernel<Half, true>::Execute(base, exp, pow);
}
#endif

}  // namespace radio_core::kernel
