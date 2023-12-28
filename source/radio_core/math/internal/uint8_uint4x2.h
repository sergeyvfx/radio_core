// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element 32 bit integer values using 2 Uint4 scalars.
// Relies on the SIMD optimization of the Uint4.

#pragma once

#include "radio_core/base/aligned_register.h"
#include "radio_core/math/uint4.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo;

template <bool SpecializationMarker>
struct VectorizedIntTypeInfo<uint32_t, 8, SpecializationMarker> {
  using RegisterType = AlignedRegister<UInt4, 2, 32>;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = false;

  static auto GetName() -> const char* { return "UInt4x2"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const uint32_t values[8]) -> RegisterType {
    return {UInt4(values), UInt4(values + 4)};
  }

  static inline auto Load(const uint32_t a,
                          const uint32_t b,
                          const uint32_t c,
                          const uint32_t d,
                          const uint32_t e,
                          const uint32_t f,
                          const uint32_t g,
                          const uint32_t h) -> RegisterType {
    return {UInt4(a, b, c, d), UInt4(e, f, g, h)};
  }

  static inline auto Load(const uint32_t value) -> RegisterType {
    return {UInt4(value), UInt4(value)};
  }

  static inline auto Load(const UInt4::RegisterType& low,
                          const UInt4::RegisterType& high) -> RegisterType {
    return {low, high};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, uint32_t dst[8]) {
    value[0].Store(dst);
    value[1].Store(dst + 4);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, uint32_t* dst) {
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
  static inline auto Extract(const RegisterType& value) -> uint32_t {
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
      -> VectorizedIntType<uint32_t, 4> {
    return value[0];
  }

  static inline auto ExtractHigh(const RegisterType& value)
      -> VectorizedIntType<uint32_t, 4> {
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

  static inline auto Min(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Min(a[0], b[0]), radio_core::Min(a[1], b[1])};
  }

  static inline auto Max(const RegisterType& a, const RegisterType& b)
      -> RegisterType {
    return {radio_core::Max(a[0], b[0]), radio_core::Max(a[1], b[1])};
  }

  static inline auto HorizontalMax(const RegisterType& value) -> uint32_t {
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
