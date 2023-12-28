// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Find maximum element in the given span.
// Passing empty span has an underfined behavior.

#pragma once

#include <cassert>
#include <cfloat>
#include <span>

#include "radio_core/base/algorithm.h"
#include "radio_core/base/half.h"
#include "radio_core/math/kernel/internal/horizontal_max_vectorized.h"

#include "radio_core/math/kernel/internal/horizontal_max_neon.h"

namespace radio_core {

template <class T>
inline auto HorizontalMax(const std::span<const T> samples) -> T {
  assert(!samples.empty());

  const size_t n = samples.size();

  T max_value = samples[0];

  size_t index = 1;
  while (index < n) {
    max_value = Max(max_value, samples[index]);
    ++index;
  }

  return max_value;
}

// Specialization for the single precision floating point samples.
template <>
inline auto HorizontalMax(const std::span<const float> samples) -> float {
  return horizontal_max_kernel_internal::Kernel<float, true>::Execute(samples);
}

#if RADIO_CORE_HAVE_HALF

// Specialization for the half precision floating point samples.
template <>
inline auto HorizontalMax(const std::span<const Half> samples) -> Half {
  return horizontal_max_kernel_internal::Kernel<Half, true>::Execute(samples);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
