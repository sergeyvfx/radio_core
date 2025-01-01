// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element half precision floating point complex values
// values using Neon CPU instruction set.

#pragma once

#include "radio_core/base/build_config.h"
#include "radio_core/base/half.h"

#if ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF

#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC

#    include "radio_core/math/half_complex.h"
#    include "radio_core/math/internal/math_neon.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo;

template <>
struct VectorizedComplexTypeInfo<Half, 8, true> {
  using RegisterType = float16x8x2_t;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Helpers.

  // Cast from memory pointer type to __fp16*.
  static inline auto CastPtr(const HalfComplex* ptr) -> const __fp16* {
    return reinterpret_cast<const __fp16*>(ptr);
  }
  static inline auto CastPtr(HalfComplex* ptr) -> __fp16* {
    return reinterpret_cast<__fp16*>(ptr);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const HalfComplex values[8]) -> float16x8x2_t {
    return vld2q_f16(CastPtr(values));
  }

  static inline auto Load(const HalfComplex& a,
                          const HalfComplex& b,
                          const HalfComplex& c,
                          const HalfComplex& d,
                          const HalfComplex& e,
                          const HalfComplex& f,
                          const HalfComplex& g,
                          const HalfComplex& h) -> float16x8x2_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const HalfComplex values[8] = {a, b, c, d, e, f, g, h};
    return Load(values);
  }

  static inline auto Load(const HalfComplex& value) -> float16x8x2_t {
    float16x8x2_t r;
    r.val[0] = vdupq_n_f16(_Float16(value.real));
    r.val[1] = vdupq_n_f16(_Float16(value.imag));
    return r;
  }

  static inline auto Load(const float16x8_t& real, const float16x8_t& imag)
      -> float16x8x2_t {
    float16x8x2_t r;
    r.val[0] = real;
    r.val[1] = imag;
    return r;
  }

  static inline auto Load(const Half real) -> float16x8x2_t {
    float16x8x2_t r;
    r.val[0] = vdupq_n_f16(_Float16(real));
    r.val[1] = vdupq_n_f16(_Float16(0));
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const float16x8x2_t& value) -> float16x8x2_t {
    float16x8x2_t result;
    result.val[0] = vnegq_f16(value.val[0]);
    result.val[1] = vnegq_f16(value.val[1]);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

  static inline auto Add(const float16x8x2_t& lhs, const float16x8x2_t& rhs)
      -> float16x8x2_t {
    float16x8x2_t result;
    result.val[0] = vaddq_f16(lhs.val[0], rhs.val[0]);
    result.val[1] = vaddq_f16(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Subtract(const float16x8x2_t& lhs,
                              const float16x8x2_t& rhs) -> float16x8x2_t {
    float16x8x2_t result;
    result.val[0] = vsubq_f16(lhs.val[0], rhs.val[0]);
    result.val[1] = vsubq_f16(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Multiply(const float16x8x2_t& lhs,
                              const float16x8x2_t& rhs) -> float16x8x2_t {
    const float16x8_t ac = vmulq_f16(lhs.val[0], rhs.val[0]);
    const float16x8_t bd = vmulq_f16(lhs.val[1], rhs.val[1]);
    const float16x8_t ad = vmulq_f16(lhs.val[0], rhs.val[1]);
    const float16x8_t bc = vmulq_f16(lhs.val[1], rhs.val[0]);

    float16x8x2_t result;
    result.val[0] = vsubq_f16(ac, bd);
    result.val[1] = vaddq_f16(ad, bc);
    return result;
  }

  static inline auto Multiply(const float16x8x2_t& lhs, const float16x8_t& rhs)
      -> float16x8x2_t {
    float16x8x2_t result;
    result.val[0] = vmulq_f16(lhs.val[0], rhs);
    result.val[1] = vmulq_f16(lhs.val[1], rhs);
    return result;
  }

  static inline auto Divide(const float16x8x2_t& lhs, const float16x8x2_t& rhs)
      -> float16x8x2_t {
    const float16x8_t ac = vmulq_f16(lhs.val[0], rhs.val[0]);
    const float16x8_t bd = vmulq_f16(lhs.val[1], rhs.val[1]);
    const float16x8_t ad = vmulq_f16(lhs.val[0], rhs.val[1]);
    const float16x8_t bc = vmulq_f16(lhs.val[1], rhs.val[0]);

    const float16x8_t c2 = vmulq_f16(rhs.val[0], rhs.val[0]);
    const float16x8_t d2 = vmulq_f16(rhs.val[1], rhs.val[1]);
    const float16x8_t den = vaddq_f16(c2, d2);
    const float16x8_t den_inv = internal::neon::vinvertq_f16(den);

    RegisterType result;
    result.val[0] = vmulq_f16(vaddq_f16(ac, bd), den_inv);
    result.val[1] = vmulq_f16(vsubq_f16(bc, ad), den_inv);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const float16x8x2_t& value, HalfComplex dst[4]) {
    vst2q_f16(CastPtr(dst), value);
  }

  template <int Index>
  static inline void Store(const float16x8x2_t& value, HalfComplex* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = HalfComplex(vgetq_lane_f16(value.val[0], Index),
                       vgetq_lane_f16(value.val[1], Index));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const float16x8x2_t& value) -> HalfComplex {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return HalfComplex(vgetq_lane_f16(value.val[0], Index),
                       vgetq_lane_f16(value.val[1], Index));
  }

  static inline auto ExtractLow(const float16x8x2_t& value) -> HalfComplex4 {
    float16x4x2_t r;
    r.val[0] = vget_low_f16(value.val[0]);
    r.val[1] = vget_low_f16(value.val[1]);
    return HalfComplex4(r);
  }

  static inline auto ExtractHigh(const float16x8x2_t& value) -> HalfComplex4 {
    float16x4x2_t r;
    r.val[0] = vget_high_f16(value.val[0]);
    r.val[1] = vget_high_f16(value.val[1]);
    return HalfComplex4(r);
  }

  static inline auto ExtractReal(const float16x8x2_t& value) -> Half8 {
    return Half8(value.val[0]);
  }

  static inline auto ExtractImag(const float16x8x2_t& value) -> Half8 {
    return Half8(value.val[1]);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const float16x8x2_t& value,
                             const HalfComplex new_lane_value)
      -> float16x8x2_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    RegisterType result;
    result.val[0] =
        vsetq_lane_f16(_Float16(new_lane_value.real), value.val[0], Index);
    result.val[1] =
        vsetq_lane_f16(_Float16(new_lane_value.imag), value.val[1], Index);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto NormImpl(const float16x8x2_t& value) -> float16x8_t {
    // Multiply the real part by real path, then multiply-add square of the
    // imaginary part.
    const float16x8_t real2 = vmulq_f16(value.val[0], value.val[0]);
    return vfmaq_f16(real2, value.val[1], value.val[1]);
  }

  static inline auto Abs(const float16x8x2_t& value) -> Half8 {
    const float16x8_t magnitude_sq = NormImpl(value);
    return Half8(internal::neon::vsqrtq_f16(magnitude_sq));
  }

  static inline auto FastAbs(const float16x8x2_t& value) -> Half8 {
    const float16x8_t magnitude_sq = NormImpl(value);
    const float16x8_t magnitude_inv = vrsqrteq_f16(magnitude_sq);
    return Half8(vrecpeq_f16(magnitude_inv));
  }

  static inline auto Norm(const float16x8x2_t& value) -> Half8 {
    return Half8(NormImpl(value));
  }

  static inline auto HorizontalSum(const float16x8x2_t& value) -> HalfComplex {
    float16x8_t i = vpaddq_f16(value.val[0], value.val[0]);
    i = vpaddq_f16(i, i);
    i = vpaddq_f16(i, i);

    float16x8_t q = vpaddq_f16(value.val[1], value.val[1]);
    q = vpaddq_f16(q, q);
    q = vpaddq_f16(q, q);

    return HalfComplex(vgetq_lane_f16(i, 0), vgetq_lane_f16(q, 0));
  }

  static inline auto MultiplyAdd(const float16x8x2_t& a,
                                 const float16x8x2_t& b,
                                 const float16x8_t& c) -> float16x8x2_t {
    float16x8x2_t result;
    result.val[0] = vfmaq_f16(a.val[0], b.val[0], c);
    result.val[1] = vfmaq_f16(a.val[1], b.val[1], c);
    return result;
  }

  static inline auto FastArg(const float16x8x2_t& value) -> Half8 {
    const Half8 x(value.val[0]);
    const Half8 y(value.val[1]);

    return FastArcTan2(y, x);
  }

  static inline auto Conj(const float16x8x2_t& value) -> float16x8x2_t {
    float16x8x2_t r;
    r.val[0] = value.val[0];
    r.val[1] = vnegq_f16(value.val[1]);
    return r;
  }

  static inline auto ComplexExp(const float16x8_t& x) -> float16x8x2_t {
    float16x8x2_t result;
    internal::neon::vsincosq_f16(x, &result.val[1], &result.val[0]);
    return result;
  }

  static inline auto Exp(const float16x8x2_t& z) -> float16x8x2_t {
    const float16x8_t exp_real = internal::neon::vexpq_f16(z.val[0]);
    float16x8x2_t result = ComplexExp(z.val[1]);
    result.val[0] = vmulq_f16(result.val[0], exp_real);
    result.val[1] = vmulq_f16(result.val[1], exp_real);
    return result;
  }

  static inline auto Reverse(const float16x8x2_t& value) -> float16x8x2_t {
    float16x8x2_t r;
    r.val[0] = internal::neon::vreveseq_f16(value.val[0]);
    r.val[1] = internal::neon::vreveseq_f16(value.val[1]);
    return r;
  }
};

}  // namespace radio_core

#  endif  //__ARM_FEATURE_FP16_VECTOR_ARITHMETIC
#endif    // ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF
