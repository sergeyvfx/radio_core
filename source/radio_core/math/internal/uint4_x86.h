// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of 4-element 32 bit integer values using SSE2 and above CPU
// instruction set.
// The SSE2 is the minimum system requirement.

#pragma once

#include "radio_core/base/build_config.h"

#if ARCH_CPU_X86_FAMILY

#  include "radio_core/math/internal/math_x86.h"

namespace radio_core {

static_assert(ISA_CPU_X86_SSE2, "SSE 2 is the required minimum");

template <class T, int N, bool SpecializationMarker>
struct VectorizedIntTypeInfo;

namespace internal::x64 {

// An implementation pf _mm_min_epi32() from SSE4.1 which falls back to an
// emulation for SSE2.
inline auto mm_min_epi32(__m128i a, __m128i b) -> __m128i {
#  if ISA_CPU_X86_SSE4_1
  return _mm_min_epi32(a, b);
#  else
  const __m128i mask = _mm_cmplt_epi32(a, b);
  a = _mm_and_si128(a, mask);
  b = _mm_andnot_si128(mask, b);
  return _mm_or_si128(a, b);
#  endif
}

// An implementation pf _mm_max_epi32() from SSE4.1 which falls back to an
// emulation for SSE2.
inline auto mm_max_epi32(__m128i a, __m128i b) -> __m128i {
#  if ISA_CPU_X86_SSE4_1
  return _mm_max_epi32(a, b);
#  else
  const __m128i mask = _mm_cmpgt_epi32(a, b);
  a = _mm_and_si128(a, mask);
  b = _mm_andnot_si128(mask, b);
  return _mm_or_si128(a, b);
#  endif
}

}  // namespace internal::x64

template <>
struct VectorizedIntTypeInfo<uint32_t, 4, true> {
  using RegisterType = __m128i;

  static constexpr int kSize = 4;
  static constexpr bool kIsVectorized = true;

  static auto GetName() -> const char* { return "X86"; }

  //////////////////////////////////////////////////////////////////////////////
  // Helpers.

  // Cast from memory pointer type to __m128i*.
  static inline auto CastPtr(uint32_t* ptr) -> __m128i* {
    return reinterpret_cast<__m128i*>(ptr);
  }
  static inline auto CastPtr(const uint32_t* ptr) -> const __m128i* {
    return reinterpret_cast<const __m128i*>(ptr);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Load.

  static inline auto Load(const uint32_t values[4]) -> __m128i {
    return _mm_loadu_si128(CastPtr(values));
  }

  static inline auto Load(const uint32_t a,
                          const uint32_t b,
                          const uint32_t c,
                          const uint32_t d) -> __m128i {
    // NOTE: Can not trust order of function arguments in memory, so ensure they
    // are loaded into a continuous memory chunk.
    //
    // TODO(sergey): Optimize this somehow, avoiding possible extra memory
    // transfers.
    const uint32_t values[4] = {a, b, c, d};
    return Load(values);
  }

  static inline auto Load(const uint32_t value) -> __m128i {
    return _mm_set1_epi32(int(value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  static inline void Store(const __m128i& value, uint32_t dst[4]) {
    _mm_storeu_si128(CastPtr(dst), value);
  }

  template <int Index>
  static inline void Store(const __m128i& value, uint32_t* dst) {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    *dst = Extract<Index>(value);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  template <int Index>
  static inline auto Extract(const __m128i& value) -> uint32_t {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

#  if ISA_CPU_X86_SSE4_1
    return _mm_extract_epi32(value, Index);
#  else
    union {
      __m128i v;
      int32_t a[4];
    } u32{value};
    return u32.a[Index];
#  endif
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.

  template <int Index>
  static inline auto SetLane(const __m128i& value,
                             const uint32_t new_lane_value) -> __m128i {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    // TODO(sergey): With SSE4 available _mm_insert_ps or _mm_insert_epi16 can
    // be used.

    alignas(16) uint32_t tmp[4];
    Store(value, tmp);
    tmp[Index] = new_lane_value;

    return Load(tmp);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-class functions.

  static inline auto Min(const __m128i& a, const __m128i& b) -> __m128i {
    return internal::x64::mm_min_epi32(a, b);
  }

  static inline auto Max(const __m128i& a, const __m128i& b) -> __m128i {
    return internal::x64::mm_max_epi32(a, b);
  }

  static inline auto HorizontalMax(const __m128i& value) -> uint32_t {
    const __m128i max1 = _mm_shuffle_epi32(value, _MM_SHUFFLE(0, 0, 3, 2));
    const __m128i max2 = internal::x64::mm_max_epi32(value, max1);
    const __m128i max3 = _mm_shuffle_epi32(max2, _MM_SHUFFLE(0, 0, 0, 1));
    return _mm_cvtsi128_si32(internal::x64::mm_max_epi32(max2, max3));
  }

  static inline auto Select(const __m128i& mask,
                            const __m128i& source1,
                            const __m128i& source2) -> __m128i {
    const __m128i bits_from_source1 = _mm_and_si128(mask, source1);
    const __m128i bits_from_source2 = _mm_andnot_si128(mask, source2);
    return _mm_or_si128(bits_from_source1, bits_from_source2);
  }

  static inline auto Reverse(const __m128i& value) -> __m128i {
    return _mm_shuffle_epi32(value, _MM_SHUFFLE(0, 1, 2, 3));
  }
};

}  // namespace radio_core

#endif
