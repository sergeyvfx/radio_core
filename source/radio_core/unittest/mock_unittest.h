// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Utilities to unit-test the mocking functionality.

#pragma once

#include "radio_core/unittest/mock.h"

namespace radio_core::testing {

// Returns the description of the given matcher.
template <typename T>
auto Describe(const Matcher<T>& m) -> std::string {
  return DescribeMatcher<T>(m);
}

// Returns the description of the negation of the given matcher.
template <typename T>
auto DescribeNegation(const Matcher<T>& m) -> std::string {
  return DescribeMatcher<T>(m, true);
}

}  // namespace radio_core::testing
