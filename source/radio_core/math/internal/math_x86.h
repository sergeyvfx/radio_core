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

// =============================================================================
// Adopted from Simple SSE and SSE2 (and now NEON) optimized sin, cos, log and
// exp
// http://gruntthepeon.free.fr/ssemath/
//
// Simplified code by requiring SSE2 as the bare minimum.
//
// Copyright (C) 2007 Julien Pommier
// SPDX-License-Identifier: Zlib
// =============================================================================

const __m128 _ps_1 = _mm_set1_ps(1.0f);
const __m128 _ps_0p5 = _mm_set1_ps(0.5f);

const __m128 _ps_sign_mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
const __m128 _ps_inv_sign_mask = _mm_castsi128_ps(_mm_set1_epi32(~0x80000000));

const __m128i _pi32_1 = _mm_set1_epi32(1);
const __m128i _pi32_inv1 = _mm_set1_epi32(~1);
const __m128i _pi32_2 = _mm_set1_epi32(2);
const __m128i _pi32_4 = _mm_set1_epi32(4);
const __m128i _pi32_0x7f = _mm_set1_epi32(0x7f);

const __m128 _ps_exp_hi = _mm_set1_ps(88.3762626647949f);
const __m128 _ps_exp_lo = _mm_set1_ps(-88.3762626647949f);

const __m128 _ps_cephes_LOG2EF = _mm_set1_ps(1.44269504088896341);
const __m128 _ps_cephes_exp_C1 = _mm_set1_ps(0.693359375);
const __m128 _ps_cephes_exp_C2 = _mm_set1_ps(-2.12194440e-4);

const __m128 _ps_cephes_exp_p0 = _mm_set1_ps(1.9875691500E-4);
const __m128 _ps_cephes_exp_p1 = _mm_set1_ps(1.3981999507E-3);
const __m128 _ps_cephes_exp_p2 = _mm_set1_ps(8.3334519073E-3);
const __m128 _ps_cephes_exp_p3 = _mm_set1_ps(4.1665795894E-2);
const __m128 _ps_cephes_exp_p4 = _mm_set1_ps(1.6666665459E-1);
const __m128 _ps_cephes_exp_p5 = _mm_set1_ps(5.0000001201E-1);

inline __m128 exp_ps(__m128 x) {
  __m128 tmp = _mm_setzero_ps(), fx;
  __m128i emm0;
  __m128 one = _ps_1;

  x = _mm_min_ps(x, _ps_exp_hi);
  x = _mm_max_ps(x, _ps_exp_lo);

  /* express exp(x) as exp(g + n*log(2)) */
  fx = _mm_mul_ps(x, _ps_cephes_LOG2EF);
  fx = _mm_add_ps(fx, _ps_0p5);

  /* how to perform a floorf with SSE: just below */
  emm0 = _mm_cvttps_epi32(fx);
  tmp = _mm_cvtepi32_ps(emm0);
  /* if greater, substract 1 */
  __m128 mask = _mm_cmpgt_ps(tmp, fx);
  mask = _mm_and_ps(mask, one);
  fx = _mm_sub_ps(tmp, mask);

  tmp = _mm_mul_ps(fx, _ps_cephes_exp_C1);
  __m128 z = _mm_mul_ps(fx, _ps_cephes_exp_C2);
  x = _mm_sub_ps(x, tmp);
  x = _mm_sub_ps(x, z);

  z = _mm_mul_ps(x, x);

  __m128 y = _ps_cephes_exp_p0;
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, _ps_cephes_exp_p1);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, _ps_cephes_exp_p2);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, _ps_cephes_exp_p3);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, _ps_cephes_exp_p4);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, _ps_cephes_exp_p5);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, x);
  y = _mm_add_ps(y, one);

  /* build 2^n */
  emm0 = _mm_cvttps_epi32(fx);
  emm0 = _mm_add_epi32(emm0, _pi32_0x7f);
  emm0 = _mm_slli_epi32(emm0, 23);
  __m128 pow2n = _mm_castsi128_ps(emm0);
  y = _mm_mul_ps(y, pow2n);
  return y;
}

const __m128 _ps_minus_cephes_DP1 = _mm_set1_ps(-0.78515625f);
const __m128 _ps_minus_cephes_DP2 = _mm_set1_ps(-2.4187564849853515625e-4f);
const __m128 _ps_minus_cephes_DP3 = _mm_set1_ps(-3.77489497744594108e-8f);
const __m128 _ps_sincof_p0 = _mm_set1_ps(-1.9515295891E-4f);
const __m128 _ps_sincof_p1 = _mm_set1_ps(8.3321608736E-3f);
const __m128 _ps_sincof_p2 = _mm_set1_ps(-1.6666654611E-1f);
const __m128 _ps_coscof_p0 = _mm_set1_ps(2.443315711809948E-005f);
const __m128 _ps_coscof_p1 = _mm_set1_ps(-1.388731625493765E-003f);
const __m128 _ps_coscof_p2 = _mm_set1_ps(4.166664568298827E-002f);
const __m128 _ps_cephes_FOPI = _mm_set1_ps(1.27323954473516f);  // 4 / M_PI

/* since sin_ps and cos_ps are almost identical, sincos_ps could replace both of
   them.. it is almost as fast, and gives you a free cosine with your sine */
inline void sincos_ps(__m128 x, __m128* s, __m128* c) {
  __m128 xmm1, xmm2, xmm3 = _mm_setzero_ps(), sign_bit_sin, y;
  __m128i emm0, emm2, emm4;
  sign_bit_sin = x;
  /* take the absolute value */
  x = _mm_and_ps(x, _ps_inv_sign_mask);
  /* extract the sign bit (upper one) */
  sign_bit_sin = _mm_and_ps(sign_bit_sin, _ps_sign_mask);

  /* scale by 4/Pi */
  y = _mm_mul_ps(x, _ps_cephes_FOPI);

  /* store the integer part of y in emm2 */
  emm2 = _mm_cvttps_epi32(y);

  /* j=(j+1) & (~1) (see the cephes sources) */
  emm2 = _mm_add_epi32(emm2, _pi32_1);
  emm2 = _mm_and_si128(emm2, _pi32_inv1);
  y = _mm_cvtepi32_ps(emm2);

  emm4 = emm2;

  /* get the swap sign flag for the sine */
  emm0 = _mm_and_si128(emm2, _pi32_4);
  emm0 = _mm_slli_epi32(emm0, 29);
  __m128 swap_sign_bit_sin = _mm_castsi128_ps(emm0);

  /* get the polynom selection mask for the sine*/
  emm2 = _mm_and_si128(emm2, _pi32_2);
  emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
  __m128 poly_mask = _mm_castsi128_ps(emm2);

  /* The magic pass: "Extended precision modular arithmetic"
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = _ps_minus_cephes_DP1;
  xmm2 = _ps_minus_cephes_DP2;
  xmm3 = _ps_minus_cephes_DP3;
  xmm1 = _mm_mul_ps(y, xmm1);
  xmm2 = _mm_mul_ps(y, xmm2);
  xmm3 = _mm_mul_ps(y, xmm3);
  x = _mm_add_ps(x, xmm1);
  x = _mm_add_ps(x, xmm2);
  x = _mm_add_ps(x, xmm3);

  emm4 = _mm_sub_epi32(emm4, _pi32_2);
  emm4 = _mm_andnot_si128(emm4, _pi32_4);
  emm4 = _mm_slli_epi32(emm4, 29);
  __m128 sign_bit_cos = _mm_castsi128_ps(emm4);

  sign_bit_sin = _mm_xor_ps(sign_bit_sin, swap_sign_bit_sin);

  /* Evaluate the first polynom  (0 <= x <= Pi/4) */
  __m128 z = _mm_mul_ps(x, x);
  y = _ps_coscof_p0;

  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, _ps_coscof_p1);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, _ps_coscof_p2);
  y = _mm_mul_ps(y, z);
  y = _mm_mul_ps(y, z);
  __m128 tmp = _mm_mul_ps(z, _ps_0p5);
  y = _mm_sub_ps(y, tmp);
  y = _mm_add_ps(y, _ps_1);

  /* Evaluate the second polynom  (Pi/4 <= x <= 0) */

  __m128 y2 = _ps_sincof_p0;
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, _ps_sincof_p1);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, _ps_sincof_p2);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_mul_ps(y2, x);
  y2 = _mm_add_ps(y2, x);

  /* select the correct result from the two polynoms */
  xmm3 = poly_mask;
  __m128 ysin2 = _mm_and_ps(xmm3, y2);
  __m128 ysin1 = _mm_andnot_ps(xmm3, y);
  y2 = _mm_sub_ps(y2, ysin2);
  y = _mm_sub_ps(y, ysin1);

  xmm1 = _mm_add_ps(ysin1, ysin2);
  xmm2 = _mm_add_ps(y, y2);

  /* update the sign */
  *s = _mm_xor_ps(xmm1, sign_bit_sin);
  *c = _mm_xor_ps(xmm2, sign_bit_cos);
}

inline __m128 sin_ps(__m128 x) {
  __m128 ysin, ycos;
  sincos_ps(x, &ysin, &ycos);
  return ysin;
}

inline __m128 cos_ps(__m128 x) {
  __m128 ysin, ycos;
  sincos_ps(x, &ysin, &ycos);
  return ycos;
}

}  // namespace radio_core::internal::x86

#endif  // ARCH_CPU_X86_FAMILY
