// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/base/build_config.h"

#if ARCH_CPU_X86_FAMILY

#  include <immintrin.h>

namespace radio_core::internal::x86 {

// Multiply-add to accumulator:
//   RESULT[i] = a[i] + (b[i] * c[i]) for i = 0 to N
inline auto MultiplyAdd(const __m128& a, const __m128& b, const __m128& c)
    -> __m128 {
#  if ISA_CPU_X86_FMA
  return _mm_fmadd_ps(b, c, a);
#  else
  return _mm_add_ps(a, _mm_mul_ps(b, c));
#  endif
}

// Sum of all elements:
//  RESULT = a[0] + a[1] + a[2] + a[3]
inline auto HorizontalSum(const __m128& value) -> float {
#  if ISA_CPU_X86_SSE3
  const __m128 xx = _mm_hadd_ps(value, value);
  return _mm_cvtss_f32(_mm_hadd_ps(xx, xx));
#  else
  const __m128 hi =
      _mm_add_ps(_mm_shuffle_ps(value, value, _MM_SHUFFLE(1, 0, 3, 2)), value);
  return _mm_cvtss_f32(
      _mm_add_ps(_mm_shuffle_ps(hi, hi, _MM_SHUFFLE(2, 3, 0, 1)), hi));
#  endif
}

// Calculate the following polynomial:
//
//   c7*x^7 + c6*x^6 + c5*x^5 + c4*x^4 + c3*x^3 + c2*x^2 + c1*x + c0
inline auto CalculatePolynom(const __m128& x,
                             const float c0,
                             const float c1,
                             const float c2,
                             const float c3,
                             const float c4,
                             const float c5,
                             const float c6,
                             const float c7) -> __m128 {
  __m128 result = _mm_set1_ps(c7);

  result = MultiplyAdd(_mm_set1_ps(c6), result, x);
  result = MultiplyAdd(_mm_set1_ps(c5), result, x);
  result = MultiplyAdd(_mm_set1_ps(c4), result, x);
  result = MultiplyAdd(_mm_set1_ps(c3), result, x);
  result = MultiplyAdd(_mm_set1_ps(c2), result, x);
  result = MultiplyAdd(_mm_set1_ps(c1), result, x);
  result = MultiplyAdd(_mm_set1_ps(c0), result, x);

  return result;
}

// Approximate per-element element base-2 logarithm.
//
// The code is based on the one from the
//
//   https://jrfonseca.blogspot.com/2008/09/fast-sse2-pow-tables-or-polynomials.html
//
// with tweaks for FMA for performance. Additionally, uses a higher order
// polynomial for better accuracy.
inline auto ApproximateLog2(__m128 x) -> __m128 {
  const __m128i exp = _mm_set1_epi32(0x7f800000);
  const __m128i mant = _mm_set1_epi32(0x007fffff);

  const __m128 one = _mm_set1_ps(1.0f);

  const __m128i i = _mm_castps_si128(x);

  const __m128 e = _mm_cvtepi32_ps(_mm_sub_epi32(
      _mm_srli_epi32(_mm_and_si128(i, exp), 23), _mm_set1_epi32(127)));

  const __m128 m = _mm_or_ps(_mm_castsi128_ps(_mm_and_si128(i, mant)), one);

  // Minimax polynomial fit of Log2(x)/(x - 1), for x in range [1, 2[.
  //
  // Used minimax approximation tool from Boost:
  //
  //  https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/internals/minimax.html
  //
  // Variant              = 45
  // range                = [1.000000000000000000e+00,2.000000000000000000e+00]
  // Relative Error       = 1
  // Pin to Origin        = 0
  // Order (Num/Denom)    = 7/0
  // Target Precision     = 64
  // Working Precision    = 128
  // Skew                 = 0
  // Brake                = 50
  // X Offset             = 0.000000000000000000000e+00
  // X scale              = 1.000000000000000000000e+00
  // Y Offset             = 0.000000000000000000000e+00
  __m128 p = CalculatePolynom(m,
                              3.484752333259812739311f,
                              -5.010303889272714897639f,
                              5.842652591696923438221f,
                              -4.634291907077220346919f,
                              2.418069084345598673746f,
                              -7.957081900627795076299e-01f,
                              1.498442116273012398156e-01f,
                              -1.231947399129126435606e-02f);

  // This effectively increases the polynomial degree by one, but ensures that
  // ApproximateLog2(1) == 0.
  p = _mm_mul_ps(p, _mm_sub_ps(m, one));

  return _mm_add_ps(p, e);
}

}  // namespace radio_core::internal::x86

#endif  // ARCH_CPU_X86_FAMILY
