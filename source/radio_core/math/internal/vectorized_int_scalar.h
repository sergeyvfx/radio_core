// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of N-element integer values using naive scalar operations.

#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>

#include "radio_core/base/algorithm.h"
#include "radio_core/base/aligned_register.h"
#include "radio_core/base/unroll.h"
#include "radio_core/math/bitwise.h"
#include "radio_core/math/internal/vectorized_type.h"

namespace radio_core {

template <class T, int N>
class VectorizedIntType;

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo {
  using RegisterType = AlignedRegister<T, N, 16>;

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

  static inline auto Load(const T value) -> RegisterType { return {value}; }

  template <int HalfN = N / 2, typename = std::enable_if_t<HalfN >= 4>>
  static inline auto Load(
      const typename VectorizedIntType<T, HalfN>::RegisterType& low,
      const typename VectorizedIntType<T, HalfN>::RegisterType& high)
      -> RegisterType {
    RegisterType r;
    Unroll<HalfN>([&](const auto i) {
      r[i] = VectorizedIntType<T, HalfN>::TypeInfo::template Extract<i>(low);
    });
    Unroll<HalfN>([&](const auto i) {
      r[i + HalfN] =
          VectorizedIntType<T, HalfN>::TypeInfo::template Extract<i>(high);
    });
    return r;
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
                          VectorizedIntType<T, HalfN>> {
    return VectorizedIntType<T, HalfN>(&value[0]);
  }

  template <int HalfN = N / 2>
  static inline auto ExtractHigh(const RegisterType& value)
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedIntType<T, HalfN>> {
    return VectorizedIntType<T, HalfN>(&value[HalfN]);
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

  static inline auto Select(const RegisterType& maskN,
                            const RegisterType& source1,
                            const RegisterType& source2) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) {
      r[i] = radio_core::Select(Extract<i>(maskN), source1[i], source2[i]);
    });
    return r;
  }

  static inline auto Reverse(const RegisterType& value) -> RegisterType {
    RegisterType r;
    Unroll<N>([&](const auto i) { r[i] = value[N - i - 1]; });
    return r;
  }
};

}  // namespace radio_core
