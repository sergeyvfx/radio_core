// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Specialization of dot-product flip kernels for Arm Neon platforms.

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

namespace radio_core::experimental::dot_flip_kernel_internal {

template <class FType, class GType, bool SpecializationMarker>
struct Kernel;

// Specialization for Dot<float, float>
template <>
struct Kernel<float, float, Float4::kIsVectorized> {
  static inline auto Execute(const std::span<const float>& f,
                             const std::span<const float>& g) -> float {
    assert(f.size() == g.size());

    const size_t num_samples = f.size();

    const float* f_ptr = f.data();
    const float* f_begin = f_ptr;
    const float* f_end = f_ptr + num_samples;

    const float* g_ptr = g.data() + num_samples - 1;

    float output = 0;

    // Handle 16 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2, using 4 registers gives
    // 30% more speedup compared to 2 registers.
    {
      Float4 output4_1(0.0f);
      Float4 output4_2(0.0f);
      Float4 output4_3(0.0f);
      Float4 output4_4(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(15);
      const float* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const Float4 f4_1(f_ptr);
        const Float4 f4_2(f_ptr + 4);
        const Float4 f4_3(f_ptr + 8);
        const Float4 f4_4(f_ptr + 12);

        const Float4 g4_4 = Reverse(Float4(g_ptr - 15));
        const Float4 g4_3 = Reverse(Float4(g_ptr - 11));
        const Float4 g4_2 = Reverse(Float4(g_ptr - 7));
        const Float4 g4_1 = Reverse(Float4(g_ptr - 3));

        f_ptr += 16;
        g_ptr -= 16;

        output4_1 = MultiplyAdd(output4_1, f4_1, g4_1);
        output4_2 = MultiplyAdd(output4_2, f4_2, g4_2);
        output4_3 = MultiplyAdd(output4_3, f4_3, g4_3);
        output4_4 = MultiplyAdd(output4_4, f4_4, g4_4);
      }

      output += HorizontalSum(output4_1 + output4_2 + output4_3 + output4_4);
    }

    // Handle up to 15 elements 4 elements at a time.
    {
      Float4 output4(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const float* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const Float4 f4(f_ptr);
        const Float4 g4 = Reverse(Float4(g_ptr - 3));

        f_ptr += 4;
        g_ptr -= 4;

        output4 = MultiplyAdd(output4, f4, g4);
      }

      output += HorizontalSum(output4);
    }

    // Handle up to 3 remaining elements.
    while (f_ptr < f_end) {
      output = MultiplyAdd(output, *f_ptr, *g_ptr);

      ++f_ptr;
      --g_ptr;
    }

    return output;
  }
};

// Specialization for Dot<Complex, float>
template <>
struct Kernel<Complex, float, Complex4::kIsVectorized> {
  static inline auto Execute(const std::span<const Complex>& f,
                             const std::span<const float>& g) -> Complex {
    assert(f.size() == g.size());

    const size_t num_samples = f.size();

    const Complex* f_ptr = f.data();
    const Complex* f_begin = f_ptr;
    const Complex* f_end = f_ptr + num_samples;

    const float* g_ptr = g.data() + num_samples - 1;

    Complex output(0);

    // Handle 8 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    {
      Complex4 output4_1(Complex(0));
      Complex4 output4_2(Complex(0));

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const Complex* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const Complex4 f4_1(f_ptr);
        const Complex4 f4_2(f_ptr + 4);

        const Float4 g4_2 = Reverse(Float4(g_ptr - 7));
        const Float4 g4_1 = Reverse(Float4(g_ptr - 3));

        f_ptr += 8;
        g_ptr -= 8;

        output4_1 = MultiplyAdd(output4_1, f4_1, g4_1);
        output4_2 = MultiplyAdd(output4_2, f4_2, g4_2);
      }

      output += HorizontalSum(output4_1 + output4_2);
    }

    // Handle up to 15 elements 4 elements at a time.
    {
      Complex4 output4(Complex(0));

      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const Complex* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const Complex4 f4(f_ptr);
        const Float4 g4 = Reverse(Float4(g_ptr - 3));

        f_ptr += 4;
        g_ptr -= 4;

        output4 = MultiplyAdd(output4, f4, g4);
      }

      output += HorizontalSum(output4);
    }

    // Handle up to 3 remaining elements.
    while (f_ptr < f_end) {
      output = MultiplyAdd(output, *f_ptr, *g_ptr);

      ++f_ptr;
      --g_ptr;
    }

    return output;
  }
};

