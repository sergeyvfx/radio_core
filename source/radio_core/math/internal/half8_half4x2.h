// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element half precision floating point values using
// 2 Half4 scalars.
// Relies on the SIMD optimization of the Half4.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/base/aligned_register.h"
#  include "radio_core/math/half4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo;

template <bool SpecializationMarker>
struct VectorizedFloatTypeInfo<Half, 8, SpecializationMarker> {
  using RegisterType = AlignedRegister<Half4, 2, 16>;
  using MaskType = VectorizedIntType<typename BitfieldForType<Half>::Type, 8>;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "Half4x2"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const Half values[8]) -> RegisterType {
    return {Half4(values), Half4(values + 4)};
  }

  static inline auto Load(const Half a,
                          const Half b,
                          const Half c,
                          const Half d,
                          const Half e,
                          const Half f,
                          const Half g,
                          const Half h) -> RegisterType {
    return {Half4(a, b, c, d), Half4(e, f, g, h)};
  }

  static inline auto Load(const Half value) -> RegisterType {
    return {Half4(value), Half4(value)};
  }

  static inline auto Load(const Half4::RegisterType& low,
                          const Half4::RegisterType& high) -> RegisterType {
    return {low, high};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const RegisterType& value) -> RegisterType {
    return {-value[0], -value[1]};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const RegisterType& value, const Half scalar)
      -> RegisterType {
    return {value[0] * scalar, value[1] * scalar};
  }

  static inline auto Divide(const RegisterType& value, const Half scalar)
      -> RegisterType {
    return {value[0] / scalar, value[1] / scalar};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between 2 vectorized registers.

  static inline auto Add(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] + rhs[0], lhs[1] + rhs[1]};
  }

  static inline auto Subtract(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] - rhs[0], lhs[1] - rhs[1]};
  }

  static inline auto Multiply(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] * rhs[0], lhs[1] * rhs[1]};
  }

  static inline auto Divide(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] / rhs[0], lhs[1] / rhs[1]};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  static inline auto LessThan(const RegisterType& lhs, const RegisterType& rhs)
      -> MaskType {
    return {(lhs[0] < rhs[0]), (lhs[1] < rhs[1])};
  }

  static inline auto GreaterThan(const RegisterType& lhs,
                                 const RegisterType& rhs) -> MaskType {
    return {(lhs[0] > rhs[0]), (lhs[1] > rhs[1])};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, Half dst[8]) {
    value[0].Store(dst);
    value[1].Store(dst + 4);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, Half* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      value[0].Store<Index>(dst);
      return;
    }

    if constexpr (Index >= 4) {
      value[1].Store<Index - 4>(dst);
      return;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const RegisterType& value) -> Half {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      return value[0].Extract<Index>();
    }

    if constexpr (Index >= 4) {
      return value[1].Extract<Index - 4>();
    }
  }

  static inline auto ExtractLow(const RegisterType& value) -> Half4 {
    return value[0];
  }

  static inline auto ExtractHigh(const RegisterType& value) -> Half4 {
    return value[1];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value,
                             const Half new_lane_value) -> RegisterType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      return {value[0].SetLane<Index>(new_lane_value), value[1]};
    }

    if constexpr (Index >= 4) {
      return {value[0], value[1].SetLane<Index - 4>(new_lane_value)};
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto FastLog10(const RegisterType& value) -> RegisterType {
    return {radio_core::FastLog10(value[0]), radio_core::FastLog10(value[1])};
  }

  static inline auto Abs(const RegisterType& value) -> RegisterType {
    return {radio_core::Abs(value[0]), radio_core::Abs(value[1])};
  }

  static inline auto Norm(const RegisterType& value) -> Half {
    return radio_core::Sqrt(HorizontalSum(Multiply(value, value)));
  }

  static inline auto Min(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Min(a[0], b[0]), radio_core::Min(a[1], b[1])};
  }

  static inline auto Max(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Max(a[0], b[0]), radio_core::Max(a[1], b[1])};
  }

  static inline auto HorizontalMax(const RegisterType& value) -> Half {
    return radio_core::Max(radio_core::HorizontalMax(value[0]),
                           radio_core::HorizontalMax(value[1]));
  }

  static inline auto HorizontalSum(const RegisterType& value) -> Half {
    return radio_core::HorizontalSum(value[0]) +
           radio_core::HorizontalSum(value[1]);
  }

  static inline auto MultiplyAdd(const RegisterType& a,
                                 const RegisterType& b,
                                 const RegisterType& c) -> RegisterType {
    return {radio_core::MultiplyAdd(a[0], b[0], c[0]),
            radio_core::MultiplyAdd(a[1], b[1], c[1])};
  }

  static inline auto Select(const MaskType& mask,
                            const RegisterType& source1,
                            const RegisterType& source2) -> RegisterType {
    return {radio_core::Select(mask.ExtractLow(), source1[0], source2[0]),
            radio_core::Select(mask.ExtractHigh(), source1[1], source2[1])};
  }

  static inline auto Sign(const RegisterType& arg) -> RegisterType {
    return {radio_core::Sign(arg[0]), radio_core::Sign(arg[1])};
  }

  static inline auto CopySign(const RegisterType& mag, const RegisterType& sgn)
      -> RegisterType {
    return {radio_core::CopySign(mag[0], sgn[0]),
            radio_core::CopySign(mag[1], sgn[1])};
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    return {radio_core::Reverse(value[1]), radio_core::Reverse(value[0])};
  }

  static inline auto Sin(const RegisterType& arg) -> RegisterType {
    return {radio_core::Sin(arg[0]), radio_core::Sin(arg[1])};
  }

  static inline auto Cos(const RegisterType& arg) -> RegisterType {
    return {radio_core::Cos(arg[0]), radio_core::Cos(arg[1])};
  }

  static inline void SinCos(const RegisterType& arg,
                            RegisterType& sin,
                            RegisterType& cos) {
    Half4 sin_low, cos_low;
    Half4 sin_high, cos_high;
    radio_core::SinCos(arg[0], sin_low, cos_low);
    radio_core::SinCos(arg[1], sin_high, cos_high);
    sin = RegisterType(sin_low, sin_high);
    cos = RegisterType(cos_low, cos_high);
  }

  static inline auto Exp(const RegisterType& arg) -> RegisterType {
    return {radio_core::Exp(arg[0]), radio_core::Exp(arg[1])};
  }
};

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
