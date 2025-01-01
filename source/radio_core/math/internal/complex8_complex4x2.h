// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element single precision floating point complex values
// using 2 Complex4 scalars. Relies on the SIMD optimization of the Complex4.

#pragma once

#include "radio_core/base/aligned_register.h"
#include "radio_core/math/complex4.h"
#include "radio_core/math/float8.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo;

template <bool SpecializationMarker>
struct VectorizedComplexTypeInfo<float, 8, SpecializationMarker> {
  using RegisterType = AlignedRegister<Complex4, 2, 32>;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "Complex4x2"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const Complex values[8]) -> RegisterType {
    return {Complex4(values), Complex4(values + 4)};
  }

  static inline auto Load(const Complex& a,
                          const Complex& b,
                          const Complex& c,
                          const Complex& d,
                          const Complex& e,
                          const Complex& f,
                          const Complex& g,
                          const Complex& h) -> RegisterType {
    return {Complex4(a, b, c, d), Complex4(e, f, g, h)};
  }

  static inline auto Load(const Complex& value) -> RegisterType {
    return {Complex4(value), Complex4(value)};
  }

  static inline auto Load(const Float8::RegisterType& real,
                          const Float8::RegisterType& imag) -> RegisterType {
    RegisterType result;
    result[0] = Complex4(Float8::TypeInfo::ExtractLow(real),
                         Float8::TypeInfo::ExtractLow(imag));
    result[1] = Complex4(Float8::TypeInfo::ExtractHigh(real),
                         Float8::TypeInfo::ExtractHigh(imag));
    return result;
  }

  static inline auto Load(const float real) -> RegisterType {
    return {Complex4(real), Complex4(real)};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  static inline auto Negate(const RegisterType& value) -> RegisterType {
    return {-value[0], -value[1]};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

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

  static inline auto Multiply(const RegisterType& lhs,
                              const typename Float8::RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] * rhs[0], lhs[1] * rhs[1]};
  }

  static inline auto Divide(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] / rhs[0], lhs[1] / rhs[1]};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, Complex dst[8]) {
    value[0].Store(dst);
    value[1].Store(dst + 4);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, Complex* dst) {
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
  static inline auto Extract(const RegisterType& value) -> Complex {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      return value[0].Extract<Index>();
    }

    if constexpr (Index >= 4) {
      return value[1].Extract<Index - 4>();
    }
  }

  static inline auto ExtractLow(const RegisterType& value) -> Complex4 {
    return value[0];
  }

  static inline auto ExtractHigh(const RegisterType& value) -> Complex4 {
    return value[1];
  }

  static inline auto ExtractReal(const RegisterType& value) -> Float8 {
    const Float4 real_low = value[0].ExtractReal();
    const Float4 real_high = value[1].ExtractReal();
    return Float8(real_low, real_high);
  }

  static inline auto ExtractImag(const RegisterType& value) -> Float8 {
    const Float4 imag_low = value[0].ExtractImag();
    const Float4 imag_high = value[1].ExtractImag();
    return Float8(imag_low, imag_high);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value,
                             const Complex new_lane_value) -> RegisterType {
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

  static inline auto Abs(const RegisterType& value) -> Float8 {
    return {radio_core::Abs(value[0]), radio_core::Abs(value[1])};
  }

  static inline auto FastAbs(const RegisterType& value) -> Float8 {
    return {radio_core::FastAbs(value[0]), radio_core::FastAbs(value[1])};
  }

  static inline auto Norm(const RegisterType& value) -> Float8 {
    return {radio_core::Norm(value[0]), radio_core::Norm(value[1])};
  }

  static inline auto HorizontalSum(const RegisterType& value) -> Complex {
    return radio_core::HorizontalSum(value[0]) +
           radio_core::HorizontalSum(value[1]);
  }

  static inline auto MultiplyAdd(const RegisterType& a,
                                 const RegisterType& b,
                                 const typename Float8::RegisterType& c)
      -> RegisterType {
    return {
        radio_core::MultiplyAdd(a[0], b[0], Float8::TypeInfo::ExtractLow(c)),
        radio_core::MultiplyAdd(a[1], b[1], Float8::TypeInfo::ExtractHigh(c))};
  }

  static inline auto FastArg(const RegisterType& value) -> Float8 {
    return {radio_core::FastArg(value[0]), radio_core::FastArg(value[1])};
  }

  static inline auto Conj(const RegisterType& value) -> RegisterType {
    return {radio_core::Conj(value[0]), radio_core::Conj(value[1])};
  }

  static inline auto ComplexExp(const typename Float8::RegisterType& x)
      -> RegisterType {
    return {radio_core::ComplexExp(Float8::TypeInfo::ExtractLow(x)),
            radio_core::ComplexExp(Float8::TypeInfo::ExtractHigh(x))};
  }

  static inline auto Exp(const RegisterType& z) -> RegisterType {
    return {radio_core::Exp(z[0]), radio_core::Exp(z[1])};
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    return {radio_core::Reverse(value[1]), radio_core::Reverse(value[0])};
  }
};

}  // namespace radio_core
