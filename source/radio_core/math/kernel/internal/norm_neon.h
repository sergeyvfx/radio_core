// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Specialization of Norm() kernels for Arm Neon platforms.

#pragma once

#include "radio_core/base/build_config.h"

#include <cassert>
#include <span>

#if ISA_CPU_ARM_NEON

#  include "radio_core/math/complex4.h"
#  include "radio_core/math/float4.h"
#  include "radio_core/math/kernel/internal/kernel_common.h"
#  include "radio_core/math/math.h"

namespace radio_core::norm_kernel_internal {

template <class T, bool SpecializationMarker>
struct Kernel;

// Specialization for Norm<Complex, float>
template <>
struct Kernel<Complex, Complex4::kIsVectorized> {
  static inline auto Execute(const std::span<const Complex>& samples,
                             const std::span<float>& absolute_values)
      -> std::span<float> {
    assert(samples.size() <= absolute_values.size());

    const size_t num_samples = samples.size();

    // On Apple M2 and clang-1403.0.22.14.1 this seems to be barely giving
    // advantage. Other CPUs need investigation.
    //
    // Using multiple register to perform calculation does not
    // seem to help.
    const Complex* __restrict samples_ptr = samples.data();
    float* __restrict absolute_values_ptr = absolute_values.data();

    const Complex* samples_begin = samples_ptr;
    const Complex* samples_end = samples_ptr + num_samples;

    // Handle 4 elements at a time.
    if constexpr (Complex4::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const Complex* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const Complex4 samples4(samples_ptr);
        const Float4 abs4 = Norm(samples4);

        abs4.Store(absolute_values_ptr);

        samples_ptr += 4;
        absolute_values_ptr += 4;
      }
    }

    // Handle up to 3 remaining elements.
    while (samples_ptr < samples_end) {
      *absolute_values_ptr = Norm(*samples_ptr);

      ++samples_ptr;
      ++absolute_values_ptr;
    }

    return absolute_values.subspan(0, num_samples);
  }
};

#  if RADIO_CORE_HAVE_HALF

// Specialization for Norm<HalfComplex, float>
template <>
struct Kernel<HalfComplex, HalfComplex8::kIsVectorized> {
  static inline auto Execute(const std::span<const HalfComplex>& samples,
                             const std::span<Half>& absolute_values)
      -> std::span<Half> {
    assert(samples.size() <= absolute_values.size());

    const size_t num_samples = samples.size();

    const HalfComplex* __restrict samples_ptr = samples.data();
    Half* __restrict absolute_values_ptr = absolute_values.data();

    const HalfComplex* samples_begin = samples_ptr;
    const HalfComplex* samples_end = samples_ptr + num_samples;

    // Handle 8 elements at a time.
    {
      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const HalfComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const HalfComplex8 samples4(samples_ptr);
        const Half8 abs4 = Norm(samples4);

        abs4.Store(absolute_values_ptr);

        samples_ptr += 8;
        absolute_values_ptr += 8;
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

#  endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::norm_kernel_internal

#endif  // ISA_CPU_ARM_NEON
