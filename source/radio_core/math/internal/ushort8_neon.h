// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 8-element 16 bit integer values using Neon CPU instruction
// set.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include "radio_core/math/internal/math_neon.h"

#  include <cstdint>

namespace radio_core {

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo;

template <>
struct VectorizedIntTypeInfo<uint16_t, 8, true> {
  using RegisterType = uint16x8_t;

  static constexpr int kSize = 8;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "Neon"; }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const uint16_t values[8]) -> RegisterType {
    return vld1q_u16(values);
  }

  static inline auto Load(const uint16_t a,
                          const uint16_t b,
                          const uint16_t c,
                          const uint16_t d,
                          const uint16_t e,
                          const uint16_t f,
                          const uint16_t g,
                          const uint16_t h) -> RegisterType {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const uint16_t values[8] = {a, b, c, d, e, f, g, h};
    return Load(values);
  }

  static inline auto Load(const uint16_t value) -> RegisterType {
    return vdupq_n_u16(value);
  }

  static inline auto Load(const UShort4::RegisterType& low,
                          const UShort4::RegisterType& high) -> RegisterType {
    return vcombine_u16(low, high);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const RegisterType& value, uint16_t dst[8]) {
    vst1q_u16(dst, value);
  }

  template <int Index>
  static inline void Store(const RegisterType& value, uint16_t* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    vst1q_lane_u16(dst, value, Index);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const uint16x8_t& value) -> uint16_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return vgetq_lane_u16(value, Index);
  }

  static inline auto ExtractLow(const uint16x8_t& value) -> UShort4 {
    return UShort4(vget_low_u16(value));
  }

  static inline auto ExtractHigh(const uint16x8_t& value) -> UShort4 {
    return UShort4(vget_high_u16(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto Min(const uint16x8_t& a, const uint16x8_t& b)
      -> uint16x8_t {
    return vminq_u16(a, b);
  }

  static inline auto Max(const uint16x8_t& a, const uint16x8_t& b)
      -> uint16x8_t {
    return vmaxq_u16(a, b);
  }

  static inline auto HorizontalMax(const uint16x8_t& value) -> uint16_t {
#  if ARCH_CPU_64_BITS
    return vmaxvq_u16(value);
#  else
    uint16x4_t r = vpmax_u16(vget_low_u16(value), vget_high_u16(value));
    r = vpmax_u16(r, r);
    r = vpmax_u16(r, r);
    return vget_lane_u16(r, 0);
#  endif
  }

  static inline auto Select(const uint16x8_t& mask,
                            const uint16x8_t& source1,
                            const uint16x8_t& source2) -> uint16x8_t {
    return vbslq_u16(mask, source1, source2);
  }

  static inline auto Reverse(const uint16x8_t& value) -> uint16x8_t {
    return internal::neon::vreveseq_u16(value);
  }
};

}  // namespace radio_core

#endif  // ISA_CPU_ARM_NEON
