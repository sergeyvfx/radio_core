// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the dot product kernel which uses the available vectorized
// types on the current platform. It does not perform any more specific
// optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/kernel/internal/kernel_common.h"

namespace radio_core::kernel::dot_internal {

template <class FType, class GType, bool SpecializationMarker>
struct Kernel {
  using OutputType = decltype(std::declval<FType>() * std::declval<GType>());

  static inline auto Execute(const std::span<const FType>& f,
                             const std::span<const GType>& g) -> OutputType {
    using kernel_internal::VectorizedBase;

    using FType4 = typename VectorizedBase<FType>::template VectorizedType<4>;
    using FType8 = typename VectorizedBase<FType>::template VectorizedType<8>;

    using GType4 = typename VectorizedBase<GType>::template VectorizedType<4>;
    using GType8 = typename VectorizedBase<GType>::template VectorizedType<8>;

    assert(f.size() == g.size());

    const size_t num_samples = f.size();

    const FType* f_ptr = f.data();
    const FType* f_begin = f_ptr;
    const FType* f_end = f_ptr + num_samples;

    const GType* g_ptr = g.data();

    OutputType output(0);

    // Handle 8 elements at a time.
    if constexpr (FType8::kIsVectorized && GType8::kIsVectorized) {
      using OutputType8 =
          typename VectorizedBase<OutputType>::template VectorizedType<8>;
      OutputType8 output8(OutputType(0));

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const FType* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const FType8 f8(f_ptr);
        const GType8 g8(g_ptr);

        f_ptr += 8;
        g_ptr += 8;

        output8 = MultiplyAdd(output8, f8, g8);
      }

      output += HorizontalSum(output8);
    }

    // Handle 4 elements at a time.
    if constexpr (FType4::kIsVectorized && GType4::kIsVectorized) {
      using OutputType4 =
          typename VectorizedBase<OutputType>::template VectorizedType<4>;
      OutputType4 output4(OutputType(0));

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const FType* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const FType4 f4(f_ptr);
        const GType4 g4(g_ptr);

        f_ptr += 4;
        g_ptr += 4;

        output4 = MultiplyAdd(output4, f4, g4);
      }

      output += HorizontalSum(output4);
    }

    // Handle the remaining tail.
    while (f_ptr < f_end) {
      output = MultiplyAdd(output, *f_ptr, *g_ptr);

      ++f_ptr;
      ++g_ptr;
    }

    return output;
  }
};

}  // namespace radio_core::kernel::dot_internal
