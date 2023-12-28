// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the HorizontalMax() kernel which uses the available
// vectorized types on the current platform. It does not perform any more
// specific optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/kernel/internal/kernel_common.h"

namespace radio_core::horizontal_max_kernel_internal {

template <class T, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(const std::span<const T>& samples) -> T {
    using kernel_internal::VectorizedBase;

    using Type4 = typename VectorizedBase<T>::template VectorizedType<4>;
    using Type8 = typename VectorizedBase<T>::template VectorizedType<8>;

    assert(!samples.empty());

    const size_t num_samples = samples.size();

    const T* samples_ptr = samples.data();
    const T* samples_begin = samples_ptr;
    const T* samples_end = samples_ptr + num_samples;

    T output(0);

    // Handle 8 elements at a time.
    if constexpr (Type8::kIsVectorized) {
      Type8 output8(0);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const T* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Type8 samples8(samples_ptr);

        samples_ptr += 8;

        output8 = Max(output8, samples8);
      }

      output = Max(output, HorizontalMax(output8));
    }

    // Handle 4 elements at a time.
    if constexpr (Type4::kIsVectorized) {
      Type4 output4(0);

      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const T* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Type4 samples4(samples_ptr);

        samples_ptr += 4;

        output4 = Max(output4, samples4);
      }

      output = Max(output, HorizontalMax(output4));
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      output = Max(output, *samples_ptr);

      ++samples_ptr;
    }

    return output;
  }
};

}  // namespace radio_core::horizontal_max_kernel_internal
