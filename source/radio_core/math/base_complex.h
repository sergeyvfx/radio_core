// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for complex values.

#pragma once

#include "radio_core/math/math.h"

#include <cmath>
#include <ostream>

namespace radio_core {

template <class T>
class BaseComplex {
 public:
  using value_type = T;

  BaseComplex() = default;

  constexpr inline explicit BaseComplex(const T& new_real)
      : BaseComplex(new_real, 0) {}

  constexpr inline BaseComplex(const T& new_real, const T& new_imag)
      : real(new_real), imag(new_imag) {}

  // Comparison.
  // Does exact comparison.
  inline auto operator==(const BaseComplex& other) const -> bool {
    return real == other.real && imag == other.imag;
  }
  inline auto operator!=(const BaseComplex& other) const -> bool {
    return !(*this == other);
  }

  // Negation.
  inline auto operator-() const -> BaseComplex { return {-real, -imag}; }

  // Mathematical operation between complex value and a scalar.

  inline auto operator+=(const T& rhs) -> BaseComplex {
    *this = *this + rhs;
    return *this;
  }

  inline auto operator-=(const T& rhs) -> BaseComplex {
    *this = *this - rhs;
    return *this;
  }

  inline auto operator*=(const T& rhs) -> BaseComplex {
    *this = *this * rhs;
    return *this;
  }

  inline auto operator/=(const T& rhs) -> BaseComplex {
    *this = *this / rhs;
    return *this;
  }

  friend inline auto operator+(const BaseComplex& lhs, const T& rhs)
      -> BaseComplex {
    return {lhs.real + rhs, lhs.imag};
  }

  friend inline auto operator-(const BaseComplex& lhs, const T& rhs)
      -> BaseComplex {
    return {lhs.real - rhs, lhs.imag};
  }

  friend inline auto operator*(const BaseComplex& lhs, const T& rhs)
      -> BaseComplex {
    return {lhs.real * rhs, lhs.imag * rhs};
  }
  friend inline auto operator*(const T& lhs, const BaseComplex& rhs)
      -> BaseComplex {
    return {lhs * rhs.real, lhs * rhs.imag};
  }

  friend inline auto operator/(const BaseComplex& lhs, const T& rhs)
      -> BaseComplex {
    const T rhs_inv = T(1) / rhs;
    return {lhs.real * rhs_inv, lhs.imag * rhs_inv};
  }

  // Mathematical operation between two complex values.

  inline auto operator+=(const BaseComplex& rhs) -> BaseComplex& {
    *this = *this + rhs;
    return *this;
  }

  inline auto operator-=(const BaseComplex& rhs) -> BaseComplex& {
    *this = *this - rhs;
    return *this;
  }

  inline auto operator*=(const BaseComplex& rhs) -> BaseComplex& {
    *this = *this * rhs;
    return *this;
  }

  inline auto operator/=(const BaseComplex& rhs) -> BaseComplex& {
    *this = *this / rhs;
    return *this;
  }

  friend inline auto operator+(const BaseComplex& lhs, const BaseComplex& rhs)
      -> BaseComplex {
    return {lhs.real + rhs.real, lhs.imag + rhs.imag};
  }

  friend inline auto operator-(const BaseComplex& lhs, const BaseComplex& rhs)
      -> BaseComplex {
    return {lhs.real - rhs.real, lhs.imag - rhs.imag};
  }

  friend inline auto operator*(const BaseComplex& lhs, const BaseComplex& rhs)
      -> BaseComplex {
    const T a = lhs.real;
    const T b = lhs.imag;
    const T c = rhs.real;
    const T d = rhs.imag;
    const T ac = a * c;
    const T bd = b * d;
    const T ad = a * d;
    const T bc = b * c;
    return {ac - bd, ad + bc};
  }

  friend inline auto operator/(const BaseComplex& lhs, const BaseComplex& rhs)
      -> BaseComplex {
    const T a = lhs.real;
    const T b = lhs.imag;
    const T c = rhs.real;
    const T d = rhs.imag;

    const T ac = a * c;
    const T bd = b * d;
    const T ad = a * d;
    const T bc = b * c;

    const T c2 = c * c;
    const T d2 = d * d;
    const T den = c2 + d2;
    const T den_inv = T(1) / den;

    return {(ac + bd) * den_inv, (bc - ad) * den_inv};
  }

  T real, imag;
};

template <class T>
inline auto operator<<(std::ostream& os, const BaseComplex<T> complex)
    -> std::ostream& {
  if (complex.real == T(0) && complex.imag == T(0)) {
    os << '0';
    return os;
  }

  if (complex.real != T(0)) {
    os << complex.real;
  }

  if (complex.imag == T(0)) {
    return os;
  }

  if (complex.imag > T(0)) {
    os << '+';
  }

  os << complex.imag << 'j';

  return os;
}

// Determines if the given complex number has finite real and imaginary parts
// (i.e. both of them are not infinite or NaN).
//
// This function is safe for fast-math optimization.
template <class T>
inline auto IsFinite(const BaseComplex<T>& a) -> bool {
  return IsFinite(a.real) && IsFinite(a.imag);
}

// Determines if the given complex number is a not-a-number (NaN) value.
// Complex value considered to be a not-a-number if any of the real and
// imaginary parts is not-a-number.
template <class T>
inline auto IsNaN(const BaseComplex<T>& a) -> bool {
  return IsNaN(a.real) || IsNaN(a.imag);
}

// Calculate the squared magnitude of a complex number.
template <class T>
inline auto Norm(const BaseComplex<T>& a) -> T {
  return a.imag * a.imag + a.real * a.real;
}

// Calculate the magnitude of a complex number.
template <class T>
inline auto Abs(const BaseComplex<T>& a) -> T {
  return Sqrt(Norm(a));
}

// Possibly faster implementation of magnitude calculation trading off some
// precision.
template <class T>
inline auto FastAbs(const BaseComplex<T>& a) -> T {
  return Abs(a);
}

// Calculates the phase angle (in radians) of the complex number.
// `FastArg(a)` is equivalent to `FastArcTan2(a.imag, a.real)`
template <class T>
inline auto FastArg(const BaseComplex<T>& a) -> T {
  return FastArcTan2(a.imag, a.real);
}

}  // namespace radio_core
