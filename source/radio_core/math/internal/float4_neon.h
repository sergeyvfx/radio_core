// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element single precision floating point values using Neon
// CPU instruction set.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include "radio_core/math/bitwise.h"
#  include "radio_core/math/internal/math_neon.h"
#  include "radio_core/math/uint4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo;

template <>
struct VectorizedFloatTypeInfo<float, 4, true> {
  using RegisterType = float32x4_t;
  using MaskType = UInt4;

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const float values[4]) -> float32x4_t {
    return vld1q_f32(values);
  }

  static inline auto Load(const float a,
                          const float b,
                          const float c,
                          const float d) -> float32x4_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const float values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const float value) -> float32x4_t {
    return vdupq_n_f32(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const float32x4_t& value) -> float32x4_t {
    return vnegq_f32(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const float32x4_t& value, const float scalar)
      -> float32x4_t {
    return vmulq_n_f32(value, scalar);
  }

  static inline auto Divide(const float32x4_t& value, const float scalar)
      -> float32x4_t {
    return vmulq_n_f32(value, 1.0f / scalar);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between 2 vectorized registers.

  static inline auto Add(const float32x4_t& lhs, const float32x4_t& rhs)
      -> float32x4_t {
    return vaddq_f32(lhs, rhs);
  }

  static inline auto Subtract(const float32x4_t& lhs, const float32x4_t& rhs)
      -> float32x4_t {
    return vsubq_f32(lhs, rhs);
  }

  static inline auto Multiply(const float32x4_t& lhs, const float32x4_t& rhs)
      -> float32x4_t {
    return vmulq_f32(lhs, rhs);
  }

  static inline auto Divide(const float32x4_t& lhs, const float32x4_t& rhs)
      -> float32x4_t {
    return internal::neon::vdivq_f32(lhs, rhs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  static inline auto LessThan(const float32x4_t& lhs, const float32x4_t& rhs)
      -> MaskType {
    return MaskType(vcltq_f32(lhs, rhs));
  }

  static inline auto GreaterThan(const float32x4_t& lhs, const float32x4_t& rhs)
      -> MaskType {
    return MaskType(vcgtq_f32(lhs, rhs));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const float32x4_t& value, float dst[4]) {
    vst1q_f32(dst, value);
  }

  template <int Index>
  static inline void Store(const float32x4_t& value, float* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    vst1q_lane_f32(dst, value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const float32x4_t& value) -> float {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vgetq_lane_f32(value, Index);
  }

  static inline auto ExtractLow(const float32x4_t& value) -> Float2 {
    return Float2(Extract<0>(value), Extract<1>(value));
  }

  static inline auto ExtractHigh(const float32x4_t& value) -> Float2 {
    return Float2(Extract<2>(value), Extract<3>(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const float32x4_t& value,
                             const float new_lane_value) -> float32x4_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vsetq_lane_f32(new_lane_value, value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto FastLog10(const float32x4_t& value) -> float32x4_t {
    // Calculate log10(x) as Log2(x) / Log(10).

    // 1 / Log(10)
    static constexpr float kLog2ToLog10Fac = 0.43429448190325176f;

    return vmulq_n_f32(internal::neon::vlogq_f32(value), kLog2ToLog10Fac);
  }

  static inline auto Abs(const float32x4_t& value) -> float32x4_t {
    return vabsq_f32(value);
  }

  static inline auto SquaredNorm(const float32x4_t& value) -> float {
    return HorizontalSum(Multiply(value, value));
  }

  static inline auto Norm(const float32x4_t& value) -> float {
    return radio_core::Sqrt(SquaredNorm(value));
  }

  static inline auto Min(const float32x4_t& a, const float32x4_t& b)
      -> float32x4_t {
    return vminq_f32(a, b);
  }

  static inline auto Max(const float32x4_t& a, const float32x4_t& b)
      -> float32x4_t {
    return vmaxq_f32(a, b);
  }

  static inline auto HorizontalMax(const float32x4_t& value) -> float {
#  if ARCH_CPU_64_BITS
    return vmaxvq_f32(value);
#  else
    float32x2_t r = vpmax_f32(vget_low_f32(value), vget_high_f32(value));
    r = vpmax_f32(r, r);
    return vget_lane_f32(r, 0);
#  endif
  }

  static inline auto HorizontalSum(const float32x4_t& value) -> float {
    return internal::neon::vaddvq_f32(value);
  }

  static inline auto MultiplyAdd(const float32x4_t& a,
                                 const float32x4_t& b,
                                 const float32x4_t& c) -> float32x4_t {
    return vmlaq_f32(a, b, c);
  }

  static inline auto Select(const MaskType& mask,
                            const float32x4_t& source1,
                            const float32x4_t& source2) -> float32x4_t {
    return vbslq_f32(mask.GetRegister(), source1, source2);
  }

  static inline auto Sign(const float32x4_t& arg) -> float32x4_t {
    return CopySign(vdupq_n_f32(1.0f), arg);
  }

  static inline auto CopySign(const float32x4_t& mag, const float32x4_t& sgn)
      -> float32x4_t {
    const uint32x4_t mask = vdupq_n_u32(0x80000000);
    return vbslq_f32(mask, sgn, mag);
  }

  static inline auto Reverse(const float32x4_t& value) -> float32x4_t {
    return internal::neon::vreveseq_f32(value);
  }
};

}  // namespace radio_core

#endif  // ISA_CPU_ARM_NEON
