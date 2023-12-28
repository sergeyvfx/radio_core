// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the FastArg() kernel which uses the available vectorized
// types on the current platform. It does not perform any more specific
// optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/kernel/internal/kernel_common.h"
#include "radio_core/math/math.h"

namespace radio_core::fast_arg_kernel_internal {

template <class Real, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(const std::span<const BaseComplex<Real>>& samples,
                             const std::span<Real>& arg) -> std::span<Real> {
    using kernel_internal::VectorizedBase;

    using RealComplex = BaseComplex<Real>;
    using RealComplex4 =
        typename VectorizedBase<RealComplex>::template VectorizedType<4>;
    using RealComplex8 =
        typename VectorizedBase<RealComplex>::template VectorizedType<8>;

    using Real4 = typename VectorizedBase<Real>::template VectorizedType<4>;
    using Real8 = typename VectorizedBase<Real>::template VectorizedType<8>;

    assert(samples.size() <= arg.size());

    const size_t num_samples = samples.size();

    const RealComplex* __restrict samples_ptr = samples.data();
    Real* __restrict arg_ptr = arg.data();

    const RealComplex* samples_begin = samples_ptr;
    const RealComplex* samples_end = samples_ptr + num_samples;

    // Handle 8 elements at a time.
    if constexpr (RealComplex8::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const RealComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const RealComplex8 samples8(samples_ptr);
        const Real8 abs8 = FastArg(samples8);

        abs8.Store(arg_ptr);

        samples_ptr += 8;
        arg_ptr += 8;
      }
    }

    // Handle 4 elements at a time.
    if constexpr (RealComplex4::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const RealComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const RealComplex4 samples4(samples_ptr);
        const Real4 abs4 = FastArg(samples4);

        abs4.Store(arg_ptr);

        samples_ptr += 4;
        arg_ptr += 4;
      }
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      *arg_ptr = FastArg(*samples_ptr);

      ++samples_ptr;
      ++arg_ptr;
    }

    return arg.subspan(0, num_samples);
  }
};

}  // namespace radio_core::fast_arg_kernel_internal
