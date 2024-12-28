// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element half precision floating point complex values
// using 2 HalfComplex4 scalars. Relies on the SIMD optimization of the
// HalfComplex4.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/base/aligned_register.h"
#  include "radio_core/math/half8.h"
#  include "radio_core/math/half_complex4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo;

template <bool SpecializationMarker>
struct VectorizedComplexTypeInfo<Half, 8, SpecializationMarker> {
  using RegisterType = AlignedRegister<HalfComplex4, 2, 16>;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "HalfComplex4x2"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const HalfComplex values[8]) -> RegisterType {
    return {HalfComplex4(values), HalfComplex4(values + 4)};
  }

  static inline auto Load(const HalfComplex& a,
                          const HalfComplex& b,
                          const HalfComplex& c,
                          const HalfComplex& d,
                          const HalfComplex& e,
                          const HalfComplex& f,
                          const HalfComplex& g,
                          const HalfComplex& h) -> RegisterType {
    return {HalfComplex4(a, b, c, d), HalfComplex4(e, f, g, h)};
  }

  static inline auto Load(const HalfComplex& value) -> RegisterType {
    return {HalfComplex4(value), HalfComplex4(value)};
  }

  static inline auto Load(const typename Half8::RegisterType& real,
                          const typename Half8::RegisterType& imag)
      -> RegisterType {
    RegisterType result;
    result[0] = HalfComplex4(Half8::TypeInfo::ExtractLow(real),
                             Half8::TypeInfo::ExtractLow(imag));
    result[1] = HalfComplex4(Half8::TypeInfo::ExtractHigh(real),
                             Half8::TypeInfo::ExtractHigh(imag));
    return result;
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
                              const typename Half8::RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] * rhs[0], lhs[1] * rhs[1]};
  }

  static inline auto Divide(const RegisterType& lhs, const RegisterType& rhs)
      -> RegisterType {
    return {lhs[0] / rhs[0], lhs[1] / rhs[1]};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, HalfComplex dst[8]) {
    value[0].Store(dst);
    value[1].Store(dst + 4);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, HalfComplex* dst) {
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
  static inline auto Extract(const RegisterType& value) -> HalfComplex {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      return value[0].Extract<Index>();
    }

    if constexpr (Index >= 4) {
      return value[1].Extract<Index - 4>();
    }
  }

  static inline auto ExtractLow(const RegisterType& value) -> HalfComplex4 {
    return value[0];
  }

  static inline auto ExtractHigh(const RegisterType& value) -> HalfComplex4 {
    return value[1];
  }

  static inline auto ExtractReal(const RegisterType& value) -> Half8 {
    const Half4 real_low = value[0].ExtractReal();
    const Half4 real_high = value[1].ExtractReal();
    return Half8(real_low, real_high);
  }

  static inline auto ExtractImag(const RegisterType& value) -> Half8 {
    const Half4 imag_low = value[0].ExtractImag();
    const Half4 imag_high = value[1].ExtractImag();
    return Half8(imag_low, imag_high);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const RegisterType& value,
                             const HalfComplex new_lane_value) -> RegisterType {
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

  static inline auto Abs(const RegisterType& value) -> Half8 {
    return {radio_core::Abs(value[0]), radio_core::Abs(value[1])};
  }

  static inline auto FastAbs(const RegisterType& value) -> Half8 {
    return {radio_core::FastAbs(value[0]), radio_core::FastAbs(value[1])};
  }

  static inline auto Norm(const RegisterType& value) -> Half8 {
    return {radio_core::Norm(value[0]), radio_core::Norm(value[1])};
  }

  static inline auto HorizontalSum(const RegisterType& value) -> HalfComplex {
    return radio_core::HorizontalSum(value[0]) +
           radio_core::HorizontalSum(value[1]);
  }

  static inline auto MultiplyAdd(const RegisterType& a,
                                 const RegisterType& b,
                                 const typename Half8::RegisterType& c)
      -> RegisterType {
    return {
        radio_core::MultiplyAdd(a[0], b[0], Half8::TypeInfo::ExtractLow(c)),
        radio_core::MultiplyAdd(a[1], b[1], Half8::TypeInfo::ExtractHigh(c))};
  }

  static inline auto FastArg(const RegisterType& value) -> Half8 {
    return {radio_core::FastArg(value[0]), radio_core::FastArg(value[1])};
  }

  static inline auto Conj(const RegisterType& value) -> RegisterType {
    return {radio_core::Conj(value[0]), radio_core::Conj(value[1])};
  }

  static inline auto ComplexExp(const typename Half8::RegisterType& x)
      -> RegisterType {
    return {radio_core::ComplexExp(Half8::TypeInfo::ExtractLow(x)),
            radio_core::ComplexExp(Half8::TypeInfo::ExtractHigh(x))};
  }

  static inline auto Exp(const RegisterType& z) -> RegisterType {
    return {radio_core::Exp(z[0]), radio_core::Exp(z[1])};
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    return {radio_core::Reverse(value[1]), radio_core::Reverse(value[0])};
  }
};

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
