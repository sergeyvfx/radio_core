// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Sum all elements of the input vector.
// Passing empty span has an underfined behavior.

#pragma once

#include <cassert>
#include <cfloat>
#include <span>

#include "radio_core/base/algorithm.h"
#include "radio_core/base/half.h"
#include "radio_core/math/kernel/internal/horizontal_sum_vectorized.h"

#include "radio_core/math/kernel/internal/horizontal_sum_neon.h"

namespace radio_core::kernel {

template <class T>
inline auto HorizontalSum(const std::span<const T> samples) -> T {
  assert(!samples.empty());

  const size_t n = samples.size();

  T sum = samples[0];

  size_t index = 1;
  while (index < n) {
    sum += samples[index];
    ++index;
  }

  return sum;
}

// Specialization for the single precision floating point samples.
template <>
inline auto HorizontalSum(const std::span<const float> samples) -> float {
  return horizontal_sum_internal::Kernel<float, true>::Execute(samples);
}

#if RADIO_CORE_HAVE_HALF

// Specialization for the half precision floating point samples.
template <>
inline auto HorizontalSum(const std::span<const Half> samples) -> Half {
  return horizontal_sum_internal::Kernel<Half, true>::Execute(samples);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel
