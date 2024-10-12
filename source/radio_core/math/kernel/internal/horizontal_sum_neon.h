// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Specialization of HorizontalSum() kernels for Arm Neon platforms.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include <cassert>
#  include <span>

#  include "radio_core/base/half.h"
#  include "radio_core/math/float4.h"

#  if RADIO_CORE_HAVE_HALF
#    include "radio_core/math/half8.h"
#  endif

namespace radio_core::kernel::horizontal_sum_internal {

// Specialization for HorizontalSum<float>
template <>
struct Kernel<float, Float4::kIsVectorized> {
  static inline auto Execute(const std::span<const float>& samples) -> float {
    assert(!samples.empty());

    const size_t num_samples = samples.size();

    const float* samples_ptr = samples.data();
    const float* samples_begin = samples_ptr;
    const float* samples_end = samples_ptr + num_samples;

    float output = 0.0f;

    // Handle 16 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2, using 4 registers gives
    // about 2x extra speedup compared to 2 registers.
    {
      Float4 output4_1(0.0f);
      Float4 output4_2(0.0f);
      Float4 output4_3(0.0f);
      Float4 output4_4(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(15);
      const float* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Float4 samples4_1(samples_ptr);
        const Float4 samples4_2(samples_ptr + 4);
        const Float4 samples4_3(samples_ptr + 8);
        const Float4 samples4_4(samples_ptr + 12);

        samples_ptr += 16;

        output4_1 += samples4_1;
        output4_2 += samples4_2;
        output4_3 += samples4_3;
        output4_4 += samples4_4;
      }

      output += HorizontalSum(output4_1);
      output += HorizontalSum(output4_2);
      output += HorizontalSum(output4_3);
      output += HorizontalSum(output4_4);
    }

    // Handle 4 elements at a time.
    {
      Float4 output4(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const float* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Float4 samples4(samples_ptr);

        samples_ptr += 4;

        output4 += samples4;
      }

      output += HorizontalSum(output4);
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      output += *samples_ptr;

      ++samples_ptr;
    }

    return output;
  }
};

#  if RADIO_CORE_HAVE_HALF

// Specialization for HorizontalSum<Half>
template <>
struct Kernel<Half, Half8::kIsVectorized> {
  static inline auto Execute(const std::span<const Half>& samples) -> Half {
    assert(!samples.empty());

    const size_t num_samples = samples.size();

    const Half* samples_ptr = samples.data();
    const Half* samples_begin = samples_ptr;
    const Half* samples_end = samples_ptr + num_samples;

    Half output = 0.0f;

    // Handle 32 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2, using 4 registers gives
    // about 2x extra speedup compared to 2 registers.
    {
      Half8 output4_1(0.0f);
      Half8 output4_2(0.0f);
      Half8 output4_3(0.0f);
      Half8 output4_4(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(31);
      const Half* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Half8 samples4_1(samples_ptr);
        const Half8 samples4_2(samples_ptr + 8);
        const Half8 samples4_3(samples_ptr + 16);
        const Half8 samples4_4(samples_ptr + 24);

        samples_ptr += 32;

        output4_1 += samples4_1;
        output4_2 += samples4_2;
        output4_3 += samples4_3;
        output4_4 += samples4_4;
      }

      output += HorizontalSum(output4_1);
      output += HorizontalSum(output4_2);
      output += HorizontalSum(output4_3);
      output += HorizontalSum(output4_4);
    }

    // Handle 8 elements at a time.
    {
      Half8 output8(0);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const Half* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Half8 samples4(samples_ptr);

        samples_ptr += 8;

        output8 += samples4;
      }

      output += HorizontalSum(output8);
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      output += *samples_ptr;

      ++samples_ptr;
    }

    return output;
  }
};

#  endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel::horizontal_sum_internal

#endif  // ISA_CPU_ARM_NEON
