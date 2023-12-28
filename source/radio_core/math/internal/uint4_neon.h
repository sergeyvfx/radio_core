// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element 32 bit integer values using Neon CPU instruction
// set.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include "radio_core/math/internal/math_neon.h"

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo;

template <>
struct VectorizedIntTypeInfo<uint32_t, 4, true> {
  using RegisterType = uint32x4_t;

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const uint32_t values[4]) -> uint32x4_t {
    return vld1q_u32(values);
  }

  static inline auto Load(const uint32_t a,
                          const uint32_t b,
                          const uint32_t c,
                          const uint32_t d) -> uint32x4_t {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const uint32_t values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const uint32_t value) -> uint32x4_t {
    return vdupq_n_u32(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const uint32x4_t& value, uint32_t dst[4]) {
    vst1q_u32(dst, value);
  }

  template <int Index>
  static inline void Store(const uint32x4_t& value, uint32_t* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    vst1q_lane_u32(dst, value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const uint32x4_t& value) -> uint32_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vgetq_lane_u32(value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const uint32x4_t& value,
                             const float new_lane_value) -> uint32x4_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vsetq_lane_u32(new_lane_value, value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto Min(const uint32x4_t& a, const uint32x4_t& b)
      -> uint32x4_t {
    return vminq_u32(a, b);
  }

  static inline auto Max(const uint32x4_t& a, const uint32x4_t& b)
      -> uint32x4_t {
    return vmaxq_u32(a, b);
  }

  static inline auto HorizontalMax(const uint32x4_t& value) -> uint32_t {
#  if ARCH_CPU_64_BITS
    return vmaxvq_u32(value);
#  else
    uint32x2_t r = vpmax_u32(vget_low_u32(value), vget_high_u32(value));
    r = vpmax_u32(r, r);
    return vget_lane_u32(r, 0);
#  endif
  }

  static inline auto Select(const uint32x4_t& mask,
                            const uint32x4_t& source1,
                            const uint32x4_t& source2) -> uint32x4_t {
    return vbslq_u32(mask, source1, source2);
  }

  static inline auto Reverse(const uint32x4_t& value) -> uint32x4_t {
    return internal::neon::vreveseq_u32(value);
  }
};

}  // namespace radio_core

#endif  // ISA_CPU_ARM_NEON
