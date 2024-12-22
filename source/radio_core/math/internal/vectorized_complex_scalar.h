// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of N-element complex values using naive scalar
// operations.

#pragma once

#include "radio_core/base/aligned_register.h"
#include "radio_core/base/unroll.h"
#include "radio_core/math/base_complex.h"
#include "radio_core/math/vectorized_complex_type.h"
#include "radio_core/math/vectorized_float_type.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo {
  using RegisterType = AlignedRegister<BaseComplex<T>, N, 16>;

  static constexpr int kSize = N;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "Scalar"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const BaseComplex<T> values[N]) -> RegisterType {
    return {values};
  }

  template <class... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
  static inline auto Load(const Args&... args) -> RegisterType {
    return {args...};
  }

  static inline auto Load(const BaseComplex<T>& value) -> RegisterType {
    return {value};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = -value[i]; });
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

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
  // Store.

  static inline void Store(const RegisterType& value, BaseComplex<T> dst[N]) {
    Unroll<N>([&](const auto i) { dst[i] = value[i]; });
  }

  template <int Index>
  static inline void Store(const RegisterType& value, BaseComplex<T>* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = value[Index];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const RegisterType& value) -> BaseComplex<T> {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return value[Index];
  }

  template <int HalfN = N / 2>
  static inline auto ExtractLow(const RegisterType& value)
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedComplexType<T, HalfN>> {
    return VectorizedComplexType<T, HalfN>(&value[0]);
  }

  template <int HalfN = N / 2>
  static inline auto ExtractHigh(const RegisterType& value)
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedComplexType<T, HalfN>> {
    return VectorizedComplexType<T, HalfN>(&value[HalfN]);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value,
                             const BaseComplex<T> new_lane_value)
      -> RegisterType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    RegisterType new_value = value;
    new_value[Index] = new_lane_value;

    return new_value;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto Abs(const RegisterType& value)
      -> VectorizedFloatType<T, N> {
    typename VectorizedFloatType<T, N>::ScalarType data[N];
    Unroll<N>([&](const auto i) { data[i] = radio_core::Abs(value[i]); });
    return VectorizedFloatType<T, N>(data);
  }

  static inline auto FastAbs(const RegisterType& value)
      -> VectorizedFloatType<T, N> {
    return Abs(value);
  }

  static inline auto Norm(const RegisterType& value)
      -> VectorizedFloatType<T, N> {
    typename VectorizedFloatType<T, N>::ScalarType data[N];
    Unroll<N>([&](const auto i) { data[i] = radio_core::Norm(value[i]); });
    return VectorizedFloatType<T, N>(data);
  }

  static inline auto HorizontalSum(const RegisterType& value)
      -> BaseComplex<T> {
    BaseComplex<T> sum{0};
    Unroll<N>([&](const auto i) { sum += value[i]; });
    return sum;
  }

  static inline auto MultiplyAdd(
      const RegisterType& a,
      const RegisterType& b,
      const typename VectorizedFloatType<T, N>::RegisterType& c)
      -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) {
      r[i] =
          a[i] +
          (b[i] * VectorizedFloatType<T, N>::TypeInfo::template Extract<i>(c));
    });
    return r;
  }

  static inline auto FastArg(const RegisterType& value)
      -> VectorizedFloatType<T, N> {
    T x_data[N];
    Unroll<N>([&](const auto i) { x_data[i] = value[i].real; });

    T y_data[N];
    Unroll<N>([&](const auto i) { y_data[i] = value[i].imag; });

    const VectorizedFloatType<T, N> x{x_data};
    const VectorizedFloatType<T, N> y{y_data};

    return FastArcTan2(y, x);
  }

  static inline auto Conj(const RegisterType& value) -> RegisterType {
    RegisterType result;
    Unroll<N>([&](const auto i) { result[i] = radio_core::Conj(value[i]); });
    return result;
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = value[N - i - 1]; });
    return r;
  }
};

}  // namespace radio_core
