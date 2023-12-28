// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element single precision floating point values using
// 2 Float4 scalars.
// Relies on the SIMD optimization of the Float4.

#pragma once

#include "radio_core/base/aligned_register.h"
#include "radio_core/math/float4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo;

template <bool SpecializationMarker>
struct VectorizedFloatTypeInfo<float, 8, SpecializationMarker> {
  using RegisterType = AlignedRegister<Float4, 2, 32>;
  using MaskType = VectorizedIntType<typename BitfieldForType<float>::Type, 8>;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "Float4x2"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const float values[8]) -> RegisterType {
    return {Float4(values), Float4(values + 4)};
  }

  static inline auto Load(const float a,
                          const float b,
                          const float c,
                          const float d,
                          const float e,
                          const float f,
                          const float g,
                          const float h) -> RegisterType {
    return {Float4(a, b, c, d), Float4(e, f, g, h)};
  }

  static inline auto Load(const float& value) -> RegisterType {
    return {Float4(value), Float4(value)};
  }

  static inline auto Load(const Float4::RegisterType& low,
                          const Float4::RegisterType& high) -> RegisterType {
    return {low, high};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const RegisterType& value) -> RegisterType {
    return {-value[0], -value[1]};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const RegisterType& value, const float scalar)
      -> RegisterType {
    return {value[0] * scalar, value[1] * scalar};
  }

  static inline auto Divide(const RegisterType& value, const float scalar)
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

  static inline void Store(const RegisterType& value, float dst[8]) {
    value[0].Store(dst);
    value[1].Store(dst + 4);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, float* dst) {
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
  static inline auto Extract(const RegisterType& value) -> float {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      return value[0].Extract<Index>();
    }

    if constexpr (Index >= 4) {
      return value[1].Extract<Index - 4>();
    }
  }

  static inline auto ExtractLow(const RegisterType& value) -> Float4 {
    return value[0];
  }

  static inline auto ExtractHigh(const RegisterType& value) -> Float4 {
    return value[1];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value,
                             const float new_lane_value) -> RegisterType {
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

  static inline auto SquaredNorm(const RegisterType& value) -> float {
    return HorizontalSum(Multiply(value, value));
  }

  static inline auto Norm(const RegisterType& value) -> float {
    return radio_core::Sqrt(SquaredNorm(value));
  }

  static inline auto Min(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Min(a[0], b[0]), radio_core::Min(a[1], b[1])};
  }

  static inline auto Max(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Max(a[0], b[0]), radio_core::Max(a[1], b[1])};
  }

  static inline auto HorizontalMax(const RegisterType& value) -> float {
    return radio_core::Max(radio_core::HorizontalMax(value[0]),
                           radio_core::HorizontalMax(value[1]));
  }

  static inline auto HorizontalSum(const RegisterType& value) -> float {
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
};

}  // namespace radio_core
