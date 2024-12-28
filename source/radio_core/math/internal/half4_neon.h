// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element single precision floating point values using Neon
// CPU instruction set.

#pragma once

#include "radio_core/base/build_config.h"
#include "radio_core/base/half.h"

#if ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF

#  if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) &&                         \
      __ARM_FEATURE_FP16_VECTOR_ARITHMETIC

#    include "radio_core/math/internal/math_neon.h"
#    include "radio_core/math/ushort4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo;

template <>
struct VectorizedFloatTypeInfo<Half, 4, true> {
  using RegisterType = float16x4_t;
  using MaskType = UShort4;

  static constexpr int kSize = 4;
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

  static inline auto Load(const Half values[4]) -> float16x4_t {
    return vld1_f16(CastPtr(values));
  }

  static inline auto Load(const Half a,
                          const Half b,
                          const Half c,
                          const Half d) -> float16x4_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const Half values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const Half value) -> float16x4_t {
    return vdup_n_f16(_Float16(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const float16x4_t& value) -> float16x4_t {
    return vneg_f16(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const float16x4_t& value, const Half scalar)
      -> float16x4_t {
    return vmul_n_f16(value, _Float16(scalar));
  }

  static inline auto Divide(const float16x4_t& value, const Half scalar)
      -> float16x4_t {
    return vmul_n_f16(value, _Float16(1) / _Float16(scalar));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between 2 vectorized registers.

  static inline auto Add(const float16x4_t& lhs, const float16x4_t& rhs)
      -> float16x4_t {
    return vadd_f16(lhs, rhs);
  }

  static inline auto Subtract(const float16x4_t& lhs, const float16x4_t& rhs)
      -> float16x4_t {
    return vsub_f16(lhs, rhs);
  }

  static inline auto Multiply(const float16x4_t& lhs, const float16x4_t& rhs)
      -> float16x4_t {
    return vmul_f16(lhs, rhs);
  }

  static inline auto Divide(const float16x4_t& lhs, const float16x4_t& rhs)
      -> float16x4_t {
    return vdiv_f16(lhs, rhs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  static inline auto LessThan(const float16x4_t& lhs, const float16x4_t& rhs)
      -> MaskType {
    return MaskType(vclt_f16(lhs, rhs));
  }

  static inline auto GreaterThan(const float16x4_t& lhs, const float16x4_t& rhs)
      -> MaskType {
    return MaskType(vcgt_f16(lhs, rhs));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const float16x4_t& value, Half dst[4]) {
    vst1_f16(CastPtr(dst), value);
  }

  template <int Index>
  static inline void Store(const float16x4_t& value, Half* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    vst1_lane_f16(CastPtr(dst), value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const float16x4_t& value) -> Half {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vget_lane_f16(value, Index);
  }

  static inline auto ExtractLow(const float16x4_t& value) -> Half2 {
    return Half2(Extract<0>(value), Extract<1>(value));
  }

  static inline auto ExtractHigh(const float16x4_t& value) -> Half2 {
    return Half2(Extract<2>(value), Extract<3>(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const float16x4_t& value,
                             const Half new_lane_value) -> float16x4_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vset_lane_f16(_Float16(new_lane_value), value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto FastLog10(const float16x4_t& value) -> float16x4_t {
    // Calculate log10(x) as Log2(x) / Log(10).

    // 1 / Log(10)
    static constexpr float kLog2ToLog10Fac = 0.43429448190325176f;

    const float32x4_t x = vcvt_f32_f16(value);
    const float32x4_t y =
        vmulq_n_f32(internal::neon::vlogq_f32(x), kLog2ToLog10Fac);

    return vcvt_f16_f32(y);
  }

  static inline auto Abs(const float16x4_t& value) -> float16x4_t {
    return vabs_f16(value);
  }

  static inline auto Norm(const float16x4_t& value) -> Half {
    return radio_core::Sqrt(HorizontalSum(Multiply(value, value)));
  }

  static inline auto Min(const float16x4_t& a, const float16x4_t& b)
      -> float16x4_t {
    return vmin_f16(a, b);
  }

  static inline auto Max(const float16x4_t& a, const float16x4_t& b)
      -> float16x4_t {
    return vmax_f16(a, b);
  }

  static inline auto HorizontalMax(const float16x4_t& value) -> Half {
#    if ARCH_CPU_64_BITS
    return vmaxv_f16(value);
#    else
    float16x4_t r = vpmax_f16(value, value);
    r = vpmax_f16(r, r);
    return vget_lane_f16(r, 0);
#    endif
  }

  static inline auto HorizontalSum(const float16x4_t& value) -> Half {
    float16x4_t r = vpadd_f16(value, value);
    r = vpadd_f16(r, r);
    return vget_lane_f16(r, 0);
  }

  static inline auto MultiplyAdd(const float16x4_t& a,
                                 const float16x4_t& b,
                                 const float16x4_t& c) -> float16x4_t {
    return vfma_f16(a, b, c);
  }

  static inline auto Select(const MaskType& mask,
                            const float16x4_t& source1,
                            const float16x4_t& source2) -> float16x4_t {
    return vbsl_f16(mask.GetRegister(), source1, source2);
  }

  static inline auto Sign(const float16x4_t& arg) -> float16x4_t {
    return CopySign(vdup_n_f16(_Float16(1.0f)), arg);
  }

  static inline auto CopySign(const float16x4_t& mag, const float16x4_t& sgn)
      -> float16x4_t {
    const uint16x4_t mask = vdup_n_u16(0x8000);
    return vbsl_f16(mask, sgn, mag);
  }

  static inline auto Reverse(const float16x4_t& value) -> float16x4_t {
    return vrev64_f16(value);
  }

  static inline auto Sin(const float16x4_t& value) -> float16x4_t {
    return internal::neon::vsin_f16(value);
  }

  static inline auto Cos(const float16x4_t& value) -> float16x4_t {
    return internal::neon::vcos_f16(value);
  }

  static inline void SinCos(const float16x4_t& value,
                            float16x4_t& sin,
                            float16x4_t& cos) {
    internal::neon::vsincos_f16(value, &sin, &cos);
  }

  static inline auto Exp(const float16x4_t& value) -> float16x4_t {
    return internal::neon::vexp_f16(value);
  }
};

}  // namespace radio_core

#  endif  //__ARM_FEATURE_FP16_VECTOR_ARITHMETIC
#endif    // ISA_CPU_ARM_NEON && RADIO_CORE_HAVE_HALF
