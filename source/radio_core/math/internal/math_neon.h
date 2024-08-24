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
inline auto vreveseq_f16(const float16x8_t v) -> float16x8_t {
#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC
  const float16x8_t vec = vrev64q_f16(v);
  return vextq_f16(vec, vec, 4);
#  else
  // On platforms where FP16 vector arithmetic is not available cast to
  // uint16x8_t, reverse, and cast back.
  // The case where tis is needed is Raspberry Pi CM4 and Clang 15.
  return vreinterpretq_f16_u16(vreveseq_u16(vreinterpretq_u16_f16(v)));
#  endif
}

#endif  // ISA_CPU_ARM_NEON

// clang-format off

// =============================================================================
// Adopted from Arm Compute Library.
//
// Copyright (c) 2016-2021 Arm Limited.
// SPDX-License-Identifier: MIT
// =============================================================================

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

// clang-format on

#endif  // ISA_CPU_ARM_NEON

}  // namespace radio_core::internal::neon
