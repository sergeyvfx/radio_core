// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Mathematical operations for half precision floating point values.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

namespace radio_core {

// Computes the smallest integer value not less than arg.
inline auto Ceil(const Half arg) -> Half { return Half(std::ceil(float(arg))); }

// Computes the largest integer value not greater than arg.
inline auto Floor(const Half arg) -> Half {
  return Half(std::floor(float(arg)));
}

// Computes the nearest integer value to arg (in floating-point format),
// rounding halfway cases away from zero, regardless of the current rounding
// mode.
inline auto Round(const Half arg) -> Half {
  return Half(std::round(float(arg)));
}

// Computes the nearest integer not greater in magnitude than arg.
inline auto Trunc(const Half arg) -> Half {
  return Half(std::trunc(float(arg)));
}

// Computes the nearest integer value to arg (in integer format), rounding
// halfway cases away from zero, regardless of the current rounding mode.
inline auto RoundToInt(const Half arg) -> long {
  return std::lround(float(arg));
}

// Computes the principal value of the arc tangent of arg.
inline auto ArcTan(const Half arg) -> Half {
  return Half(std::atan(float(arg)));
};

// Computes the arc tangent of y/x using the signs of arguments to determine
// the correct quadrant.
inline auto ArcTan2(const Half y, const Half x) -> Half {
  return Half(std::atan2(float(y), float(x)));
};

// Computes the sine and cosine of arg (measured in radians).
inline auto Sin(const Half arg) -> Half { return Half(std::sin(float(arg))); }
inline auto Cos(const Half arg) -> Half { return Half(std::cos(float(arg))); }

// Computes e (Euler's number, 2.7182818...) raised to the given power arg.
inline auto Exp(const Half arg) -> Half { return Half(std::exp(float(arg))); }

// Computes the value of base raised to the power exp.
inline auto Pow(const Half base, const Half exp) -> Half {
  return Half(std::pow(float(base), float(exp)));
}

// Computes the square root of arg.
inline auto Sqrt(const Half arg) -> Half { return Half(std::sqrt(float(arg))); }

// Computes the natural (base e) logarithm of arg.
inline auto Log(const Half arg) -> Half { return Half(std::log(float(arg))); }

// Computes the binary (base-2) logarithm of arg.
inline auto Log2(const Half arg) -> Half { return Half(std::log2(float(arg))); }

// Computes the common (base-10) logarithm of arg.
inline auto Log10(const Half arg) -> Half {
  return Half(std::log10(float(arg)));
}

// Computes the absolute value of a value arg.
inline auto Abs(const Half arg) -> Half {
  return UInt16AsHalf(HalfAsUInt16(arg) & 0x7fff);
}

// Computes the square root of the sum of the squares of x and y, without undue
// overflow or underflow at intermediate stages of the computation
inline auto Hypot(const Half x, const Half y) -> Half {
  return Half(std::hypot(float(x), float(y)));
}

// Computes the floating-point remainder of the division operation.
inline auto Modulo(const Half x, const Half y) -> Half {
  return Half(std::fmod(float(x), float(y)));
}

// Composes a floating point value with the magnitude of mag and the sign of
// sgn.
inline auto CopySign(const Half mag, const Half sgn) -> Half {
  const uint16_t mag_bits = HalfAsUInt16(mag) & 0x7fff;
  const uint16_t sign_bit = (HalfAsUInt16(sgn) & 0x8000);
  return UInt16AsHalf(mag_bits | sign_bit);
}

// Determines if the given floating point number has finite value i.e. it is
// not infinite or NaN.
//
// This function is safe for fast-math optimization.
inline auto IsFinite(const Half f) -> bool {
  const uint16_t exponent_bits = (HalfAsUInt16(f) & 0x7c00);
  return exponent_bits != 0x7c00;
}

inline auto IsNaN(const Half f) -> bool {
  const uint16_t exponent_bits = (HalfAsUInt16(f) & 0x7e00);
  return exponent_bits == 0x7e00;
}

}  // namespace radio_core

#endif
