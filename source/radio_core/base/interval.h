// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// A bounded interval of of real-like numbers.

#pragma once

#include <ostream>

#include "radio_core/base/algorithm.h"

namespace radio_core {

template <class T>
class Interval {
 public:
  // Create new empty interval.
  // The empty interval contains no points.
  Interval() = default;

  // Create interval which lower and upper boundary are equal to the given
  // value.
  inline constexpr explicit Interval(const T value)
      : lower_bound(value), upper_bound(value) {}

  // Create interval with the given lower and upper bounds.
  inline constexpr Interval(const T new_lower_bound, const T new_upper_bound)
      : lower_bound(new_lower_bound), upper_bound(new_upper_bound) {}

  Interval(const Interval& other) = default;
  Interval(Interval&& other) noexcept = default;

  auto operator=(const Interval& other) -> Interval& = default;
  auto operator=(Interval&& other) -> Interval& = default;

  // Comparison.
  inline auto operator==(const Interval& other) const -> bool {
    return lower_bound == other.lower_bound && upper_bound == other.upper_bound;
  }
  inline auto operator!=(const Interval& other) const -> bool {
    return !(*this == other);
  }

  // Check whether the interval is empty.
  // The empty interval contains no points.
  inline constexpr auto IsEmpty() const -> bool {
    return lower_bound > upper_bound;
  }

  // Check whether the given value belongs to this interval.
  //
  // If the value equals to one of the bounds then the value is considered to be
  // in the interval.
  inline constexpr auto Contains(const T value) const -> bool {
    return value >= lower_bound && value <= upper_bound;
  }

  // Construct new interval from an intersection of this interval with the other
  // one.
  inline constexpr auto Intersection(const Interval& other) const -> Interval {
    return Interval(Max(lower_bound, other.lower_bound),
                    Min(upper_bound, other.upper_bound));
  }

  // Returns true if this interval intersects the other one.
  inline constexpr auto Intersects(const Interval& other) const -> bool {
    return !Intersection(other).IsEmpty();
  }

  // Construct new interval with expanded bounds.
  // Moves the lower boundary lower and the upper bound u[ by the given extent.
  inline constexpr auto Expanded(const T extent) const -> Interval {
    if (IsEmpty()) {
      return *this;
    }
    return Interval(lower_bound - extent, upper_bound + extent);
  }

  // A diameter of the interval, calculated as a difference between
  // upper and lower end points.
  // If the interval is empty the result is undefined.
  inline constexpr auto Diameter() const -> T {
    return upper_bound - lower_bound;
  }

  // Lower bound higher than the upper one has a special meaning of an empty
  // interval.
  T lower_bound{1};
  T upper_bound{0};
};

template <class T>
inline auto operator<<(std::ostream& os, const Interval<T>& interval)
    -> std::ostream& {
  if (interval.IsEmpty()) {
    os << "[empty interval]";
  } else {
    os << '[' << interval.lower_bound << " .. " << interval.upper_bound << "]";
  }
  return os;
}

}  // namespace radio_core
