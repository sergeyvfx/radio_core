// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>

namespace radio_core {

template <class T>
constexpr auto Min(const T& a, const T& b) -> const T& {
  return (b < a) ? b : a;
}

template <class T>
constexpr auto Max(const T& a, const T& b) -> const T& {
  return (a < b) ? b : a;
}

// Shift elements in the container.
//
// Elements that are shifted beyond the bounds of the container are
// re-introduced in the opposite bound of the container. Positive value of shift
// moves elements to the right, negative values moves elements to the left.
//
// This function can be used for any C++ style container, such as std::span.
//
// This function behaves similar to the numpy.roll() and Matlab's circshift()
// with the difference that here it essentially operates on a single axis.
template <class Container>
constexpr auto CircularShift(Container& container, const int shift) {
  if (shift == 0) {
    return;
  }

  // Shift to the right.
  if (shift >= 0) {
    const size_t warped_shift = size_t(shift) % container.size();
    std::rotate(container.rbegin(),
                container.rbegin() + warped_shift,
                container.rend());
    return;
  }

  // Shift to the left.
  const size_t warped_shift = size_t(-shift) % container.size();
  std::rotate(
      container.begin(), container.begin() + warped_shift, container.end());
}

}  // namespace radio_core
