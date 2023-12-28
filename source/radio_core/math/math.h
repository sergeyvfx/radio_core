// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <type_traits>

#include "radio_core/base/bit_cast.h"
#include "radio_core/base/constants.h"

// Polymorphic functions for the half-precision floating point values.
#include "radio_core/math/internal/half_math.h"

namespace radio_core {

// Computes the smallest integer value not less than arg.
inline auto Ceil(const float arg) -> float { return std::ceil(arg); }
inline auto Ceil(const double arg) -> double { return std::ceil(arg); }

// Computes the largest integer value not greater than arg.
inline auto Floor(const float arg) -> float { return std::floor(arg); }
inline auto Floor(const double arg) -> double { return std::floor(arg); }

// Computes the nearest integer value to arg (in floating-point format),
// rounding halfway cases away from zero, regardless of the current rounding
// mode.
inline auto Round(const float arg) -> float { return std::round(arg); }
inline auto Round(const double arg) -> double { return std::round(arg); }

// Computes the nearest integer not greater in magnitude than arg.
inline auto Trunc(const float arg) -> float { return std::trunc(arg); }
inline auto Trunc(const double arg) -> double { return std::trunc(arg); }

// Computes the nearest integer value to arg (in integer format), rounding
// halfway cases away from zero, regardless of the current rounding mode.
inline auto RoundToInt(const float arg) -> long { return std::lround(arg); }
inline auto RoundToInt(const double arg) -> long { return std::lround(arg); }

// Computes the principal value of the arc tangent of arg.
inline auto ArcTan(const float arg) -> float { return std::atan(arg); };
inline auto ArcTan(const double arg) -> double { return std::atan(arg); };

// Computes the arc tangent of y/x using the signs of arguments to determine
// the correct quadrant.
inline auto ArcTan2(const float y, const float x) -> float {
  return std::atan2(y, x);
};
inline auto ArcTan2(const double y, const double x) -> double {
  return std::atan2(y, x);
};

// Computes the sine and cosine of arg (measured in radians).
inline auto Sin(const float arg) -> float { return std::sin(arg); }
inline auto Sin(const double arg) -> double { return std::sin(arg); }
inline auto Cos(const float arg) -> float { return std::cos(arg); }
inline auto Cos(const double arg) -> double { return std::cos(arg); }

// Computes e (Euler's number, 2.7182818...) raised to the given power arg.
inline auto Exp(const float arg) -> float { return std::exp(arg); }
inline auto Exp(const double arg) -> double { return std::exp(arg); }

// Computes the value of base raised to the power exp.
inline auto Pow(const float base, const float exp) -> float {
  return std::pow(base, exp);
}
inline auto Pow(const double base, const double exp) -> double {
  return std::pow(base, exp);
}

// Computes the square root of arg.
inline auto Sqrt(const float arg) -> float { return std::sqrt(arg); }
inline auto Sqrt(const double arg) -> double { return std::sqrt(arg); }

// Computes the natural (base e) logarithm of arg.
inline auto Log(const float arg) -> float { return std::log(arg); }
inline auto Log(const double arg) -> double { return std::log(arg); }

// Computes the binary (base-2) logarithm of arg.
inline auto Log2(const float arg) -> float { return std::log2(arg); }
inline auto Log2(const double arg) -> double { return std::log2(arg); }

// Computes the common (base-10) logarithm of arg.
inline auto Log10(const float arg) -> float { return std::log10(arg); }
inline auto Log10(const double arg) -> double { return std::log10(arg); }

// Computes the absolute value of a value arg.
inline auto Abs(const int arg) -> int { return std::abs(arg); }
inline auto Abs(const float arg) -> float { return std::abs(arg); }
inline auto Abs(const double arg) -> double { return std::abs(arg); }

// Computes the square root of the sum of the squares of x and y, without undue
// overflow or underflow at intermediate stages of the computation
inline auto Hypot(const float x, const float y) -> float {
  return std::hypot(x, y);
}
inline auto Hypot(const double x, const double y) -> double {
  return std::hypot(x, y);
}

// Computes the floating-point remainder of the division operation.
inline auto Modulo(const float x, const float y) -> float {
  return std::fmod(x, y);
}
inline auto Modulo(const double x, const double y) -> double {
  return std::fmod(x, y);
}
template <class T>
inline auto Modulo(const T& x, const T& y) -> T {
  return x - Trunc(x / y) * y;
}

// Composes a floating point value with the magnitude of mag and the sign of
// sgn.
inline auto CopySign(const float mag, const float sgn) -> float {
  return std::copysign(mag, sgn);
}

// Determines if the given floating point number has finite value i.e. it is
// not infinite or NaN.
//
// This function is safe for fast-math optimization.
//
// TODO(sergey): Investigate behavior for subnormal values.
inline auto IsFinite(const float f) -> bool {
  /// By IEEE 754 rule, 2*Inf equals Inf.
  // Re-formulate it as f != f / 2 to avoid constant overflow warning with MSVC.
  const uint32_t i = BitCast<uint32_t>(f);
  return (f == f) && (i == 0 || i == (1u << 31) || (f != f / 2.0f)) &&
         !((i << 1) > 0xff000000u);
}

// Determines if the given floating point number is a not-a-number (NaN) value.
inline auto IsNaN(const float f) -> bool { return std::isnan(f); }
inline auto IsNaN(const double f) -> bool { return std::isnan(f); }

// Clamp the input argument to the given range.
template <class T>
inline auto Clamp(const T& a, const T& min, const T& max) -> T {
  if (a <= min) {
    return min;
  }
  if (a >= max) {
    return max;
  }

  return a;
}

// Clamp the input argument to [+0.0, 1.0].
template <class T>
inline auto Saturate(const T& a) -> T {
  return Clamp(a, T(0), T(1));
}

// Value of 1 with the same sign as the given argument.
// Zero has positive sign. If the implementation supports the signed zero
// consistently in arithmetic operations.
template <class T>
inline auto Sign(const T& arg) -> T {
  return CopySign(T(1), arg);
}

// Convert value measured in degrees to value measured in radians.
template <class T>
inline constexpr auto DegreesToRadians(const T& value) -> T {
  return value * T(constants::pi) / T(180.0);
}

// Convert value measured in radians to value measured in degrees.
template <class T>
inline constexpr auto RadiansToDegrees(const T& value) -> T {
  return value / T(constants::pi) * T(180.0);
}

// Normalized sinc function.
//
// NOTE: Sinc(0) is the limit value 1.
//
// NOTE: The normalization factor of Pi used in the definition. This is the most
// commonly used definition in signal processing.
// Use Sinc(x / constants::pi) to obtain the unnormalized sinc function
// that is more common in mathematics.
//
// Reference:
//   https://wikipedia.org/wiki/Sinc_function
template <class T>
inline auto Sinc(const T& x) -> T {
  if (x == T(0)) {
    return T(1);
  }
  return Sin(T(constants::pi) * x) / (T(constants::pi) * x);
}

// Multiply-add to accumulator.
//   RESULT = a + (b * c)
template <class AccumulatorType, class T1, class T2>
inline constexpr auto MultiplyAdd(const AccumulatorType& a,
                                  const T1& b,
                                  const T2& c) {
  return a + (b * c);
}

// Linear interpolation between v0 and v1.
//
// This is an imprecise method (it is not guaranteed result will be v1 when
// t = 1), but it allows to benefit from hardware's FMA instruction.
template <class ValueType, class FactorType>
inline constexpr auto Lerp(const ValueType& v0,
                           const ValueType& v1,
                           const FactorType& t) {
  return MultiplyAdd(v0, v1 - v0, t);
}

// Faster version of Abs() which trades performance over accuracy.
template <class T>
inline auto FastAbs(const T& x) -> T {
  return Abs(x);
}

// Calculate Log10(a) in a possibly faster manner than the std::log10.
template <class T>
inline auto FastLog10(const T a) -> T {
  // The Log10(x) is calculated as Log2(x) / Log2(10) for the performance
  // reasons. This is because on some libc implementations, platforms,
  // optimization flags and so on the log2 implementation can be faster than
  // log10.
  //
  // This is a factor which the Log2(x) is to be multiplied with to get Log2(x).
  // The value is calculated as 1 / Log2(10).
  const T kLog2ToLog10Fac(0.3010299956639812);

  return Log2(a) * kLog2ToLog10Fac;
}

// Approximated implementation of ArcTan2() function which does not use
// branching. Typically faster than the ArcTan2() implementation.
//
// TODO(sergey): Gather performance and accuracy information.
//
// http://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization/
// https://gist.github.com/volkansalma/2972237
template <class T>
inline auto FastArcTan2(const T& y, const T& x) -> T {
  // TODO(sergey): Find a way to ensure that possible double-to-float narrowing
  // happens at compile time. Ideally, would even make this constexpr.

  const T pi(constants::pi);

  T abs_y = Abs(y) + T(1e-10);  // kludge to prevent 0/0 condition
  T r = (x - CopySign(abs_y, x)) / (abs_y + Abs(x));
  T angle = pi / T(2) - CopySign(pi / T(4), x);

  angle += (T(0.1963) * r * r - T(0.9817)) * r;
  return CopySign(angle, y);
}

// Fast version of Pow(base, exp) which works for integer values.
// Only works for positive values of exp.
//
// https://stackoverflow.com/a/101613
template <class T>
inline auto FastIntPow(T base, T exp) {
  assert(exp >= 0);

  T result(1);

  for (;;) {
    if (exp & 1) {
      result *= base;
    }

    exp >>= 1;
    if (!exp) {
      break;
    }

    base *= base;
  }

  return result;
}

// Round the value up to the lowest value which is higher or equal to the input
// and which is a multiple of given the multiplier.
//
// NOTE: Only works with positive integral values.
template <class T>
inline auto RoundUpToMultiple(const T arg, const T multiple) -> T {
  static_assert(std::is_integral_v<T>);
  assert(arg >= 0);

  return ((arg + multiple - 1) / multiple) * multiple;
}

// Round the value down to the highest value which is not greater than the input
// and which is a multiple of the given multiplier.
//
// NOTE: Only works with positive integral values.
template <class T>
inline auto RoundDownToMultiple(const T arg, const T multiple) -> T {
  static_assert(std::is_integral_v<T>);
  assert(arg >= 0);

  return (arg / multiple) * multiple;
}

// Calculate sine and cosine of the same argument `arg`.
// Depending on a platform could be faster than calling Sin() and Cos()
// sequentially.
template <class T>
inline void SinCos(const T arg, T& sine, T& cosine) {
  sine = Sin(arg);
  cosine = Cos(arg);
}

}  // namespace radio_core
