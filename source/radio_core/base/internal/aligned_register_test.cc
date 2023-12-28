// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/aligned_register.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(AlignedRegister, Alignment) {
  static_assert(alignof(AlignedRegister<char, 3, 16>) == 16);
  static_assert(alignof(AlignedRegister<char, 4, 16>) == 16);

  static_assert(alignof(AlignedRegister<float, 3, 16>) == 16);
  static_assert(alignof(AlignedRegister<float, 4, 16>) == 16);
}

TEST(AlignedRegister, Construct) {
  {
    int data[4] = {1, 2, 3, 4};
    const AlignedRegister<int, 4, 16> r(data);
    EXPECT_EQ(r[0], 1);
    EXPECT_EQ(r[1], 2);
    EXPECT_EQ(r[2], 3);
    EXPECT_EQ(r[3], 4);
  }

  {
    const int data[4] = {1, 2, 3, 4};
    const AlignedRegister<int, 4, 16> r(data);
    EXPECT_EQ(r[0], 1);
    EXPECT_EQ(r[1], 2);
    EXPECT_EQ(r[2], 3);
    EXPECT_EQ(r[3], 4);
  }

  {
    const AlignedRegister<int, 4, 16> r(123);
    EXPECT_EQ(r[0], 123);
    EXPECT_EQ(r[1], 123);
    EXPECT_EQ(r[2], 123);
    EXPECT_EQ(r[3], 123);
  }
}

TEST(AlignedRegister, ElementAccess) {
  {
    const AlignedRegister<int, 4, 16> r(1, 2, 3, 4);
    EXPECT_EQ(r[0], 1);
    EXPECT_EQ(r[1], 2);
    EXPECT_EQ(r[2], 3);
    EXPECT_EQ(r[3], 4);
  }

  {
    AlignedRegister<int, 4, 16> r(1, 2, 3, 4);
    r[0] = 5;
    r[1] = 6;
    r[2] = 7;
    r[3] = 8;
    EXPECT_EQ(r[0], 5);
    EXPECT_EQ(r[1], 6);
    EXPECT_EQ(r[2], 7);
    EXPECT_EQ(r[3], 8);
  }
}

}  // namespace radio_core
