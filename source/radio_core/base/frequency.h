// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Frequency value which precision and range is closely matched on all supported
// platforms.
//
// The frequency is measured in Hertz and has support for fractional values.
// The value is signed, allowing to represent negative frequencies.
//
// Construction of a frequency object from an integral and floating point value
// is implicit, reducing the boiler plate code. This is because the accuracy of
// the frequency is expected to be equal or better than the scalar types.
//
// Casting frequency to other types is explicit. This is because it is not
// possible to ensure that the cast is always lossless, including possible
// difference in platform-specific implementations, and it is undesirable to
// have different behavior on different platforms.
//
// The intended use-case for the frequency is places where a large range of
// frequencies is to be represented. For example, to store tuned frequency of
// a radio which can be in the range of almost DC up to 10s of gigahertz.
// It is not intended to be used in mathematical calculations as the performance
// is not the guaranteed part of the implementation.

#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>

namespace radio_core {

namespace frequency_internal {

template <class T>
using EnableIfImplicitlyConstructibleFrom = std::enable_if_t<
    std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>>;

}  // namespace frequency_internal

class Frequency {
 public:
  Frequency() = default;

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr Frequency(const T value) : value_(double(value)) {}

  Frequency(const Frequency& other) = default;
  Frequency(Frequency&& other) noexcept = default;

  ~Frequency() = default;

  // Assignment.

  constexpr auto operator=(const Frequency& other) -> Frequency& = default;
  constexpr auto operator=(Frequency&& other) -> Frequency& = default;

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator=(const T rhs) -> Frequency& {
    value_ = rhs;
    return *this;
  }

  // Comparison.

  // Seems ot be a quirk in Clang-Tidy which suggests to nullptr in place of
  // default.
  // NOLINTNEXTLINE(modernize-use-nullptr)
  constexpr auto operator<=>(const Frequency& rhs) const = default;

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator<=>(const T rhs) const {
    return value_ <=> rhs;
  }

  // Conversion.

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr explicit operator T() const {
    return value_;
  }

  template <class T, class = std::enable_if_t<std::is_floating_point_v<T>>>
  constexpr auto ToFloatingPoint() const -> T {
    return T(value_);
  }

  template <class T, class = std::enable_if_t<std::is_integral_v<T>>>
  constexpr auto ToInteger() const -> T {
    return T(value_);
  }

  // Negation.

  constexpr auto operator-() const -> Frequency { return {-value_}; }

  // Mathematic operations.

  constexpr auto operator+(const Frequency& rhs) const -> Frequency {
    return {value_ + rhs.value_};
  }
  constexpr auto operator-(const Frequency& rhs) const -> Frequency {
    return {value_ - rhs.value_};
  }
  constexpr auto operator*(const Frequency& rhs) const -> Frequency {
    return {value_ * rhs.value_};
  }
  constexpr auto operator/(const Frequency& rhs) const -> Frequency {
    return {value_ / rhs.value_};
  }

  constexpr auto operator+=(const Frequency& rhs) -> Frequency& {
    value_ += rhs.value_;
    return *this;
  }
  constexpr auto operator-=(const Frequency& rhs) -> Frequency& {
    value_ -= rhs.value_;
    return *this;
  }
  constexpr auto operator*=(const Frequency& rhs) -> Frequency& {
    value_ *= rhs.value_;
    return *this;
  }
  constexpr auto operator/=(const Frequency& rhs) -> Frequency& {
    value_ /= rhs.value_;
    return *this;
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator+(const T rhs) const -> Frequency {
    return {value_ + rhs};
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator-(const T rhs) const -> Frequency {
    return Frequency(value_ - rhs);
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator*(const T rhs) const -> Frequency {
    return Frequency(value_ * rhs);
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator/(const T rhs) const -> Frequency {
    return Frequency(value_ / rhs);
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator+=(const T rhs) -> Frequency& {
    value_ += rhs;
    return *this;
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator-=(const T rhs) -> Frequency& {
    value_ -= rhs;
    return *this;
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator*=(const T rhs) -> Frequency& {
    value_ *= rhs;
    return *this;
  }

  template <class T,
            class = frequency_internal::EnableIfImplicitlyConstructibleFrom<T>>
  constexpr auto operator/=(const T rhs) -> Frequency& {
    value_ /= rhs;
    return *this;
  }

  // Printing.

  friend auto operator<<(std::ostream& os, const Frequency frequency)
      -> std::ostream& {
    os << frequency.value_;
    return os;
  }

  // Mathematical operations.

  friend inline auto Round(const Frequency& arg) -> Frequency {
    return std::round(arg.value_);
  }

  // Computes the floating-point remainder of the division operation.
  friend inline auto Modulo(const Frequency x, const Frequency y) -> Frequency {
    return std::fmod(x.value_, y.value_);
  }

 private:
  double value_ = 0;
};

// Convert value from kHz, MHz, and GHz to Hz.
//
// Example:
//  Frequency my_freq = 145.8_MHz;

constexpr auto operator"" _Hz(const unsigned long long int hz) -> Frequency {
  return {hz};
}
constexpr auto operator"" _Hz(const long double hz) -> Frequency {
  return {hz};
}

constexpr auto operator"" _kHz(const unsigned long long int khz) -> Frequency {
  return {khz * 1000};
}
constexpr auto operator"" _kHz(const long double khz) -> Frequency {
  return {khz * 1000};
}

constexpr auto operator"" _MHz(const unsigned long long int mhz) -> Frequency {
  return {mhz * 1000000};
}
constexpr auto operator"" _MHz(const long double mhz) -> Frequency {
  return {mhz * 1000000};
}

constexpr auto operator"" _GHz(const unsigned long long int ghz) -> Frequency {
  return {ghz * 1000000000};
}
constexpr auto operator"" _GHz(const long double ghz) -> Frequency {
  return {ghz * 1000000000};
}

}  // namespace radio_core
