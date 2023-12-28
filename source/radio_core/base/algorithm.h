// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core {

template <class T>
constexpr auto Min(const T& a, const T& b) -> const T& {
  return (b < a) ? b : a;
}

template <class T>
constexpr auto Max(const T& a, const T& b) -> const T& {
  return (a < b) ? b : a;
}

}  // namespace radio_core
