// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the Norm() kernel which uses the available vectorized
// types on the current platform. It does not perform any more specific
// optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/base_complex.h"
#include "radio_core/math/kernel/internal/kernel_common.h"
#include "radio_core/math/math.h"

namespace radio_core::norm_kernel_internal {

template <class RealType, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(
      const std::span<const BaseComplex<RealType>>& samples,
      const std::span<RealType>& absolute_values) -> std::span<RealType> {
    using kernel_internal::VectorizedBase;

    using Sample = BaseComplex<RealType>;
    using Sample4 = typename VectorizedBase<Sample>::template VectorizedType<4>;
    using Sample8 = typename VectorizedBase<Sample>::template VectorizedType<8>;

    using Result = RealType;
    using Result4 =
        typename VectorizedBase<RealType>::template VectorizedType<4>;
    using Result8 =
        typename VectorizedBase<RealType>::template VectorizedType<8>;

    assert(samples.size() <= absolute_values.size());

    const size_t num_samples = samples.size();

    const Sample* __restrict samples_ptr = samples.data();
    Result* __restrict absolute_values_ptr = absolute_values.data();

    const Sample* samples_begin = samples_ptr;
    const Sample* samples_end = samples_ptr + num_samples;

    // Handle 8 elements at a time.
    if constexpr (Sample8::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const Sample* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const Sample8 samples8(samples_ptr);
        const Result8 abs8 = Norm(samples8);

        abs8.Store(absolute_values_ptr);

        samples_ptr += 8;
        absolute_values_ptr += 8;
      }
    }

    // Handle 4 elements at a time.
    if constexpr (Sample4::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const Sample* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const Sample4 samples4(samples_ptr);
        const Result4 abs4 = Norm(samples4);

        abs4.Store(absolute_values_ptr);

        samples_ptr += 4;
        absolute_values_ptr += 4;
      }
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      *absolute_values_ptr = Norm(*samples_ptr);

      ++samples_ptr;
      ++absolute_values_ptr;
    }

    return absolute_values.subspan(0, num_samples);
  }
};

}  // namespace radio_core::norm_kernel_internal
