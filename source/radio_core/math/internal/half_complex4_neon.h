// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element half precision floating point complex values
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
struct VectorizedComplexTypeInfo<Half, 4, true> {
  using RegisterType = float16x4x2_t;

  static constexpr int kSize = 4;
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

  static inline auto Load(const HalfComplex values[4]) -> float16x4x2_t {
    return vld2_f16(CastPtr(values));
  }

  static inline auto Load(const HalfComplex& a,
                          const HalfComplex& b,
                          const HalfComplex& c,
                          const HalfComplex& d) -> float16x4x2_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const HalfComplex values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const HalfComplex& value) -> float16x4x2_t {
    float16x4x2_t r;
    r.val[0] = vdup_n_f16(_Float16(value.real));
    r.val[1] = vdup_n_f16(_Float16(value.imag));
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const float16x4x2_t& value) -> float16x4x2_t {
    float16x4x2_t result;
    result.val[0] = vneg_f16(value.val[0]);
    result.val[1] = vneg_f16(value.val[1]);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

  static inline auto Add(const float16x4x2_t& lhs, const float16x4x2_t& rhs)
      -> float16x4x2_t {
    float16x4x2_t result;
    result.val[0] = vadd_f16(lhs.val[0], rhs.val[0]);
    result.val[1] = vadd_f16(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Subtract(const float16x4x2_t& lhs,
                              const float16x4x2_t& rhs) -> float16x4x2_t {
    float16x4x2_t result;
    result.val[0] = vsub_f16(lhs.val[0], rhs.val[0]);
    result.val[1] = vsub_f16(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Multiply(const float16x4x2_t& lhs,
                              const float16x4x2_t& rhs) -> float16x4x2_t {
    const float16x4_t ac = vmul_f16(lhs.val[0], rhs.val[0]);
    const float16x4_t bd = vmul_f16(lhs.val[1], rhs.val[1]);
    const float16x4_t ad = vmul_f16(lhs.val[0], rhs.val[1]);
    const float16x4_t bc = vmul_f16(lhs.val[1], rhs.val[0]);

    float16x4x2_t result;
    result.val[0] = vsub_f16(ac, bd);
    result.val[1] = vadd_f16(ad, bc);
    return result;
  }

  static inline auto Divide(const float16x4x2_t& lhs, const float16x4x2_t& rhs)
      -> float16x4x2_t {
    const float16x4_t ac = vmul_f16(lhs.val[0], rhs.val[0]);
    const float16x4_t bd = vmul_f16(lhs.val[1], rhs.val[1]);
    const float16x4_t ad = vmul_f16(lhs.val[0], rhs.val[1]);
    const float16x4_t bc = vmul_f16(lhs.val[1], rhs.val[0]);

    const float16x4_t c2 = vmul_f16(rhs.val[0], rhs.val[0]);
    const float16x4_t d2 = vmul_f16(rhs.val[1], rhs.val[1]);
    const float16x4_t den = vadd_f16(c2, d2);
    const float16x4_t den_inv = internal::neon::vinvert_f16(den);

    RegisterType result;
    result.val[0] = vmul_f16(vadd_f16(ac, bd), den_inv);
    result.val[1] = vmul_f16(vsub_f16(bc, ad), den_inv);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const float16x4x2_t& value, HalfComplex dst[4]) {
    vst2_f16(CastPtr(dst), value);
  }

  template <int Index>
  static inline void Store(const float16x4x2_t& value, HalfComplex* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = HalfComplex(vget_lane_f16(value.val[0], Index),
                       vget_lane_f16(value.val[1], Index));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const float16x4x2_t& value) -> HalfComplex {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return HalfComplex(vget_lane_f16(value.val[0], Index),
                       vget_lane_f16(value.val[1], Index));
  }

  static inline auto ExtractLow(const float16x4x2_t& value) -> HalfComplex2 {
    return HalfComplex2(Extract<0>(value), Extract<1>(value));
  }

  static inline auto ExtractHigh(const float16x4x2_t& value) -> HalfComplex2 {
    return HalfComplex2(Extract<2>(value), Extract<3>(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto NormImpl(const float16x4x2_t& value) -> float16x4_t {
    // Multiply the real part by real path, then multiply-add square of the
    // imaginary part.
    const float16x4_t real2 = vmul_f16(value.val[0], value.val[0]);
    return vfma_f16(real2, value.val[1], value.val[1]);
  }

  static inline auto Abs(const float16x4x2_t& value) -> Half4 {
    const float16x4_t magnitude_sq = NormImpl(value);
    return Half4(internal::neon::vsqrt_f16(magnitude_sq));
  }

  static inline auto FastAbs(const float16x4x2_t& value) -> Half4 {
    const float16x4_t magnitude_sq = NormImpl(value);
    const float16x4_t magnitude_inv = vrsqrte_f16(magnitude_sq);
    return Half4(vrecpe_f16(magnitude_inv));
  }

  static inline auto Norm(const float16x4x2_t& value) -> Half4 {
    return Half4(NormImpl(value));
  }

  static inline auto HorizontalSum(const float16x4x2_t& value) -> HalfComplex {
    float16x4_t i = vpadd_f16(value.val[0], value.val[0]);
    i = vpadd_f16(i, i);

    float16x4_t q = vpadd_f16(value.val[1], value.val[1]);
    q = vpadd_f16(q, q);

    return HalfComplex(vget_lane_f16(i, 0), vget_lane_f16(q, 0));
  }

  static inline auto MultiplyAdd(const float16x4x2_t& a,
                                 const float16x4x2_t& b,
                                 const float16x4_t& c) -> float16x4x2_t {
    float16x4x2_t result;
    result.val[0] = vfma_f16(a.val[0], b.val[0], c);
    result.val[1] = vfma_f16(a.val[1], b.val[1], c);
    return result;
  }

  static inline auto FastArg(const float16x4x2_t& value) -> Half4 {
    const Half4 x(value.val[0]);
    const Half4 y(value.val[1]);

    return FastArcTan2(y, x);
  }

  static inline auto Reverse(const float16x4x2_t& value) -> float16x4x2_t {
    float16x4x2_t r;
    r.val[0] = vrev64_f16(value.val[0]);
    r.val[1] = vrev64_f16(value.val[1]);
    return r;
  }
};

}  // namespace radio_core

#  endif  //__ARM_FEATURE_FP16_VECTOR_ARITHMETIC
#endif    // ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF
