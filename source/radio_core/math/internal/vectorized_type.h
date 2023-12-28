// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Common utilities for vectorized types.

#pragma once

#include <ostream>
#include <utility>

namespace radio_core::vectorized_internal {

template <class VectorizedType, std::size_t... I>
void PrintImpl(std::ostream& os,
               const VectorizedType& value,
               std::index_sequence<I...> /*sequence*/) {
  const char* sep = "";
  os << "{";
  (((os << sep << value.template Extract<I>()), sep = ", "), ...);
  os << "}";
}

template <class VectorizedType>
void Print(std::ostream& os, const VectorizedType& value) {
  PrintImpl(os, value, std::make_index_sequence<VectorizedType::kSize>{});
}

// Evaluates to true if the vectorized type of N elements should have High and
// Low parts.
template <int N>
constexpr auto HasLowAndHighParts() -> bool {
  return (N >= 4) && ((N & (N - 1)) == 0);
}

}  // namespace radio_core::vectorized_internal
