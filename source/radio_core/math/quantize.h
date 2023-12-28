// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <limits>
#include <type_traits>

namespace radio_core {

// Convert floating point value from range [0 .. 1] to the full range of an
// unsigned integral type.
template <class UnsignedIntegerType, class RealType>
auto PositiveUnitFloatToUnsigned(const RealType value) -> UnsignedIntegerType {
  static_assert(std::is_floating_point_v<RealType>);
  static_assert(std::is_unsigned_v<UnsignedIntegerType>);

  if (value <= 0) {
    return 0;
  }

  if (value >= 1) {
    return std::numeric_limits<UnsignedIntegerType>::max();
  }

  return value * std::numeric_limits<UnsignedIntegerType>::max();
}

}  // namespace radio_core
