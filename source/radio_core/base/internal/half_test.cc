// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/half.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

#if RADIO_CORE_HAVE_HALF

////////////////////////////////////////////////////////////////////////////////
// Constructors and assignment.

TEST(Half, Construct) {
  EXPECT_EQ(Half(1), Half(1.0f));
  EXPECT_EQ(Half(1.2f), Half(1.2f));
  EXPECT_NE(Half(1.2f), Half(3.4f));
}

TEST(Half, Assign) {
  Half a(1.2f);
  a = Half(2.3f);
  EXPECT_EQ(a, Half(2.3f));
}

TEST(Half, Access) { EXPECT_NEAR(float(Half(1.2f)), 1.2001953125f, 1e-6f); }

////////////////////////////////////////////////////////////////////////////////
// Negation.

TEST(Half, Negate) {
  EXPECT_EQ(-Half(1.2f), Half(-1.2f));
  EXPECT_EQ(-Half(-1.2f), Half(1.2f));
}

////////////////////////////////////////////////////////////////////////////////
// Unary operations.

TEST(Half, UnaryAdd) {
  {
    Half a(1.2f);
    a += Half(3.4f);
    EXPECT_NEAR(float(a), 4.6015625f, 1e-6f);
  }
}

TEST(Half, UnarySubtract) {
  {
    Half a(4.6f);
    a -= Half(3.4f);
    EXPECT_NEAR(float(a), 1.201171875f, 1e-6f);
  }
}

TEST(Half, UnaryMultiply) {
  {
    Half a(4.6f);
    a *= Half(3.4f);
    EXPECT_NEAR(float(a), 15.6484375f, 1e-6f);
  }
}

TEST(Half, UnaryDivide) {
  {
    Half a(4.6f);
    a /= Half(1.2f);
    EXPECT_NEAR(float(a), 3.833984375f, 1e-6f);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Comparison.

TEST(Half, Compare) {
  EXPECT_TRUE(Half(0.0f) == Half(0.0f));
  EXPECT_TRUE(Half(1.0f) != Half(0.0f));

  EXPECT_FALSE(Half(0.0f) != Half(0.0f));
  EXPECT_FALSE(Half(1.0f) == Half(0.0f));

  EXPECT_TRUE(Half(0.0f) < Half(10.0f));
  EXPECT_TRUE(Half(0.0f) <= Half(10.0f));
  EXPECT_FALSE(Half(10.0f) < Half(0.0f));
  EXPECT_FALSE(Half(10.0f) <= Half(0.0f));

  EXPECT_TRUE(Half(10.0f) > Half(0.0f));
  EXPECT_TRUE(Half(10.0f) >= Half(0.0f));
  EXPECT_FALSE(Half(0.0f) > Half(10.0f));
  EXPECT_FALSE(Half(0.0f) >= Half(10.0f));
}

////////////////////////////////////////////////////////////////////////////////
// Binary operations.

TEST(Half, BinaryAdd) {
  EXPECT_NEAR(float(Half(1.2f) + Half(3.4f)), 4.6015625f, 1e-6f);
}

TEST(Half, BinarySubtract) {
  EXPECT_NEAR(float(Half(4.6f) - Half(3.4f)), 1.201171875f, 1e-6f);
}

TEST(Half, BinaryMultiply) {
  EXPECT_NEAR(float(Half(4.6f) * Half(3.4f)), 15.6484375f, 1e-6f);
}

TEST(Half, BinaryDivide) {
  EXPECT_NEAR(float(Half(4.6f) / Half(1.2f)), 3.833984375f, 1e-6f);
}

TEST(Half, HalfAsUInt16) {
  EXPECT_EQ(HalfAsUInt16(Half(0.0f)), 0);
  EXPECT_EQ(HalfAsUInt16(Half(12.3f)), 0x4a26);
  EXPECT_EQ(HalfAsUInt16(Half(-12.3f)), 0xca26);
}

TEST(Half, UInt16AsHalf) {
  EXPECT_EQ(UInt16AsHalf(0), Half(0.0f));
  EXPECT_EQ(UInt16AsHalf(0x4a26), Half(12.3f));
  EXPECT_EQ(UInt16AsHalf(0xca26), Half(-12.3f));
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
