// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Number of digits in a base 10 representation of a positive integral value a.
// The sign is not counted as a digit.
//
// The unrolled version is from https://stackoverflow.com/a/1489873

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace radio_core {

template <class T>
constexpr auto NumDigits(T arg) -> int {
  static_assert(std::is_integral_v<T>);

  int digits = 0;
  do {
    arg /= 10;
    ++digits;
  } while (arg != 0);

  return digits;
}

// Partial specialization optimization for 64-bit numbers.
template <>
constexpr auto NumDigits(const int64_t arg) -> int {
  if (arg == std::numeric_limits<int64_t>::min()) {
    return 19;
  }
  if (arg < 0) {
    return NumDigits(-arg);
  }

  if (arg >= 10000000000) {
    if (arg >= 100000000000000) {
      if (arg >= 10000000000000000) {
        if (arg >= 100000000000000000) {
          if (arg >= 1000000000000000000) {
            return 19;
          }
          return 18;
        }
        return 17;
      }
      if (arg >= 1000000000000000) {
        return 16;
      }
      return 15;
    }
    if (arg >= 1000000000000) {
      if (arg >= 10000000000000) {
        return 14;
      }
      return 13;
    }
    if (arg >= 100000000000) {
      return 12;
    }
    return 11;
  }

  if (arg >= 100000) {
    if (arg >= 10000000) {
      if (arg >= 100000000) {
        if (arg >= 1000000000) {
          return 10;
        }
        return 9;
      }
      return 8;
    }
    if (arg >= 1000000) {
      return 7;
    }
    return 6;
  }

  if (arg >= 100) {
    if (arg >= 1000) {
      if (arg >= 10000) {
        return 5;
      }
      return 4;
    }
    return 3;
  }

  if (arg >= 10) {
    return 2;
  }

  return 1;
}

// Partial specialization optimization for 64-bit numbers.
template <>
constexpr auto NumDigits(const int32_t arg) -> int {
  if (arg == std::numeric_limits<int32_t>::min()) {
    return 10;
  }
  if (arg < 0) {
    return NumDigits(-arg);
  }

  if (arg >= 10000) {
    if (arg >= 10000000) {
      if (arg >= 100000000) {
        if (arg >= 1000000000) {
          return 10;
        }
        return 9;
      }
      return 8;
    }

    if (arg >= 100000) {
      if (arg >= 1000000) {
        return 7;
      }
      return 6;
    }
    return 5;
  }

  if (arg >= 100) {
    if (arg >= 1000) {
      return 4;
    }
    return 3;
  }

  if (arg >= 10) {
    return 2;
  }

  return 1;
}

}  // namespace radio_core
