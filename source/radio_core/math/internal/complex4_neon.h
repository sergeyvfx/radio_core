// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element single precision floating point complex values
// values using Neon CPU instruction set.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include "radio_core/math/complex.h"
#  include "radio_core/math/float4.h"
#  include "radio_core/math/internal/math_neon.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo;

template <>
struct VectorizedComplexTypeInfo<float, 4, true> {
  using RegisterType = float32x4x2_t;

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const Complex values[4]) -> float32x4x2_t {
    return vld2q_f32(reinterpret_cast<const float*>(values));
  }

  static inline auto Load(const Complex& a,
                          const Complex& b,
                          const Complex& c,
                          const Complex& d) -> float32x4x2_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const Complex values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const Complex& value) -> float32x4x2_t {
    float32x4x2_t r;
    r.val[0] = vdupq_n_f32(value.real);
    r.val[1] = vdupq_n_f32(value.imag);
    return r;
  }

  static inline auto Load(const float32x4_t& real, const float32x4_t& imag)
      -> float32x4x2_t {
    float32x4x2_t r;
    r.val[0] = real;
    r.val[1] = imag;
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const float32x4x2_t& value) -> float32x4x2_t {
    float32x4x2_t result;
    result.val[0] = vnegq_f32(value.val[0]);
    result.val[1] = vnegq_f32(value.val[1]);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

  static inline auto Add(const float32x4x2_t& lhs, const float32x4x2_t& rhs)
      -> float32x4x2_t {
    float32x4x2_t result;
    result.val[0] = vaddq_f32(lhs.val[0], rhs.val[0]);
    result.val[1] = vaddq_f32(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Subtract(const float32x4x2_t& lhs,
                              const float32x4x2_t& rhs) -> float32x4x2_t {
    float32x4x2_t result;
    result.val[0] = vsubq_f32(lhs.val[0], rhs.val[0]);
    result.val[1] = vsubq_f32(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Multiply(const float32x4x2_t& lhs,
                              const float32x4x2_t& rhs) -> float32x4x2_t {
    const float32x4_t ac = vmulq_f32(lhs.val[0], rhs.val[0]);
    const float32x4_t bd = vmulq_f32(lhs.val[1], rhs.val[1]);
    const float32x4_t ad = vmulq_f32(lhs.val[0], rhs.val[1]);
    const float32x4_t bc = vmulq_f32(lhs.val[1], rhs.val[0]);

    float32x4x2_t result;
    result.val[0] = vsubq_f32(ac, bd);
    result.val[1] = vaddq_f32(ad, bc);
    return result;
  }

  static inline auto Multiply(const float32x4x2_t& lhs, const float32x4_t& rhs)
      -> float32x4x2_t {
    float32x4x2_t result;
    result.val[0] = vmulq_f32(lhs.val[0], rhs);
    result.val[1] = vmulq_f32(lhs.val[1], rhs);
    return result;
  }

  static inline auto Divide(const float32x4x2_t& lhs, const float32x4x2_t& rhs)
      -> float32x4x2_t {
    const float32x4_t ac = vmulq_f32(lhs.val[0], rhs.val[0]);
    const float32x4_t bd = vmulq_f32(lhs.val[1], rhs.val[1]);
    const float32x4_t ad = vmulq_f32(lhs.val[0], rhs.val[1]);
    const float32x4_t bc = vmulq_f32(lhs.val[1], rhs.val[0]);

    const float32x4_t c2 = vmulq_f32(rhs.val[0], rhs.val[0]);
    const float32x4_t d2 = vmulq_f32(rhs.val[1], rhs.val[1]);
    const float32x4_t den = vaddq_f32(c2, d2);
    const float32x4_t den_inv = internal::neon::vinvertq_f32(den);

    RegisterType result;
    result.val[0] = vmulq_f32(vaddq_f32(ac, bd), den_inv);
    result.val[1] = vmulq_f32(vsubq_f32(bc, ad), den_inv);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const float32x4x2_t& value, Complex dst[4]) {
    vst2q_f32(reinterpret_cast<float*>(dst), value);
  }

  template <int Index>
  static inline void Store(const float32x4x2_t& value, Complex* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = Complex(vgetq_lane_f32(value.val[0], Index),
                   vgetq_lane_f32(value.val[1], Index));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const float32x4x2_t& value) -> Complex {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return Complex(vgetq_lane_f32(value.val[0], Index),
                   vgetq_lane_f32(value.val[1], Index));
  }

  static inline auto ExtractLow(const float32x4x2_t& value) -> Complex2 {
    return Complex2(Extract<0>(value), Extract<1>(value));
  }

  static inline auto ExtractHigh(const float32x4x2_t& value) -> Complex2 {
    return Complex2(Extract<2>(value), Extract<3>(value));
  }

  static inline auto ExtractReal(const float32x4x2_t& value) -> Float4 {
    return Float4(value.val[0]);
  }

  static inline auto ExtractImag(const float32x4x2_t& value) -> Float4 {
    return Float4(value.val[1]);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const float32x4x2_t& value,
                             const Complex new_lane_value) -> float32x4x2_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    RegisterType result;
    result.val[0] = vsetq_lane_f32(new_lane_value.real, value.val[0], Index);
    result.val[1] = vsetq_lane_f32(new_lane_value.imag, value.val[1], Index);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto NormImpl(const float32x4x2_t& value) -> float32x4_t {
    // Multiply the real part by real path, then multiply-add square of the
    // imaginary part.
    const float32x4_t real2 = vmulq_f32(value.val[0], value.val[0]);
    return vmlaq_f32(real2, value.val[1], value.val[1]);
  }

  static inline auto Abs(const float32x4x2_t& value) -> Float4 {
    // TODO(sergey): This actually seems to not have any advantage over naive
    // scalar implementation when measuring using a real world application.
    // Need to verify performance using a synthetic test.
    const float32x4_t magnitude_sq = NormImpl(value);
    const float32x2_t low =
        internal::neon::vsqrt_f32(vget_low_f32(magnitude_sq));
    const float32x2_t high =
        internal::neon::vsqrt_f32(vget_high_f32(magnitude_sq));
    return Float4(vcombine_f32(low, high));
  }

  static inline auto FastAbs(const float32x4x2_t& value) -> Float4 {
    const float32x4_t magnitude_sq = NormImpl(value);
    const float32x4_t magnitude_inv = vrsqrteq_f32(magnitude_sq);
    return Float4(vrecpeq_f32(magnitude_inv));
  }

  static inline auto Norm(const float32x4x2_t& value) -> Float4 {
    return Float4(NormImpl(value));
  }

  static inline auto HorizontalSum(const float32x4x2_t& value) -> Complex {
    return Complex(internal::neon::vaddvq_f32(value.val[0]),
                   internal::neon::vaddvq_f32(value.val[1]));
  }

  static inline auto MultiplyAdd(const float32x4x2_t& a,
                                 const float32x4x2_t& b,
                                 const float32x4_t& c) -> float32x4x2_t {
    float32x4x2_t result;
    result.val[0] = vmlaq_f32(a.val[0], b.val[0], c);
    result.val[1] = vmlaq_f32(a.val[1], b.val[1], c);
    return result;
  }

  static inline auto FastArg(const float32x4x2_t& value) -> Float4 {
    const Float4 x(value.val[0]);
    const Float4 y(value.val[1]);

    return FastArcTan2(y, x);
  }

  static inline auto Conj(const float32x4x2_t& value) -> float32x4x2_t {
    float32x4x2_t result;
    result.val[0] = value.val[0];
    result.val[1] = vnegq_f32(value.val[1]);
    return result;
  }

  static inline auto ComplexExp(const float32x4_t& x) -> float32x4x2_t {
    float32x4x2_t result;
    internal::neon::vsincosq_f32(x, &result.val[1], &result.val[0]);
    return result;
  }

  static inline auto Exp(const float32x4x2_t& z) -> float32x4x2_t {
    const float32x4_t exp_real = internal::neon::vexpq_f32(z.val[0]);
    float32x4x2_t result = ComplexExp(z.val[1]);
    result.val[0] = vmulq_f32(result.val[0], exp_real);
    result.val[1] = vmulq_f32(result.val[1], exp_real);
    return result;
  }

  static inline auto Reverse(const float32x4x2_t& value) -> float32x4x2_t {
    float32x4x2_t r;
    r.val[0] = internal::neon::vreveseq_f32(value.val[0]);
    r.val[1] = internal::neon::vreveseq_f32(value.val[1]);
    return r;
  }
};

}  // namespace radio_core

#endif  // ISA_CPU_ARM_NEON
