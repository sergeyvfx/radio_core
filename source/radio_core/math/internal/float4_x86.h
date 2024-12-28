// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element single precision floating point values using
// SSE2 and above CPU instruction set.
// The SSE2 is the minimum system requirement.

#pragma once

#include "radio_core/base/build_config.h"

#if ARCH_CPU_X86_FAMILY

#  include "radio_core/math/bitwise.h"
#  include "radio_core/math/internal/math_x86.h"
#  include "radio_core/math/uint4.h"

namespace radio_core {

static_assert(ISA_CPU_X86_SSE2, "SSE 2 is the required minimum");

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo;

template <>
struct VectorizedFloatTypeInfo<float, 4, true> {
  using RegisterType = __m128;
  using MaskType = VectorizedIntType<typename BitfieldForType<float>::Type, 4>;

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "X86"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const float values[4]) -> __m128 {
    return _mm_loadu_ps(values);
  }

  static inline auto Load(const float a,
                          const float b,
                          const float c,
                          const float d) -> __m128 {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const float values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const float value) -> __m128 {
#  if ISA_CPU_X86_AVX2
    return _mm_broadcast_ss(&value);
#  else
    return _mm_set1_ps(value);
#  endif
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const __m128& value) -> __m128 {
    return _mm_xor_ps(value, _mm_set1_ps(-0.0f));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const __m128& value, const float scalar)
      -> __m128 {
    return _mm_mul_ps(value, Load(scalar));
  }

  static inline auto Divide(const __m128& value, const float scalar) -> __m128 {
    return _mm_div_ps(value, Load(scalar));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between 2 vectorized registers.

  static inline auto Add(const __m128& lhs, const __m128& rhs) -> __m128 {
    return _mm_add_ps(lhs, rhs);
  }

  static inline auto Subtract(const __m128& lhs, const __m128& rhs) -> __m128 {
    return _mm_sub_ps(lhs, rhs);
  }

  static inline auto Multiply(const __m128& lhs, const __m128& rhs) -> __m128 {
    return _mm_mul_ps(lhs, rhs);
  }

  static inline auto Divide(const __m128& lhs, const __m128& rhs) -> __m128 {
    return _mm_div_ps(lhs, rhs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  static inline auto LessThan(const __m128& lhs, const __m128& rhs)
      -> MaskType {
    return MaskType(_mm_castps_si128(_mm_cmplt_ps(lhs, rhs)));
  }

  static inline auto GreaterThan(const __m128& lhs, const __m128& rhs)
      -> MaskType {
    return MaskType(_mm_castps_si128(_mm_cmpgt_ps(lhs, rhs)));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const __m128& value, float dst[4]) {
    _mm_storeu_ps(dst, value);
  }

  template <int Index>
  static inline void Store(const __m128& value, float* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = Extract<Index>(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const __m128& value) -> float {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index == 0) {
      return _mm_cvtss_f32(value);
    }

    return _mm_cvtss_f32(
        _mm_shuffle_ps(value, value, _MM_SHUFFLE(0, 0, 0, Index)));
  }

  static inline auto ExtractLow(const __m128& value) -> Float2 {
    return Float2(Extract<0>(value), Extract<1>(value));
  }

  static inline auto ExtractHigh(const __m128& value) -> Float2 {
    return Float2(Extract<2>(value), Extract<3>(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const __m128& value, const float new_lane_value)
      -> __m128 {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

#  if ISA_CPU_X86_SSE4_1
    return _mm_insert_ps(value, _mm_set_ss(new_lane_value), Index << 4);
#  else
    alignas(16) float tmp[4];
    Store(value, tmp);
    tmp[Index] = new_lane_value;
    return Load(tmp);
#  endif
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto FastLog10(const __m128& value) -> __m128 {
    // Calculate Log10(x) as Log2(x) / Ln(10).

    // 1 / Log2(10)
    static constexpr float kLn2ToLog10Fac = 0.3010299956639812f;

    return _mm_mul_ps(internal::x86::ApproximateLog2(value),
                      _mm_set1_ps(kLn2ToLog10Fac));
  }

  static inline auto Abs(const __m128& value) -> __m128 {
    return _mm_and_ps(value, _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
  }

  static inline auto SquaredNorm(const __m128& value) -> float {
    return HorizontalSum(Multiply(value, value));
  }

  static inline auto Norm(const __m128& value) -> float {
    return radio_core::Sqrt(SquaredNorm(value));
  }

  static inline auto Min(const __m128& a, const __m128& b) -> __m128 {
    return _mm_min_ps(a, b);
  }

  static inline auto Max(const __m128& a, const __m128& b) -> __m128 {
    return _mm_max_ps(a, b);
  }

  static inline auto HorizontalMax(const __m128& value) -> float {
    const __m128 max1 = _mm_shuffle_ps(value, value, _MM_SHUFFLE(0, 0, 3, 2));
    const __m128 max2 = _mm_max_ps(value, max1);
    const __m128 max3 = _mm_shuffle_ps(max2, max2, _MM_SHUFFLE(0, 0, 0, 1));
    return _mm_cvtss_f32(_mm_max_ps(max2, max3));
  }

  static inline auto HorizontalSum(const __m128& value) -> float {
    return internal::x86::HorizontalSum(value);
  }

  static inline auto MultiplyAdd(const __m128& a,
                                 const __m128& b,
                                 const __m128& c) -> __m128 {
    return internal::x86::MultiplyAdd(a, b, c);
  }

  static inline auto Select(const MaskType& mask,
                            const __m128& source1,
                            const __m128& source2) -> __m128 {
    const __m128 mask_m128 =
        _mm_loadu_ps(reinterpret_cast<const float*>(&mask));
    const __m128 bits_from_source1 = _mm_and_ps(mask_m128, source1);
    const __m128 bits_from_source2 = _mm_andnot_ps(mask_m128, source2);
    return _mm_or_ps(bits_from_source1, bits_from_source2);
  }

  static inline auto Sign(const __m128& arg) -> __m128 {
    return CopySign(Load(1.0f), arg);
  }

  static inline auto CopySign(const __m128& mag, const __m128& sgn) -> __m128 {
    const __m128 signbit = _mm_set1_ps(-0.0f);
    return _mm_or_ps(_mm_and_ps(signbit, sgn), _mm_andnot_ps(signbit, mag));
  }

  static inline auto Reverse(const __m128& value) -> __m128 {
    return _mm_shuffle_ps(value, value, _MM_SHUFFLE(0, 1, 2, 3));
  }

  static inline auto Sin(const __m128& arg) -> __m128 {
    return internal::x86::sin_ps(arg);
  }

  static inline auto Cos(const __m128& arg) -> __m128 {
    return internal::x86::cos_ps(arg);
  }

  static inline void SinCos(const __m128& arg, __m128& sin, __m128& cos) {
    return internal::x86::sincos_ps(arg, &sin, &cos);
  }

  static inline auto Exp(const __m128& arg) -> __m128 {
    return internal::x86::exp_ps(arg);
  }
};

}  // namespace radio_core

#endif
