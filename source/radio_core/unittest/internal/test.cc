// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/test.h"

#include <cmath>

#include "gflags/gflags.h"

DECLARE_string(test_srcdir);

namespace testing::internal {

GTEST_API_ auto DoubleNearPredFormat(const char* expr1,
                                     const char* expr2,
                                     const char* abs_error_expr,
                                     const float val1,
                                     const float val2,
                                     const float abs_error)
    -> ::testing::AssertionResult {
  const float diff = fabsf(val1 - val2);
  if (diff <= abs_error) {
    return AssertionSuccess();
  }

  // TODO(wan): do not print the value of an expression if it's
  // already a literal.
  return AssertionFailure()
         << "The difference between " << expr1 << " and " << expr2 << " is "
         << diff << ", which exceeds " << abs_error_expr << ", where\n"
         << expr1 << " evaluates to " << val1 << ",\n"
         << expr2 << " evaluates to " << val2 << ", and\n"
         << abs_error_expr << " evaluates to " << abs_error << ".";
}

}  // namespace testing::internal

namespace radio_core::testing {

auto TestFileAbsolutePath(const std::filesystem::path& filename)
    -> std::filesystem::path {
  return FLAGS_test_srcdir / filename;
}

}  // namespace radio_core::testing
