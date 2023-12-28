// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element 16 half precision floating point values using
// Neon CPU instruction set.

#pragma once

#include "radio_core/base/build_config.h"
#include "radio_core/base/half.h"

#if ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF

#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC

#    include "radio_core/math/internal/math_neon.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo;

template <>
struct VectorizedFloatTypeInfo<Half, 8, true> {
  using RegisterType = float16x8_t;
  using MaskType = UShort8;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Helpers.

  // Cast from memory pointer type to __fp16*.
  static inline auto CastPtr(const Half* ptr) -> const __fp16* {
    return reinterpret_cast<const __fp16*>(ptr);
  }
  static inline auto CastPtr(Half* ptr) -> __fp16* {
    return reinterpret_cast<__fp16*>(ptr);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const Half values[8]) -> float16x8_t {
    return vld1q_f16(CastPtr(values));
  }

  static inline auto Load(const Half a,
                          const Half b,
                          const Half c,
                          const Half d,
                          const Half e,
                          const Half f,
                          const Half g,
                          const Half h) -> float16x8_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const Half values[8] = {a, b, c, d, e, f, g, h};
    return Load(values);
  }

  static inline auto Load(const Half value) -> float16x8_t {
    return vdupq_n_f16(_Float16(value));
  }

  static inline auto Load(const Half4::RegisterType& low,
                          const Half4::RegisterType& high) -> RegisterType {
    return vcombine_f16(low, high);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const float16x8_t& value) -> float16x8_t {
    return vnegq_f16(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const float16x8_t& value, const Half scalar)
      -> float16x8_t {
    return vmulq_n_f16(value, _Float16(scalar));
  }

  static inline auto Divide(const float16x8_t& value, const Half scalar)
      -> float16x8_t {
    return vmulq_n_f16(value, _Float16(1) / _Float16(scalar));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between 2 vectorized registers.

  static inline auto Add(const float16x8_t& lhs, const float16x8_t& rhs)
      -> float16x8_t {
    return vaddq_f16(lhs, rhs);
  }

  static inline auto Subtract(const float16x8_t& lhs, const float16x8_t& rhs)
      -> float16x8_t {
    return vsubq_f16(lhs, rhs);
  }

  static inline auto Multiply(const float16x8_t& lhs, const float16x8_t& rhs)
      -> float16x8_t {
    return vmulq_f16(lhs, rhs);
  }

  static inline auto Divide(const float16x8_t& lhs, const float16x8_t& rhs)
      -> float16x8_t {
    return vdivq_f16(lhs, rhs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  static inline auto LessThan(const float16x8_t& lhs, const float16x8_t& rhs)
      -> MaskType {
    return MaskType(vcltq_f16(lhs, rhs));
  }

  static inline auto GreaterThan(const float16x8_t& lhs, const float16x8_t& rhs)
      -> MaskType {
    return MaskType(vcgtq_f16(lhs, rhs));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const float16x8_t& value, Half dst[8]) {
    vst1q_f16(CastPtr(dst), value);
  }

  template <int Index>
  static inline void Store(const float16x8_t& value, Half* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    vst1q_lane_f16(CastPtr(dst), value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const float16x8_t& value) -> Half {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vgetq_lane_f16(value, Index);
  }

  static inline auto ExtractLow(const RegisterType& value) -> Half4 {
    return Half4(vget_low_f16(value));
  }

  static inline auto ExtractHigh(const RegisterType& value) -> Half4 {
    return Half4(vget_high_f16(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const float16x8_t& value,
                             const Half new_lane_value) -> float16x8_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vsetq_lane_f16(_Float16(new_lane_value), value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto FastLog10(const float16x8_t& value) -> float16x8_t {
    // Calculate log10(x) as Log2(x) / Log(10).

    // 1 / Log(10)
    static constexpr float kLog2ToLog10Fac = 0.43429448190325176f;

    return vmulq_n_f16(internal::neon::vlogq_f16(value), kLog2ToLog10Fac);
  }

  static inline auto Abs(const float16x8_t& value) -> float16x8_t {
    return vabsq_f16(value);
  }

  static inline auto Norm(const float16x8_t& value) -> Half {
    return radio_core::Sqrt(HorizontalSum(Multiply(value, value)));
  }

  static inline auto Min(const float16x8_t& a, const float16x8_t& b)
      -> float16x8_t {
    return vminq_f16(a, b);
  }

  static inline auto Max(const float16x8_t& a, const float16x8_t& b)
      -> float16x8_t {
    return vmaxq_f16(a, b);
  }

  static inline auto HorizontalMax(const float16x8_t& value) -> Half {
#    if ARCH_CPU_64_BITS
    return vmaxvq_f16(value);
#    else
    float16x4_t r = vpmax_f16(vget_low_f16(value), vget_high_f16(value));
    r = vpmax_f16(r, r);
    r = vpmax_f16(r, r);
    return vget_lane_f16(r, 0);
#    endif
  }

  static inline auto HorizontalSum(const float16x8_t& value) -> Half {
    float16x4_t r = vpadd_f16(vget_low_f16(value), vget_high_f16(value));
    r = vpadd_f16(r, r);
    r = vpadd_f16(r, r);
    return vget_lane_f16(r, 0);
  }

  static inline auto MultiplyAdd(const float16x8_t& a,
                                 const float16x8_t& b,
                                 const float16x8_t& c) -> float16x8_t {
    return vfmaq_f16(a, b, c);
  }

  static inline auto Select(const MaskType& mask,
                            const float16x8_t& source1,
                            const float16x8_t& source2) -> float16x8_t {
    return vbslq_f16(mask.GetRegister(), source1, source2);
  }

  static inline auto Sign(const float16x8_t& arg) -> float16x8_t {
    return CopySign(vdupq_n_f16(_Float16(1.0f)), arg);
  }

  static inline auto CopySign(const float16x8_t& mag, const float16x8_t& sgn)
      -> float16x8_t {
    const uint16x8_t mask = vdupq_n_u16(0x8000);
    return vbslq_f16(mask, sgn, mag);
  }

  static inline auto Reverse(const float16x8_t& value) -> float16x8_t {
    return internal::neon::vreveseq_f16(value);
  }
};

}  // namespace radio_core

#  endif  //__ARM_FEATURE_FP16_VECTOR_ARITHMETIC
#endif    // ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF
