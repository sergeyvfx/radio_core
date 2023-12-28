// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element 16 bit integer values using 2 UShort4 scalars.
// Relies on the SIMD optimization of the UShort4.

#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>

#include "radio_core/base/aligned_register.h"
#include "radio_core/math/ushort4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo;

template <bool SpecializationMarker>
struct VectorizedIntTypeInfo<uint16_t, 8, SpecializationMarker> {
  using RegisterType = AlignedRegister<UShort4, 2, 16>;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "UShort4x2"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const uint16_t values[8]) -> RegisterType {
    return {UShort4(values), UShort4(values + 4)};
  }

  static inline auto Load(const uint16_t a,
                          const uint16_t b,
                          const uint16_t c,
                          const uint16_t d,
                          const uint16_t e,
                          const uint16_t f,
                          const uint16_t g,
                          const uint16_t h) -> RegisterType {
    return {UShort4(a, b, c, d), UShort4(e, f, g, h)};
  }

  static inline auto Load(const uint16_t value) -> RegisterType {
    return {value};
  }

  static inline auto Load(
      const typename VectorizedIntType<uint16_t, 4>::RegisterType& low,
      const typename VectorizedIntType<uint16_t, 4>::RegisterType& high)
      -> RegisterType {
    return {low, high};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, uint16_t dst[8]) {
    value[0].Store(dst);
    value[1].Store(dst + 4);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, uint16_t* dst) {
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
  static inline auto Extract(const RegisterType& value) -> uint16_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    if constexpr (Index < 4) {
      return value[0].Extract<Index>();
    }

    if constexpr (Index >= 4) {
      return value[1].Extract<Index - 4>();
    }
  }

  static inline auto ExtractLow(const RegisterType& value)
      -> VectorizedIntType<uint16_t, 4> {
    return value[0];
  }

  static inline auto ExtractHigh(const RegisterType& value)
      -> VectorizedIntType<uint16_t, 4> {
    return value[1];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto Min(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Min(a[0], b[0]), radio_core::Min(a[1], b[1])};
  }

  static inline auto Max(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Max(a[0], b[0]), radio_core::Max(a[1], b[1])};
  }

  static inline auto HorizontalMax(const RegisterType& value) -> uint16_t {
    return radio_core::Max(radio_core::HorizontalMax(value[0]),
                           radio_core::HorizontalMax(value[1]));
  }

  static inline auto Select(const RegisterType& mask,
                            const RegisterType& source1,
                            const RegisterType& source2) -> RegisterType {
    return {radio_core::Select(ExtractLow(mask), source1[0], source2[0]),
            radio_core::Select(ExtractHigh(mask), source1[1], source2[1])};
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    return {radio_core::Reverse(value[1]), radio_core::Reverse(value[0])};
  }
};

}  // namespace radio_core
