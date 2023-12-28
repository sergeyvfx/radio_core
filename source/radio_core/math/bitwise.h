// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Bit level manipulation helpers.

#pragma once

#include <cstdint>
#include <type_traits>

#include "radio_core/base/bit_cast.h"
#include "radio_core/base/compiler_specific.h"

// Polymorphic functions for the half-precision floating point values.
#include "radio_core/math/internal/half_bitwise.h"

#if !HAS_BUILTIN(__builtin_bit_cast)
#  include <cstring>
#endif

namespace radio_core {

// BitfieldWithSize allows to access type which is needed to hold bits of a bit
// field with given size in bytes.
//
// Example:
//
//   typename BitfieldWithSize<sizeof(MyType)> bitfield;

template <int SizeInBytes>
struct BitfieldWithSize;

template <>
struct BitfieldWithSize<1> {
  using Type = uint8_t;
};

template <>
struct BitfieldWithSize<2> {
  using Type = uint16_t;
};

template <>
struct BitfieldWithSize<4> {
  using Type = uint32_t;
};

template <>
struct BitfieldWithSize<8> {
  using Type = uint64_t;
};

// A shortcut for creating bit field which matches size of the given type.
//
// Example:
//
//   typename BitfieldForType<MyType> bitfield;
template <class T>
struct BitfieldForType : public BitfieldWithSize<sizeof(T)> {};

// Bitwise select.
//
// Sets each bit in the result to the corresponding bit from the first source
// when the mask bit is 1, otherwise from the second source.
//
//   RESULT[i] = mask[i] == 1 ? source1[i] : source2[i] for i = 0 to <num bits>
template <class T, class = std::enable_if_t<std::is_arithmetic_v<T>>>
inline auto Select(const typename BitfieldForType<T>::Type& mask,
                   const T& source1,
                   const T& source2) -> T {
  using BitfieldType = typename BitfieldForType<T>::Type;

  const BitfieldType source1_bits = BitCast<BitfieldType>(source1);
  const BitfieldType source2_bits = BitCast<BitfieldType>(source2);

  const BitfieldType result_bits =
      (source1_bits & mask) | (source2_bits & ~mask);

  return BitCast<T>(result_bits);
}

}  // namespace radio_core
