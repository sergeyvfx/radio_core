// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Bitwise operations for half precision floating point values.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/base/bit_cast.h"

#  include <cstdint>

namespace radio_core {

// Bitwise select.
//
// Sets each bit in the result to the corresponding bit from the first source
// when the mask bit is 1, otherwise from the second source.
//
//   RESULT[i] = mask[i] == 1 ? source1[i] : source2[i] for i = 0 to <num bits>
inline auto Select(const uint16_t mask, const Half source1, const Half source2)
    -> Half {
  union Convert {
    Half f;
    uint16_t i;
  };

  Convert a;
  a.f = source1;

  Convert b;
  b.f = source2;

  const uint16_t source1_bits = BitCast<uint16_t>(a.i);
  const uint16_t source2_bits = BitCast<uint16_t>(b.i);
  const uint16_t result_bits = (source1_bits & mask) | (source2_bits & ~mask);

  Convert r;
  r.i = BitCast<uint16_t>(result_bits);

  return r.f;
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
