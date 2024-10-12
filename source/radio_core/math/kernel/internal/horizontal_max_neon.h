// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Specialization of HorizontalMax() kernels for Arm Neon platforms.

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

namespace radio_core::kernel::horizontal_max_internal {

// Specialization for HorizontalMax<float>
template <>
struct Kernel<float, Float4::kIsVectorized> {
  static inline auto Execute(const std::span<const float>& samples) -> float {
    assert(!samples.empty());

    const size_t num_samples = samples.size();

    const float* samples_ptr = samples.data();
    const float* samples_begin = samples_ptr;
    const float* samples_end = samples_ptr + num_samples;

    float output = samples[0];

    // Handle 16 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2, using 4 registers gives
    // 30% more speedup compared to 2 registers.
    {
      Float4 output4_1(output);
      Float4 output4_2(output);
      Float4 output4_3(output);
      Float4 output4_4(output);

      const size_t num_samples_aligned = num_samples & ~size_t(15);
      const float* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Float4 samples4_1(samples_ptr);
        const Float4 samples4_2(samples_ptr + 4);
        const Float4 samples4_3(samples_ptr + 8);
        const Float4 samples4_4(samples_ptr + 12);

        samples_ptr += 16;

        output4_1 = Max(output4_1, samples4_1);
        output4_2 = Max(output4_2, samples4_2);
        output4_3 = Max(output4_3, samples4_3);
        output4_4 = Max(output4_4, samples4_4);
      }

      output = Max(output, HorizontalMax(output4_1));
      output = Max(output, HorizontalMax(output4_2));
      output = Max(output, HorizontalMax(output4_3));
      output = Max(output, HorizontalMax(output4_4));
    }

    // Handle 4 elements at a time.
    {
      Float4 output4(output);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const float* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Float4 samples4(samples_ptr);

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

#  if RADIO_CORE_HAVE_HALF

// Specialization for HorizontalMax<Half>
template <>
struct Kernel<Half, Half8::kIsVectorized> {
  static inline auto Execute(const std::span<const Half>& samples) -> Half {
    assert(!samples.empty());

    const size_t num_samples = samples.size();

    const Half* samples_ptr = samples.data();
    const Half* samples_begin = samples_ptr;
    const Half* samples_end = samples_ptr + num_samples;

    Half output = samples[0];

    // Handle 32 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2, using 4 registers gives
    // about 2x extra speedup compared to 2 registers.
    {
      Half8 output4_1(output);
      Half8 output4_2(output);
      Half8 output4_3(output);
      Half8 output4_4(output);

      const size_t num_samples_aligned = num_samples & ~size_t(31);
      const Half* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Half8 samples4_1(samples_ptr);
        const Half8 samples4_2(samples_ptr + 8);
        const Half8 samples4_3(samples_ptr + 16);
        const Half8 samples4_4(samples_ptr + 24);

        samples_ptr += 32;

        output4_1 = Max(output4_1, samples4_1);
        output4_2 = Max(output4_2, samples4_2);
        output4_3 = Max(output4_3, samples4_3);
        output4_4 = Max(output4_4, samples4_4);
      }

      output = Max(output, HorizontalMax(output4_1));
      output = Max(output, HorizontalMax(output4_2));
      output = Max(output, HorizontalMax(output4_3));
      output = Max(output, HorizontalMax(output4_4));
    }

    // Handle 8 elements at a time.
    {
      Half8 output8(output);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const Half* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        const Half8 samples4(samples_ptr);

        samples_ptr += 8;

        output8 = Max(output8, samples4);
      }

      output = Max(output, HorizontalMax(output8));
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      output = Max(output, *samples_ptr);

      ++samples_ptr;
    }

    return output;
  }
};

#  endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel::horizontal_max_internal

#endif  // ISA_CPU_ARM_NEON
