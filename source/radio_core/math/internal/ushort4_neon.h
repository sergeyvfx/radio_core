// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element 16 bit integer values using Neon CPU instruction
// set.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include "radio_core/math/internal/math_neon.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo;

template <>
struct VectorizedIntTypeInfo<uint16_t, 4, true> {
  using RegisterType = uint16x4_t;

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const uint16_t values[4]) -> uint16x4_t {
    return vld1_u16(values);
  }

  static inline auto Load(const uint16_t a,
                          const uint16_t b,
                          const uint16_t c,
                          const uint16_t d) -> uint16x4_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const uint16_t values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const uint16_t value) -> uint16x4_t {
    return vdup_n_u16(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const uint16x4_t& value, uint16_t dst[4]) {
    vst1_u16(dst, value);
  }

  template <int Index>
  static inline void Store(const uint16x4_t& value, uint16_t* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    vst1_lane_u16(dst, value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const uint16x4_t& value) -> uint16_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vget_lane_u16(value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto Min(const uint16x4_t& a, const uint16x4_t& b)
      -> uint16x4_t {
    return vmin_u16(a, b);
  }

  static inline auto Max(const uint16x4_t& a, const uint16x4_t& b)
      -> uint16x4_t {
    return vmax_u16(a, b);
  }

  static inline auto HorizontalMax(const uint16x4_t& value) -> float {
#  if ARCH_CPU_64_BITS
    return vmaxv_u16(value);
#  else
    uint16x4_t r = vpmax_u16(value, value);
    r = vpmax_u16(r, r);
    return vget_lane_u16(r, 0);
#  endif
  }

  static inline auto Select(const uint16x4_t& mask,
                            const uint16x4_t& source1,
                            const uint16x4_t& source2) -> uint16x4_t {
    return vbsl_u16(mask, source1, source2);
  }

  static inline auto Reverse(const uint16x4_t& value) -> uint16x4_t {
    return vrev64_u16(value);
  }
};

}  // namespace radio_core

#endif  // ISA_CPU_ARM_NEON
