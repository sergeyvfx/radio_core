// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/bitwise.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(bitwise, Select) {
  EXPECT_EQ(Select<uint32_t>(0, 0xdeadbeef, 0), 0);
  EXPECT_EQ(Select<uint32_t>(0xffffffff, 0xdeadbeef, 0), 0xdeadbeef);
  EXPECT_EQ(Select<uint32_t>(0x55555555, 0xdeadbeef, 0x8badf00d), 0xdeadb44d);
}

}  // namespace radio_core
