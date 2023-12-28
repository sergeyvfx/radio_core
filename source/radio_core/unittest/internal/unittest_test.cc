// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/test.h"

#include <array>
#include <span>
#include <sstream>

namespace radio_core::testing {

TEST(test, PrintSpan) {
  const std::array array = std::to_array<int>({1, 2, 3, 4});
  const std::span<const int> span{array};

  std::stringstream stream;
  testing::internal::PrintTo<std::span<const int>>(span, &stream);

  EXPECT_EQ(stream.str(), "{ 1, 2, 3, 4 }");
}

}  // namespace radio_core::testing
