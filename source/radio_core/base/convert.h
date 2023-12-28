// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace radio_core {

namespace convert_internal {

// Convert character to an integer value.
inline auto CharToDigit(const char ch) -> int { return ch - '0'; }

// Check whether the character is a decimal separator.
inline auto IsDecimalSeparator(const char ch) -> bool { return ch == '.'; }

// Implementation of the string to integer conversion.
template <class Int>
auto StringToInt(const std::string_view str, std::string_view* remainder_ptr)
    -> Int {
  size_t index = 0;

  Int sign = 1;
  if (str.starts_with('-')) {
    sign = -1;
    ++index;
  } else if (str.starts_with('+')) {
    sign = 1;
    ++index;
  }

  const size_t size = str.size();
  Int result = 0;
  while (index < size) {
    const char ch = str[index];
    if (ch >= '0' && ch <= '9') {
      // TODO(sergey): Find a solution which will avoid per-digit multiplication
      // and will not cause integer overflow for negative values.
      result = result * 10 + CharToDigit(ch) * sign;
      ++index;
    } else {
      break;
    }
  }

  if (remainder_ptr != nullptr) {
    *remainder_ptr = str.substr(index);
  }

  return result;
}

template <class RealType>
auto StringToFloat(const std::string_view str, std::string_view* remainder_ptr)
    -> RealType {
  const size_t size = str.size();
  size_t index = 0;

  RealType sign = 1;
  if (str.starts_with('-')) {
    sign = -1;
    ++index;
  } else if (str.starts_with('+')) {
    sign = 1;
    ++index;
  }

  RealType value = 0;

  // Integer part.
  while (index < size) {
    const char ch = str[index];
    if (ch >= '0' && ch <= '9') {
      value = value * 10 + CharToDigit(ch);
      ++index;
    } else {
      break;
    }
  }

  // Fractional part.
  if (index < size && IsDecimalSeparator(str[index])) {
    ++index;
    RealType divider = RealType(1) / 10;
    while (index < size) {
      const char ch = str[index];
      if (ch >= '0' && ch <= '9') {
        value = value + CharToDigit(ch) * divider;
        divider /= 10;
        ++index;
      } else {
        break;
      }
    }
  }

  if (remainder_ptr != nullptr) {
    *remainder_ptr = str.substr(index);
  }

  return sign * value;
}

}  // namespace convert_internal

// Convert string to an integer value.
//
// The string is being interpreted while the value is representing a valid
// integral. The optional remainder hold the un-interpreted part of the string.
//
// Does not perform any locale lookups so often behaves faster than the similar
// functions from the standard library.
//
// Does not keep track of possible overflow, and is only working with base-10
// values.
template <class Int>
auto StringToInt(const std::string_view str) -> Int {
  return convert_internal::StringToInt<Int>(str, nullptr);
}
template <class Int>
auto StringToInt(const std::string_view str, std::string_view& remainder)
    -> Int {
  return convert_internal::StringToInt<Int>(str, &remainder);
}

// Convert string to a floating point value.
//
// The string is being interpreted while the value is representing a valid
// floating point value. The optional remainder hold the un-interpreted part of
// the string.
//
// Does not perform any locale lookups so often behaves faster than the similar
// functions from the standard library.
//
// Does not keep track of possible overflow, and is only working with base-10
// values, without scientific or nan/inf notation.
template <class RealType>
auto StringToFloat(const std::string_view str) -> RealType {
  return convert_internal::StringToFloat<RealType>(str, nullptr);
}
template <class RealType>
auto StringToFloat(const std::string_view str, std::string_view& remainder)
    -> RealType {
  return convert_internal::StringToFloat<RealType>(str, &remainder);
}

}  // namespace radio_core
