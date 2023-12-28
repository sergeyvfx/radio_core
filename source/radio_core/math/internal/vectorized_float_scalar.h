// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of N-element floating point values using naive scalar
// operations.

#pragma once

#include "radio_core/base/algorithm.h"
#include "radio_core/base/aligned_register.h"
#include "radio_core/base/unroll.h"
#include "radio_core/math/bitwise.h"
#include "radio_core/math/internal/vectorized_type.h"
#include "radio_core/math/math.h"
#include "radio_core/math/vectorized_int_type.h"

namespace radio_core {

template <class T, int N>
class VectorizedFloatType;

template <class T, int N, bool SpecializationMarker>
struct VectorizedFloatTypeInfo {
  using RegisterType = AlignedRegister<T, N, 16>;
  using MaskType = VectorizedIntType<typename BitfieldForType<T>::Type, N>;

  static constexpr int kSize = N;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "Scalar"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const T values[N]) -> RegisterType {
    return {values};
  }

  template <class... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
  static inline auto Load(const Args&... args) -> RegisterType {
    return {args...};
  }

  static inline auto Load(const T& value) -> RegisterType { return {value}; }

  template <int HalfN = N / 2, typename = std::enable_if_t<HalfN >= 4>>
  static inline auto Load(
      const typename VectorizedFloatType<T, HalfN>::RegisterType& low,
      const typename VectorizedFloatType<T, HalfN>::RegisterType& high)
      -> RegisterType {
    RegisterType r;
    Unroll<HalfN>([&](const auto i) {
      r[i] = VectorizedFloatType<T, HalfN>::TypeInfo::template Extract<i>(low);
    });
    Unroll<HalfN>([&](const auto i) {
      r[i + HalfN] =
          VectorizedFloatType<T, HalfN>::TypeInfo::template Extract<i>(high);
    });
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = -value[i]; });
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  static inline auto Multiply(const RegisterType& value, const T scalar)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = value[i] * scalar; });
    return r;
  }

  static inline auto Divide(const RegisterType& value, const T scalar)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = value[i] / scalar; });
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between 2 vectorized registers.

  static inline auto Add(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = lhs[i] + rhs[i]; });
    return r;
  }

  static inline auto Subtract(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = lhs[i] - rhs[i]; });
    return r;
  }

  static inline auto Multiply(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = lhs[i] * rhs[i]; });
    return r;
  }

  static inline auto Divide(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = lhs[i] / rhs[i]; });
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  static inline auto LessThan(const RegisterType& lhs, const RegisterType& rhs)
      -> MaskType {
    typename MaskType::ScalarType data[N];
    Unroll<N>([&](const auto i) {
      data[i] = (lhs[i] < rhs[i] ? ~(typename MaskType::ScalarType(0)) : 0);
    });
    return MaskType(data);
  }

  static inline auto GreaterThan(const RegisterType& lhs,
                                 const RegisterType& rhs) -> MaskType {
    typename MaskType::ScalarType data[N];
    Unroll<N>([&](const auto i) {
      data[i] = (lhs[i] > rhs[i] ? ~(typename MaskType::ScalarType(0)) : 0);
    });
    return MaskType(data);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, T dst[N]) {
    Unroll<N>([&](const auto i) { dst[i] = value[i]; });
  }

  template <int Index>
  static inline void Store(const RegisterType& value, T* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = value[Index];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const RegisterType& value) -> T {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return value[Index];
  }

  template <int HalfN = N / 2>
  static inline auto ExtractLow(const RegisterType& value)
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedFloatType<T, HalfN>> {
    return VectorizedFloatType<T, HalfN>(&value[0]);
  }

  template <int HalfN = N / 2>
  static inline auto ExtractHigh(const RegisterType& value)
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedFloatType<T, HalfN>> {
    return VectorizedFloatType<T, HalfN>(&value[HalfN]);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value, const T new_lane_value)
      -> RegisterType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    RegisterType new_value = value;
    new_value[Index] = new_lane_value;

    return new_value;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto FastLog10(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = radio_core::FastLog10(value[i]); });
    return r;
  }

  static inline auto Abs(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = radio_core::Abs(value[i]); });
    return r;
  }

  static inline auto SquaredNorm(const RegisterType& value) -> T {
    T squared_norm{0};
    Unroll<N>([&](const auto i) { squared_norm += value[i] * value[i]; });
    return squared_norm;
  }

  static inline auto Norm(const RegisterType& value) -> T {
    return radio_core::Sqrt(SquaredNorm(value));
  }

  static inline auto Min(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = radio_core::Min(a[i], b[i]); });
    return r;
  }

  static inline auto Max(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = radio_core::Max(a[i], b[i]); });
    return r;
  }

  static inline auto HorizontalMax(const RegisterType& value) -> T {
    T max = value[0];
    Unroll<N - 1>(
        [&](const auto i) { max = radio_core::Max(max, value[i + 1]); });
    return max;
  }

  static inline auto HorizontalSum(const RegisterType& value) -> T {
    T sum{0};
    Unroll<N>([&](const auto i) { sum += value[i]; });
    return sum;
  }

  static inline auto MultiplyAdd(const RegisterType& a,
                                 const RegisterType& b,
                                 const RegisterType& c) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = a[i] + (b[i] * c[i]); });
    return r;
  }

  static inline auto Select(const MaskType& maskN,
                            const RegisterType& source1,
                            const RegisterType& source2) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) {
      r[i] = radio_core::Select(
          maskN.template Extract<i>(), source1[i], source2[i]);
    });
    return r;
  }

  static inline auto Sign(const RegisterType& arg) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = radio_core::Sign(arg[i]); });
    return r;
  }

  static inline auto CopySign(const RegisterType& mag, const RegisterType& sgn)
      -> RegisterType {
    RegisterType r;
    Unroll<N>(
        [&](const auto i) { r[i] = radio_core::CopySign(mag[i], sgn[i]); });
    return r;
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = value[N - i - 1]; });
    return r;
  }
};

}  // namespace radio_core
