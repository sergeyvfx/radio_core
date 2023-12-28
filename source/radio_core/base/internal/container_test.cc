// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/container.h"

#include <vector>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(base_byte_utils, ToBytesArray) {
  {
    std::vector<int> vec;

    EnsureSizeAtLeast(vec, 100);
    EXPECT_EQ(vec.size(), 100);

    EnsureSizeAtLeast(vec, 10);
    EXPECT_EQ(vec.size(), 100);
  }
}

}  // namespace radio_core
