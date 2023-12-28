// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace radio_core {

namespace byte_internal {

template <std::size_t N, std::size_t... I>
constexpr auto ToBytesArrayImpl(const uint8_t (&a)[N],
                                std::index_sequence<I...> /*sequence*/)
    -> std::array<std::byte, N> {
  return {{std::byte{a[I]}...}};
}

}  // namespace byte_internal

// Construct std::array from the given sequence of integral values.
template <std::size_t N>
constexpr auto ToBytesArray(const uint8_t (&a)[N]) -> std::array<std::byte, N> {
  return byte_internal::ToBytesArrayImpl(a, std::make_index_sequence<N>{});
}

}  // namespace radio_core
