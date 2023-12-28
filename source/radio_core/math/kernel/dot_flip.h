// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Calculate dot-product of two signals, with an implicit flip of one of them.
//
// The signals are fined as a span of their samples. The exact data type of the
// signals is not important: what matters is that there is a defined
// multiplication and addition operators for the types.
//
// NOTE: A bit of a niche use-case, so it is marked as an experimental API.

#pragma once

#include <cassert>
#include <span>
#include <utility>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/internal/dot_flip_vectorized.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

#include "radio_core/math/kernel/internal/dot_flip_neon.h"

namespace radio_core::experimental {

// Equivalent of `numpy.dot(f, numpy.flip(g))`.
template <class FType,
          class GType,
          class OutputType = decltype(std::declval<FType>() *
                                      std::declval<GType>())>
inline auto DotFlipG(const std::span<const FType>& f,
                     const std::span<const GType>& g) -> OutputType {
  assert(f.size() == g.size());
  const size_t num_samples = f.size();

  OutputType output(0);

  for (size_t i = 0, j = num_samples - 1; i < num_samples; ++i, --j) {
    output += f[i] * g[j];
  }

  return output;
}

// Specialization for dot product between float and float arguments.
template <>
inline auto DotFlipG(const std::span<const float>& f,
                     const std::span<const float>& g) -> float {
  return dot_flip_kernel_internal::Kernel<float, float, true>::Execute(f, g);
}

// Specialization for dot product between Complex and float arguments.
template <>
inline auto DotFlipG(const std::span<const Complex>& f,
                     const std::span<const float>& g) -> Complex {
  return dot_flip_kernel_internal::Kernel<Complex, float, true>::Execute(f, g);
}

#if RADIO_CORE_HAVE_HALF

// Specialization for dot product between Half and Half arguments.
template <>
inline auto DotFlipG(const std::span<const Half>& f,
                     const std::span<const Half>& g) -> Half {
  return dot_flip_kernel_internal::Kernel<Half, Half, true>::Execute(f, g);
}

// Specialization for dot product between HalfComplex and Half arguments.
template <>
inline auto DotFlipG(const std::span<const HalfComplex>& f,
                     const std::span<const Half>& g) -> HalfComplex {
  return dot_flip_kernel_internal::Kernel<HalfComplex, Half, true>::Execute(f,
                                                                            g);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::experimental