#  if RADIO_CORE_HAVE_HALF

// Specialization for Dot<Half, Half>
template <>
struct Kernel<Half, Half, Half8::kIsVectorized> {
  static inline auto Execute(const std::span<const Half>& f,
                             const std::span<const Half>& g) -> Half {
    assert(f.size() == g.size());

    const size_t num_samples = f.size();

    const Half* f_ptr = f.data();
    const Half* f_begin = f_ptr;
    const Half* f_end = f_ptr + num_samples;

    const Half* g_ptr = g.data() + num_samples - 1;

    Half output(0);

    // Handle 16 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2, using 4 registers gives
    // 40% more speedup compared to 2 registers.
    {
      Half8 output8_1(0.0f);
      Half8 output8_2(0.0f);
      Half8 output8_3(0.0f);
      Half8 output8_4(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(31);
      const Half* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const Half8 f8_1(f_ptr);
        const Half8 f8_2(f_ptr + 8);
        const Half8 f8_3(f_ptr + 16);
        const Half8 f8_4(f_ptr + 24);

        const Half8 g8_1 = Reverse(Half8(g_ptr - 7));
        const Half8 g8_2 = Reverse(Half8(g_ptr - 15));
        const Half8 g8_3 = Reverse(Half8(g_ptr - 23));
        const Half8 g8_4 = Reverse(Half8(g_ptr - 31));

        f_ptr += 32;
        g_ptr -= 32;

        output8_1 = MultiplyAdd(output8_1, f8_1, g8_1);
        output8_2 = MultiplyAdd(output8_2, f8_2, g8_2);
        output8_3 = MultiplyAdd(output8_3, f8_3, g8_3);
        output8_4 = MultiplyAdd(output8_4, f8_4, g8_4);
      }

      output += HorizontalSum(output8_1 + output8_2 + output8_3 + output8_4);
    }

    // Handle up to 31 elements 8 elements at a time.
    {
      Half8 output8(0.0f);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const Half* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const Half8 f8(f_ptr);
        const Half8 g8 = Reverse(Half8(g_ptr - 7));

        f_ptr += 8;
        g_ptr -= 8;

        output8 = MultiplyAdd(output8, f8, g8);
      }

      output += HorizontalSum(output8);
    }

    // Handle up to 7 remaining elements.
    while (f_ptr < f_end) {
      output = MultiplyAdd(output, *f_ptr, *g_ptr);

      ++f_ptr;
      --g_ptr;
    }

    return output;
  }
};

// Specialization for Dot<HalfComplex, Half>
template <>
struct Kernel<HalfComplex, Half, Half8::kIsVectorized> {
  static inline auto Execute(const std::span<const HalfComplex>& f,
                             const std::span<const Half>& g) -> HalfComplex {
    assert(f.size() == g.size());

    const size_t num_samples = f.size();

    const HalfComplex* f_ptr = f.data();
    const HalfComplex* f_begin = f_ptr;
    const HalfComplex* f_end = f_ptr + num_samples;

    const Half* g_ptr = g.data() + num_samples - 1;

    HalfComplex output(0);

    // Handle 16 elements at a time.
    // Accumulate into multiple registers, which more fully utilizes the
    // available resources.
    // Using 2 registers is about 2x faster on Apple M2.
    {
      HalfComplex8 output8_1(HalfComplex(0));
      HalfComplex8 output8_2(HalfComplex(0));

      const size_t num_samples_aligned = num_samples & ~size_t(15);
      const HalfComplex* aligned_f_end = f_begin + num_samples_aligned;

      while (f_ptr < aligned_f_end) {
        const HalfComplex8 f8_1(f_ptr);
        const HalfComplex8 f8_2(f_ptr + 8);

        const Half8 g8_2 = Reverse(Half8(g_ptr - 15));
        const Half8 g8_1 = Reverse(Half8(g_ptr - 7));

        f_ptr += 16;
        g_ptr -= 16;

        output8_1 = MultiplyAdd(output8_1, f8_1, g8_1);
        output8_2 = MultiplyAdd(output8_2, f8_2, g8_2);
      }

      output += HorizontalSum(output8_1 + output8_2);
    }

    // Handle up to 15 remaining elements.
    while (f_ptr < f_end) {
      output = MultiplyAdd(output, *f_ptr, *g_ptr);

      ++f_ptr;
      --g_ptr;
    }

    return output;
  }
};

#  endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::experimental::dot_flip_kernel_internal

#endif  // ISA_CPU_ARM_NEON
