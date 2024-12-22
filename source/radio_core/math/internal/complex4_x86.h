// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element complex values using SSE2 and above CPU
// instruction set.
// The SSE2 is the minimum system requirement.

#pragma once

#include "radio_core/base/build_config.h"

#if ARCH_CPU_X86_FAMILY

#  include "radio_core/math/complex.h"
#  include "radio_core/math/float4.h"
#  include "radio_core/math/internal/math_x86.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo;

template <>
struct VectorizedComplexTypeInfo<float, 4, true> {
  struct RegisterType {
    __m128 val[2];
  };

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "X86"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const Complex values[4]) -> RegisterType {
    const auto* data = reinterpret_cast<const float*>(values);

    const __m128 a = _mm_loadu_ps(data);
    const __m128 b = _mm_loadu_ps(data + 4);

    RegisterType r;
    r.val[0] = _mm_shuffle_ps(a, b, _MM_SHUFFLE(2, 0, 2, 0));
    r.val[1] = _mm_shuffle_ps(a, b, _MM_SHUFFLE(3, 1, 3, 1));
    return r;
  }

  static inline auto Load(const Complex a,
                          const Complex b,
                          const Complex c,
                          const Complex d) -> RegisterType {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const Complex values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const Complex& value) -> RegisterType {
    return Load(value, value, value, value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const RegisterType& value) -> RegisterType {
    RegisterType r;
    r.val[0] = _mm_xor_ps(value.val[0], _mm_set1_ps(-0.0f));
    r.val[1] = _mm_xor_ps(value.val[1], _mm_set1_ps(-0.0f));
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

  static inline auto Add(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    RegisterType result;
    result.val[0] = _mm_add_ps(lhs.val[0], rhs.val[0]);
    result.val[1] = _mm_add_ps(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Subtract(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    RegisterType result;
    result.val[0] = _mm_sub_ps(lhs.val[0], rhs.val[0]);
    result.val[1] = _mm_sub_ps(lhs.val[1], rhs.val[1]);
    return result;
  }

  static inline auto Multiply(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    const __m128 ac = _mm_mul_ps(lhs.val[0], rhs.val[0]);
    const __m128 bd = _mm_mul_ps(lhs.val[1], rhs.val[1]);
    const __m128 ad = _mm_mul_ps(lhs.val[0], rhs.val[1]);
    const __m128 bc = _mm_mul_ps(lhs.val[1], rhs.val[0]);

    RegisterType result;
    result.val[0] = _mm_sub_ps(ac, bd);
    result.val[1] = _mm_add_ps(ad, bc);
    return result;
  }

  static inline auto Divide(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    const __m128 ac = _mm_mul_ps(lhs.val[0], rhs.val[0]);
    const __m128 bd = _mm_mul_ps(lhs.val[1], rhs.val[1]);
    const __m128 ad = _mm_mul_ps(lhs.val[0], rhs.val[1]);
    const __m128 bc = _mm_mul_ps(lhs.val[1], rhs.val[0]);

    const __m128 c2 = _mm_mul_ps(rhs.val[0], rhs.val[0]);
    const __m128 d2 = _mm_mul_ps(rhs.val[1], rhs.val[1]);
    const __m128 den = _mm_add_ps(c2, d2);
    const __m128 den_inv = _mm_div_ps(_mm_set1_ps(1), den);

    RegisterType result;
    result.val[0] = _mm_mul_ps(_mm_add_ps(ac, bd), den_inv);
    result.val[1] = _mm_mul_ps(_mm_sub_ps(bc, ad), den_inv);
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, Complex dst[4]) {
    auto* data = reinterpret_cast<float*>(dst);

    const __m128 xy = _mm_unpacklo_ps(value.val[0], value.val[1]);
    const __m128 zw = _mm_unpackhi_ps(value.val[0], value.val[1]);

    _mm_storeu_ps(data, xy);
    _mm_storeu_ps(data + 4, zw);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, Complex* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = Extract<Index>(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const RegisterType& value) -> Complex {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index == 0) {
      return {_mm_cvtss_f32(value.val[0]), _mm_cvtss_f32(value.val[1])};
    } else {
      // NOTE: Keep `else` to silence unreachable code warning generated by
      // MSVC.

      const float real = _mm_cvtss_f32(_mm_shuffle_ps(
          value.val[0], value.val[0], _MM_SHUFFLE(0, 0, 0, Index)));
      const float imag = _mm_cvtss_f32(_mm_shuffle_ps(
          value.val[1], value.val[1], _MM_SHUFFLE(0, 0, 0, Index)));

      return {real, imag};
    }
  }

  static inline auto ExtractLow(const RegisterType& value) -> Complex2 {
    return Complex2(Extract<0>(value), Extract<1>(value));
  }

  static inline auto ExtractHigh(const RegisterType& value) -> Complex2 {
    return Complex2(Extract<2>(value), Extract<3>(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value,
                             const Complex new_lane_value) -> RegisterType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

#  if ISA_CPU_X86_SSE4_1
    const int imm8 = Index << 4;
    RegisterType result;
    result.val[0] =
        _mm_insert_ps(value.val[0], _mm_set_ss(new_lane_value.real), imm8);
    result.val[1] =
        _mm_insert_ps(value.val[1], _mm_set_ss(new_lane_value.imag), imm8);
    return result;
#  else
    alignas(16) Complex tmp[4];
    Store(value, tmp);
    tmp[Index] = new_lane_value;
    return Load(tmp);
#  endif
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto NormImpl(const RegisterType& value) -> __m128 {
    // Multiply the real part by real path, then multiply-add square of the
    // imaginary part.
    const __m128 real2 = _mm_mul_ps(value.val[0], value.val[0]);
    return internal::x86::MultiplyAdd(real2, value.val[1], value.val[1]);
  }

  static inline auto Abs(const RegisterType& value) -> Float4 {
#  if ISA_CPU_X86_AVX
    const __m256 magnitude_sq = _mm256_castps128_ps256(NormImpl(value));
    const __m256 magnitude = _mm256_sqrt_ps(magnitude_sq);
    return Float4(_mm256_castps256_ps128(magnitude));
#  else
    const __m128 magnitude_sq = NormImpl(value);
    return Float4(_mm_sqrt_ps(magnitude_sq));
#  endif
  }

  static inline auto FastAbs(const RegisterType& value) -> Float4 {
    const __m128 magnitude_sq = NormImpl(value);
    const __m128 magnitude_inv = _mm_rsqrt_ps(magnitude_sq);
    return Float4(_mm_rcp_ps(magnitude_inv));
  }

  static inline auto Norm(const RegisterType& value) -> Float4 {
    return Float4(NormImpl(value));
  }

  static inline auto HorizontalSum(const RegisterType& value) -> Complex {
    const float real = internal::x86::HorizontalSum(value.val[0]);
    const float imag = internal::x86::HorizontalSum(value.val[1]);
    return {real, imag};
  }

  static inline auto MultiplyAdd(const RegisterType& a,
                                 const RegisterType& b,
                                 const typename Float4::RegisterType& c)
      -> RegisterType {
    RegisterType result;
    result.val[0] = internal::x86::MultiplyAdd(a.val[0], b.val[0], c);
    result.val[1] = internal::x86::MultiplyAdd(a.val[1], b.val[1], c);
    return result;
  }

  static inline auto FastArg(const RegisterType& value) -> Float4 {
    const Float4 x(value.val[0]);
    const Float4 y(value.val[1]);

    return FastArcTan2(y, x);
  }

  static inline auto Conj(const RegisterType& value) -> RegisterType {
    const __m128 sign_mask = _mm_set1_ps(-0.0f);
    RegisterType result;
    result.val[0] = value.val[0];
    result.val[1] = _mm_xor_ps(value.val[1], sign_mask);
    return result;
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    RegisterType result;
    result.val[0] =
        _mm_shuffle_ps(value.val[0], value.val[0], _MM_SHUFFLE(0, 1, 2, 3));
    result.val[1] =
        _mm_shuffle_ps(value.val[1], value.val[1], _MM_SHUFFLE(0, 1, 2, 3));
    return result;
  }
};

}  // namespace radio_core

#endif
