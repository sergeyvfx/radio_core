// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include <arm_neon.h>

namespace radio_core::internal::neon {

// Reciprocal of v, with higher precision than vrecpeq_f32.
inline auto vinvertq_f32(const float32x4_t v) -> float32x4_t {
  float32x4_t reciprocal = vrecpeq_f32(v);
  reciprocal = vmulq_f32(vrecpsq_f32(v, reciprocal), reciprocal);
  reciprocal = vmulq_f32(vrecpsq_f32(v, reciprocal), reciprocal);
  return reciprocal;
}

// Reciprocal of v, with higher precision than vrecpe_f32.
inline auto vinvert_f32(const float32x2_t v) -> float32x2_t {
  float32x2_t reciprocal = vrecpe_f32(v);
  reciprocal = vmul_f32(vrecps_f32(v, reciprocal), reciprocal);
  reciprocal = vmul_f32(vrecps_f32(v, reciprocal), reciprocal);
  return reciprocal;
}

// Higher precision variant of vrsqrte_f32().
inline auto vinvsqrt_f32(const float32x2_t v) -> float32x2_t {
  float32x2_t r = vrsqrte_f32(v);
  r = vmul_f32(vrsqrts_f32(vmul_f32(r, r), v), r);
  r = vmul_f32(vrsqrts_f32(vmul_f32(r, r), v), r);
  return r;
}

#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC

// Reciprocal of v, with higher precision than vrecpe_f16.
inline auto vinvert_f16(const float16x4_t v) -> float16x4_t {
  float16x4_t reciprocal = vrecpe_f16(v);
  reciprocal = vmul_f16(vrecps_f16(v, reciprocal), reciprocal);
  reciprocal = vmul_f16(vrecps_f16(v, reciprocal), reciprocal);
  return reciprocal;
}

// Reciprocal of v, with higher precision than vrecpeq_f16.
inline auto vinvertq_f16(const float16x8_t v) -> float16x8_t {
  float16x8_t r = vrecpeq_f16(v);
  r = vmulq_f16(vrecpsq_f16(v, r), r);
  r = vmulq_f16(vrecpsq_f16(v, r), r);
  return r;
}

// Inversed square root.
// Higher precision variant of vrsqrte_f16().
inline auto vinvsqrt_f16(const float16x4_t v) -> float16x4_t {
  float16x4_t r = vrsqrte_f16(v);
  r = vmul_f16(vrsqrts_f16(vmul_f16(r, r), v), r);
  r = vmul_f16(vrsqrts_f16(vmul_f16(r, r), v), r);
  return r;
}

// Inversed square root.
// Higher precision variant of vrsqrteq_f16().
inline auto vinvsqrtq_f16(const float16x8_t v) -> float16x8_t {
  float16x8_t r = vrsqrteq_f16(v);
  r = vmulq_f16(vrsqrtsq_f16(vmulq_f16(v, r), r), r);
  r = vmulq_f16(vrsqrtsq_f16(vmulq_f16(v, r), r), r);
  return r;
}

// Square root.
inline auto vsqrt_f16(const float16x4_t v) -> float16x4_t {
  return vinvert_f16(vinvsqrt_f16(v));
}

// Square root.
inline auto vsqrtq_f16(const float16x8_t v) -> float16x8_t {
  static float16x8_t zero = vmovq_n_f16(0);

  const float16x8_t r = vinvertq_f16(vinvsqrtq_f16(v));
  const uint16x8_t mask = vceqq_f16(v, zero);
  return vbslq_f16(mask, zero, r);
}

#  endif

#  if ARCH_CPU_32_BITS

// SImulation of intrinsics which are only available for 64bit platforms using
// 32bit instructions.
// Tested on BeagleBone Black board which uses AM335x CPU.

inline auto vdivq_f32(const float32x4_t a, const float32x4_t b) -> float32x4_t {
  const float32x4_t reciprocal = internal::neon::vinvertq_f32(b);
  return vmulq_f32(a, reciprocal);
}

inline auto vaddvq_f32(const float32x4_t v) -> float {
  float32x2_t r = vadd_f32(vget_low_f32(v), vget_high_f32(v));
  r = vpadd_f32(r, r);
  return vget_lane_f32(r, 0);
}

inline auto vsqrt_f32(const float32x2_t v) -> float32x2_t {
  return vinvert_f32(vinvsqrt_f32(v));
}

#  else
using ::vaddvq_f32;
using ::vdivq_f32;
using ::vsqrt_f32;
#  endif

// Reverse elements of the given vector.
inline auto vreveseq_f32(const float32x4_t v) -> float32x4_t {
#  if ARCH_CPU_64_BITS
  const float32x4_t vec = vrev64q_f32(v);
  return vextq_f32(vec, vec, 2);
#  else
  const float32x2_t lo = vrev64_f32(vget_high_f32(v));
  const float32x2_t hi = vrev64_f32(vget_low_f32(v));
  return vcombine_f32(lo, hi);
#  endif
}

// Reverse elements of the given vector.
inline auto vreveseq_u32(const uint32x4_t v) -> uint32x4_t {
#  if ARCH_CPU_64_BITS
  const uint32x4_t vec = vrev64q_u32(v);
  return vextq_u32(vec, vec, 2);
#  else
  const uint32x2_t lo = vrev64_u32(vget_high_u32(v));
  const uint32x2_t hi = vrev64_u32(vget_low_u32(v));
  return vcombine_u32(lo, hi);
#  endif
}

// Reverse elements of the given vector.
inline auto vreveseq_u16(const uint16x8_t v) -> uint16x8_t {
  const uint16x8_t vec = vrev64q_u16(v);
  return vextq_u16(vec, vec, 4);
}

// Reverse elements of the given vector.
#  if ARCH_CPU_64_BITS
inline auto vreveseq_f16(const float16x8_t v) -> float16x8_t {
#    if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                       \
        __ARM_FEATURE_FP16_VECTOR_ARITHMETIC
  const float16x8_t vec = vrev64q_f16(v);
  return vextq_f16(vec, vec, 4);
#    else
  // On platforms where FP16 vector arithmetic is not available cast to
  // uint16x8_t, reverse, and cast back.
  // The case where its is needed is Raspberry Pi CM4 and Clang 15.
  return vreinterpretq_f16_u16(vreveseq_u16(vreinterpretq_u16_f16(v)));
#    endif
}
#  endif

// clang-format off

// =============================================================================
// Adopted from Arm Compute Library.
//
// Version used: v22.02
//
// Copyright (c) 2016-2021 Arm Limited.
// SPDX-License-Identifier: MIT
// =============================================================================

// Exponent polynomial coefficients.
const float32x4_t exp_tab[8] =
{
  vdupq_n_f32(1.f),
  vdupq_n_f32(0.0416598916054f),
  vdupq_n_f32(0.500000596046f),
  vdupq_n_f32(0.0014122662833f),
  vdupq_n_f32(1.00000011921f),
  vdupq_n_f32(0.00833693705499f),
  vdupq_n_f32(0.166665703058f),
  vdupq_n_f32(0.000195780929062f),

};
// Logarithm polynomial coefficients.
const float32x4_t log_tab[8] =
{
  vdupq_n_f32(-2.29561495781f),
  vdupq_n_f32(-2.47071170807f),
  vdupq_n_f32(-5.68692588806f),
  vdupq_n_f32(-0.165253549814f),
  vdupq_n_f32(5.17591238022f),
  vdupq_n_f32(0.844007015228f),
  vdupq_n_f32(4.58445882797f),
  vdupq_n_f32(0.0141278216615f),
};

// Perform a 7th degree polynomial approximation using Estrin's method.
//
// @param[in] x      Input vector value in F32 format.
// @param[in] coeffs Polynomial coefficients table.
//
// @return The calculated approximation.
inline float32x4_t vtaylor_polyq_f32(float32x4_t x, const float32x4_t coeffs[8])
{
  float32x4_t A   = vmlaq_f32(coeffs[0], coeffs[4], x);
  float32x4_t B   = vmlaq_f32(coeffs[2], coeffs[6], x);
  float32x4_t C   = vmlaq_f32(coeffs[1], coeffs[5], x);
  float32x4_t D   = vmlaq_f32(coeffs[3], coeffs[7], x);
  float32x4_t x2  = vmulq_f32(x, x);
  float32x4_t x4  = vmulq_f32(x2, x2);
  float32x4_t res = vmlaq_f32(vmlaq_f32(A, B, x2), vmlaq_f32(C, D, x2), x4);
  return res;
}

// Calculate exponential
//
// @param[in] x Input vector value in F32 format.
//
// @return The calculated exponent.
inline float32x4_t vexpq_f32(float32x4_t x)
{
  static const float32x4_t CONST_LN2          = vdupq_n_f32(0.6931471805f); // ln(2)
  static const float32x4_t CONST_INV_LN2      = vdupq_n_f32(1.4426950408f); // 1/ln(2)
  static const float32x4_t CONST_INF          = vdupq_n_f32(std::numeric_limits<float>::infinity());
  static const float32x4_t CONST_MAX_INPUT    = vdupq_n_f32(88.7f);
  static const float32x4_t CONST_0            = vdupq_n_f32(0.f);
  static const int32x4_t   CONST_NEGATIVE_126 = vdupq_n_s32(-126);

  // Perform range reduction [-log(2),log(2)]
  int32x4_t   m   = vcvtq_s32_f32(vmulq_f32(x, CONST_INV_LN2));
  float32x4_t val = vmlsq_f32(x, vcvtq_f32_s32(m), CONST_LN2);

  // Polynomial Approximation
  float32x4_t poly = vtaylor_polyq_f32(val, exp_tab);

  // Reconstruct
  poly = vreinterpretq_f32_s32(vqaddq_s32(vreinterpretq_s32_f32(poly), vqshlq_n_s32(m, 23)));
  poly = vbslq_f32(vcltq_s32(m, CONST_NEGATIVE_126), CONST_0, poly); // Handle underflow
  poly = vbslq_f32(vcgtq_f32(x, CONST_MAX_INPUT), CONST_INF, poly);  // Handle overflow

  return poly;
}

// Calculate per-element exponent of the given 16bit floating point value.
// Internally it casts 16bit to 32bit value and uses vexpq_f32(). This should be
// good enough since the actual evaluation will use 32bit floating point math
// even when fully sticking to float16x4_t.
//
// TODO(sergey): Investigate whether having float16x4_t implementation allows
// to utilize more register simultaneously.
#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC
inline float16x4_t vexp_f16(float16x4_t x) {
  const float32x4_t r = vexpq_f32(vcvt_f32_f16(x));
  return vcvt_f16_f32(r);
}
#  endif

// A naive implementation of 8-element exponent for 16bit floating point values.
// It breaks down the calculation to two calculations of vexpq_f32().
//
// TODO(sergey): Investigate proper 16bit exponent.
#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC
inline float16x8_t vexpq_f16(float16x8_t x)
{
  const float32x4_t x_lo = vcvt_f32_f16(vget_low_f16(x));
  const float32x4_t x_hi = vcvt_f32_f16(vget_high_f16(x));

  const float32x4_t exp_lo = vexpq_f32(x_lo);
  const float32x4_t exp_hi = vexpq_f32(x_hi);

  return vcombine_f16(vcvt_f16_f32(exp_lo), vcvt_f16_f32(exp_hi));
}
#endif

// Calculate logarithm
//
// @param[in] x Input vector value in F32 format.
//
// @return The calculated logarithm.
inline auto vlogq_f32(float32x4_t x) -> float32x4_t
{
    static const int32x4_t   CONST_127 = vdupq_n_s32(127);           // 127
    static const float32x4_t CONST_LN2 = vdupq_n_f32(0.6931471805f); // Log(2)

    // Extract exponent
    int32x4_t   m   = vsubq_s32(vreinterpretq_s32_u32(vshrq_n_u32(vreinterpretq_u32_f32(x), 23)), CONST_127);
    float32x4_t val = vreinterpretq_f32_s32(vsubq_s32(vreinterpretq_s32_f32(x), vshlq_n_s32(m, 23)));

    // Polynomial Approximation
    float32x4_t poly = vtaylor_polyq_f32(val, log_tab);

    // Reconstruct
    poly = vmlaq_f32(poly, vcvtq_f32_s32(m), CONST_LN2);

    return poly;
}

#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC

inline float16x8_t vlogq_f16(float16x8_t x)
{
    const float32x4_t x_high = vcvt_f32_f16(vget_high_f16(x));
    const float32x4_t x_low  = vcvt_f32_f16(vget_low_f16(x));

    const float16x8_t res = vcombine_f16(vcvt_f16_f32(vlogq_f32(x_low)), vcvt_f16_f32(vlogq_f32(x_high)));
    return res;
}

#  endif  // __ARM_FEATURE_FP16_VECTOR_ARITHMETIC

// =============================================================================
// Adopted from Simple ARM NEON optimized sin, cos, log and exp
// http://gruntthepeon.free.fr/ssemath/neon_mathfun.html
//
// These functions have better precision than their alternatives from the
// ARM Compute Library.
//
// There are local expansion of the code to support 16 bit floating point
// values. The modification is also adding 8-element 16bit floating point
// variant of the function.
//
// Copyright (C) 2011 Julien Pommier
// SPDX-License-Identifier: Zlib
// =============================================================================

constexpr float c_minus_cephes_DP1 = -0.78515625;
constexpr float c_minus_cephes_DP2 = -2.4187564849853515625e-4;
constexpr float c_minus_cephes_DP3 = -3.77489497744594108e-8;
constexpr float c_sincof_p0 = -1.9515295891E-4;
constexpr float c_sincof_p1 = 8.3321608736E-3;
constexpr float c_sincof_p2 = -1.6666654611E-1;
constexpr float c_coscof_p0 = 2.443315711809948E-005;
constexpr float c_coscof_p1 = -1.388731625493765E-003;
constexpr float c_coscof_p2 = 4.166664568298827E-002;
constexpr float c_cephes_FOPI = 1.27323954473516;  // 4 / M_PI

/* evaluation of 4 sines & cosines at once.

   The code is the exact rewriting of the cephes sinf function.
   Precision is excellent as long as x < 8192 (I did not bother to
   take into account the special handling they have for greater values
   -- it does not return garbage for arguments over 8192, though, but
   the extra precision is missing).

   Note that it is such that sinf((float)M_PI) = 8.74e-8, which is the
   surprising but correct result.

   Note also that when you compute sin(x), cos(x) is available at
   almost no extra price so both sin_ps and cos_ps make use of
   sincos_ps..
  */
inline void vsincosq_f32(float32x4_t x,
                         float32x4_t* ysin,
                         float32x4_t* ycos) {  // any x
  float32x4_t xmm1, xmm2, xmm3, y;

  uint32x4_t emm2;

  uint32x4_t sign_mask_sin, sign_mask_cos;
  sign_mask_sin = vcltq_f32(x, vdupq_n_f32(0));
  x = vabsq_f32(x);

  /* scale by 4/Pi */
  y = vmulq_f32(x, vdupq_n_f32(c_cephes_FOPI));

  /* store the integer part of y in mm0 */
  emm2 = vcvtq_u32_f32(y);
  /* j=(j+1) & (~1) (see the cephes sources) */
  emm2 = vaddq_u32(emm2, vdupq_n_u32(1));
  emm2 = vandq_u32(emm2, vdupq_n_u32(~1));
  y = vcvtq_f32_u32(emm2);

  /* get the polynom selection mask
     there is one polynom for 0 <= x <= Pi/4
     and another one for Pi/4<x<=Pi/2

     Both branches will be computed.
  */
  uint32x4_t poly_mask = vtstq_u32(emm2, vdupq_n_u32(2));

  /* The magic pass: "Extended precision modular arithmetic"
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = vmulq_n_f32(y, c_minus_cephes_DP1);
  xmm2 = vmulq_n_f32(y, c_minus_cephes_DP2);
  xmm3 = vmulq_n_f32(y, c_minus_cephes_DP3);
  x = vaddq_f32(x, xmm1);
  x = vaddq_f32(x, xmm2);
  x = vaddq_f32(x, xmm3);

  sign_mask_sin = veorq_u32(sign_mask_sin, vtstq_u32(emm2, vdupq_n_u32(4)));
  sign_mask_cos = vtstq_u32(vsubq_u32(emm2, vdupq_n_u32(2)), vdupq_n_u32(4));

  /* Evaluate the first polynom  (0 <= x <= Pi/4) in y1,
     and the second polynom      (Pi/4 <= x <= 0) in y2 */
  float32x4_t z = vmulq_f32(x, x);
  float32x4_t y1, y2;

  y1 = vmulq_n_f32(z, c_coscof_p0);
  y2 = vmulq_n_f32(z, c_sincof_p0);
  y1 = vaddq_f32(y1, vdupq_n_f32(c_coscof_p1));
  y2 = vaddq_f32(y2, vdupq_n_f32(c_sincof_p1));
  y1 = vmulq_f32(y1, z);
  y2 = vmulq_f32(y2, z);
  y1 = vaddq_f32(y1, vdupq_n_f32(c_coscof_p2));
  y2 = vaddq_f32(y2, vdupq_n_f32(c_sincof_p2));
  y1 = vmulq_f32(y1, z);
  y2 = vmulq_f32(y2, z);
  y1 = vmulq_f32(y1, z);
  y2 = vmulq_f32(y2, x);
  y1 = vsubq_f32(y1, vmulq_f32(z, vdupq_n_f32(0.5f)));
  y2 = vaddq_f32(y2, x);
  y1 = vaddq_f32(y1, vdupq_n_f32(1));

  /* select the correct result from the two polynoms */
  float32x4_t ys = vbslq_f32(poly_mask, y1, y2);
  float32x4_t yc = vbslq_f32(poly_mask, y2, y1);
  *ysin = vbslq_f32(sign_mask_sin, vnegq_f32(ys), ys);
  *ycos = vbslq_f32(sign_mask_cos, yc, vnegq_f32(yc));
}

inline float32x4_t vsinq_f32(float32x4_t x) {
  float32x4_t ysin, ycos;
  vsincosq_f32(x, &ysin, &ycos);
  return ysin;
}

inline float32x4_t vcosq_f32(float32x4_t x) {
  float32x4_t ysin, ycos;
  vsincosq_f32(x, &ysin, &ycos);
  return ycos;
}

// Calculate per-element sine and cosine of the given 16bit floating point
// value.
// Internally it casts 16bit to 32bit value and uses vsincosq_f32(). This should
// be good enough since the actual evaluation will use 32bit floating point math
// even when fully sticking to float16x4_t.
//
// TODO(sergey): Investigate whether having float16x4_t implementation allows
// to utilize more register simultaneously.
#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC
inline void vsincos_f16(float16x4_t x, float16x4_t* ysin, float16x4_t* ycos) {
  float32x4_t ysin32, ycos32;
  vsincosq_f32(vcvt_f32_f16(x), &ysin32, &ycos32);
  *ysin = vcvt_f16_f32(ysin32);
  *ycos = vcvt_f16_f32(ycos32);
}

inline float16x4_t vsin_f16(float16x4_t x) {
  float16x4_t ysin, ycos;
  vsincos_f16(x, &ysin, &ycos);
  return ysin;
}

inline float16x4_t vcos_f16(float16x4_t x) {
  float16x4_t ysin, ycos;
  vsincos_f16(x, &ysin, &ycos);
  return ycos;
}
#endif

#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
__ARM_FEATURE_FP16_VECTOR_ARITHMETIC

// A port of vsincosq_f32 to operate on 8 16bit floating point values.
inline void vsincosq_f16(float16x8_t x, float16x8_t* ysin, float16x8_t* ycos) {
  float16x8_t xmm1, xmm2, xmm3, y;

  uint16x8_t emm2;

  uint16x8_t sign_mask_sin, sign_mask_cos;
  sign_mask_sin = vcltq_f16(x, vdupq_n_f16(0));
  x = vabsq_f16(x);

  /* scale by 4/Pi */
  y = vmulq_f16(x, vdupq_n_f16(c_cephes_FOPI));

  /* store the integer part of y in mm0 */
  emm2 = vcvtq_u16_f16(y);
  /* j=(j+1) & (~1) (see the cephes sources) */
  emm2 = vaddq_u16(emm2, vdupq_n_u16(1));
  emm2 = vandq_u16(emm2, vdupq_n_u16(~1));
  y = vcvtq_f16_u16(emm2);

  /* get the polynom selection mask
     there is one polynom for 0 <= x <= Pi/4
     and another one for Pi/4<x<=Pi/2

     Both branches will be computed.
  */
  uint16x8_t poly_mask = vtstq_u16(emm2, vdupq_n_u16(2));

  /* The magic pass: "Extended precision modular arithmetic"
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = vmulq_n_f16(y, c_minus_cephes_DP1);
  xmm2 = vmulq_n_f16(y, c_minus_cephes_DP2);
  xmm3 = vmulq_n_f16(y, c_minus_cephes_DP3);
  x = vaddq_f16(x, xmm1);
  x = vaddq_f16(x, xmm2);
  x = vaddq_f16(x, xmm3);

  sign_mask_sin = veorq_u16(sign_mask_sin, vtstq_u16(emm2, vdupq_n_u16(4)));
  sign_mask_cos = vtstq_u16(vsubq_u16(emm2, vdupq_n_u16(2)), vdupq_n_u16(4));

  /* Evaluate the first polynom  (0 <= x <= Pi/4) in y1,
     and the second polynom      (Pi/4 <= x <= 0) in y2 */
  float16x8_t z = vmulq_f16(x, x);
  float16x8_t y1, y2;

  y1 = vmulq_n_f16(z, c_coscof_p0);
  y2 = vmulq_n_f16(z, c_sincof_p0);
  y1 = vaddq_f16(y1, vdupq_n_f16(c_coscof_p1));
  y2 = vaddq_f16(y2, vdupq_n_f16(c_sincof_p1));
  y1 = vmulq_f16(y1, z);
  y2 = vmulq_f16(y2, z);
  y1 = vaddq_f16(y1, vdupq_n_f16(c_coscof_p2));
  y2 = vaddq_f16(y2, vdupq_n_f16(c_sincof_p2));
  y1 = vmulq_f16(y1, z);
  y2 = vmulq_f16(y2, z);
  y1 = vmulq_f16(y1, z);
  y2 = vmulq_f16(y2, x);
  y1 = vsubq_f16(y1, vmulq_f16(z, vdupq_n_f16(0.5f)));
  y2 = vaddq_f16(y2, x);
  y1 = vaddq_f16(y1, vdupq_n_f16(1));

  /* select the correct result from the two polynoms */
  float16x8_t ys = vbslq_f16(poly_mask, y1, y2);
  float16x8_t yc = vbslq_f16(poly_mask, y2, y1);
  *ysin = vbslq_f16(sign_mask_sin, vnegq_f16(ys), ys);
  *ycos = vbslq_f16(sign_mask_cos, yc, vnegq_f16(yc));
}

inline float16x8_t vsinq_f16(float16x8_t x) {
  float16x8_t ysin, ycos;
  vsincosq_f16(x, &ysin, &ycos);
  return ysin;
}

inline float16x8_t vcosq_f16(float16x8_t x) {
  float16x8_t ysin, ycos;
  vsincosq_f16(x, &ysin, &ycos);
  return ycos;
}

#endif

// clang-format on

#endif  // ISA_CPU_ARM_NEON

}  // namespace radio_core::internal::neon
