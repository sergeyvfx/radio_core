// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Half-precision floating point value.
//
// The availability is platform/compiler dependent. When it is supported the
// RADIO_CORE_HAVE_HALF is defined to 1 (otherwise it is defined to 0).
//
// Provides a Half data type which acts like any other built-in floating point
// type like float or double. Implements construction, casts, mathematical
// operations.

#pragma once

#include "radio_core/base/build_config.h"

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <ostream>

namespace radio_core {

#if COMPILER_CLANG

class Half;

class Half {
 public:
  // Default constructor which leaves the value un-initialized.
  Half() = default;

  // Copy constructor.
  constexpr Half(const Half& value) : value_(value.value_) {}

  // Construction from various scalar types.
  //
  // Not marked as explicit to help with emulation on systems which do not have
  // support of f16 floating point suffix: it otherwise becomes too verbose to
  // create vectorized half-floating point values.
  constexpr Half(const _Float16 value) : value_(value) {}
  constexpr Half(const int value) : value_(value) {}
  constexpr Half(const float value) : value_(value) {}
  constexpr Half(const double value) : value_(value) {}

  // Assignment.
  constexpr auto operator=(const Half rhs) -> Half& {
    value_ = rhs.value_;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Access.

  explicit constexpr operator int() const { return int(value_); }
  explicit constexpr operator size_t() const { return size_t(value_); }
  explicit constexpr operator float() const { return float(value_); }
  explicit constexpr operator double() const { return double(value_); }
  explicit constexpr operator _Float16() const { return value_; }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operations.

  // Get the value which value is the negative of this value.
  constexpr auto operator-() const -> Half { return Half(-value_); }

  // Addition.
  constexpr auto operator+=(const Half rhs) -> Half& {
    value_ += rhs.value_;
    return *this;
  }

  // Subtraction.
  constexpr auto operator-=(const Half rhs) -> Half& {
    value_ -= rhs.value_;
    return *this;
  }

  // Multiplication.
  constexpr auto operator*=(const Half rhs) -> Half& {
    value_ *= rhs.value_;
    return *this;
  }

  // Division.
  constexpr auto operator/=(const Half rhs) -> Half& {
    value_ /= rhs.value_;
    return *this;
  }

  // Comparison.
  friend constexpr auto operator<=>(const Half lhs, const Half rhs) {
    return lhs.value_ <=> rhs.value_;
  }
  friend constexpr auto operator==(const Half lhs, const Half rhs) -> bool {
    return lhs.value_ == rhs.value_;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-member functions.

  // Addition.
  friend constexpr auto operator+(const Half lhs, const Half rhs) -> Half {
    return Half(lhs.value_ + rhs.value_);
  }

  // Subtraction.
  friend constexpr auto operator-(const Half lhs, const Half rhs) -> Half {
    return Half(lhs.value_ - rhs.value_);
  }

  // Multiplication.
  friend constexpr auto operator*(const Half lhs, const Half rhs) -> Half {
    return Half(lhs.value_ * rhs.value_);
  }

  // Division.
  friend constexpr auto operator/(const Half lhs, const Half rhs) -> Half {
    return Half(lhs.value_ / rhs.value_);
  }

  friend auto operator<<(std::ostream& os, const Half value) -> std::ostream& {
    os << float(value.value_);
    return os;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  _Float16 value_;
};

static_assert(sizeof(Half) == 2);

// Bit-wise cast of half-floating point arg to 16 bit unsigned integer.
inline auto HalfAsUInt16(const Half arg) -> uint16_t {
  union {
    Half f;
    uint16_t i;
  } a;

  a.f = arg;

  return a.i;
}

// Bit-wise cast of 16 bit unsigned integer arg to half-floating point.
inline auto UInt16AsHalf(const uint16_t arg) -> Half {
  union {
    Half f;
    uint16_t i;
  } a;

  a.i = arg;

  return a.f;
}

#  define RADIO_CORE_HAVE_HALF 1
#endif  // COMPILER_CLANG

#if !defined(RADIO_CORE_HAVE_HALF)
#  define RADIO_CORE_HAVE_HALF 0
#endif

}  // namespace radio_core
