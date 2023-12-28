// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the PowerSpectralDensity() kernel which uses the available
// vectorized types on the current platform. It does not perform any more
// specific optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/kernel/internal/kernel_common.h"
#include "radio_core/math/math.h"

namespace radio_core::power_spectral_density_kernel_internal {

template <class Real, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(const std::span<const BaseComplex<Real>> samples,
                             const std::span<Real> power) -> std::span<Real> {
    using kernel_internal::VectorizedBase;

    using RealComplex = BaseComplex<Real>;
    using RealComplex4 =
        typename VectorizedBase<RealComplex>::template VectorizedType<4>;
    using RealComplex8 =
        typename VectorizedBase<RealComplex>::template VectorizedType<8>;

    using Real4 = typename VectorizedBase<Real>::template VectorizedType<4>;
    using Real8 = typename VectorizedBase<Real>::template VectorizedType<8>;

    assert(samples.size() == power.size());

    const size_t num_samples = samples.size();

    const RealComplex* __restrict samples_ptr = samples.data();
    Real* __restrict power_ptr = power.data();

    const RealComplex* samples_begin = samples_ptr;
    const RealComplex* samples_end = samples_ptr + num_samples;

    // Handle 8 elements at a time.
    if constexpr (RealComplex8::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const RealComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const RealComplex8 sample4(samples_ptr);
        const Real8 power8 = Real(10) * FastLog10(Norm(sample4));
        power8.Store(power_ptr);

        samples_ptr += 8;
        power_ptr += 8;
      }
    }

    // Handle 4 elements at a time.
    if constexpr (RealComplex4::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const RealComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const RealComplex4 sample4(samples_ptr);
        const Real4 power4 = Real(10) * FastLog10(Norm(sample4));
        power4.Store(power_ptr);

        samples_ptr += 4;
        power_ptr += 4;
      }
    }

    while (samples_ptr < samples_end) {
      *power_ptr = Real(10) * FastLog10(Norm(*samples_ptr));

      ++samples_ptr;
      ++power_ptr;
    }

    return power.subspan(0, num_samples);
  }
};

}  // namespace radio_core::power_spectral_density_kernel_internal
