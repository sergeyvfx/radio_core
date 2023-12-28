// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Utilities for polyphase filter, such as polyphase decomposition.

#pragma once

#include <cassert>
#include <cstdlib>
#include <span>

namespace radio_core::signal {

// Calculate size of polyphase components of the decomposition of the filter of
// the given size (size = order + 1).
inline constexpr auto PolyphaseDecompositionSize(const size_t filter_size,
                                                 const int num_components)
    -> size_t {
  return filter_size / num_components;
}

// Decompose a polynormal component l of the impulse response h into e_l.
// The total number of polyphase components in the decomposition is N.
//
// Reference:
//   Polyphase decomposition
//   https://www.dsprelated.com/freebooks/sasp/Polyphase_Decomposition.html
template <class T>
inline constexpr void PolyphaseComponentDecomposition(
    const std::span<const T> h,
    const int N,
    const size_t l,
    const std::span<T> e_l) {
  assert(e_l.size() == PolyphaseDecompositionSize(h.size(), N));

  const size_t component_size = e_l.size();
  for (int n = 0; n < component_size; ++n) {
    const size_t index = N * n + l;
    assert(index < h.size());
    e_l[n] = h[index];
  }
}

}  // namespace radio_core::signal
