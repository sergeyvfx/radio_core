// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/unittest/vectorized_matchers.h"

#include "radio_core/math/float4.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/mock_unittest.h"
#include "radio_core/unittest/test.h"

namespace radio_core::testing {

TEST(VectorizedNear, VectorizedNearMatches) {
  // Check behavior around zero expected value, with the zero epsilon.
  {
    Matcher<Float4> matcher = VectorizedNear(Float4(0.0f), 0.0f);
    EXPECT_TRUE(matcher.Matches(Float4(0.0f)));
    EXPECT_TRUE(matcher.Matches(Float4(-0.0f)));
    EXPECT_FALSE(matcher.Matches(Float4(1e-18f)));
    EXPECT_FALSE(matcher.Matches(Float4(-1e-18f)));
    EXPECT_FALSE(matcher.Matches(Float4(1.0f)));
  }

  // Check behavior around zero expected value, with the high epsilon.
  {
    Matcher<Float4> matcher = VectorizedNear(Float4(0.0f), 1.0f);
    EXPECT_TRUE(matcher.Matches(Float4(0.0f)));
    EXPECT_TRUE(matcher.Matches(Float4(-0.0f)));
    EXPECT_TRUE(matcher.Matches(Float4(1.0f)));
    EXPECT_TRUE(matcher.Matches(Float4(-1.0f)));
    EXPECT_FALSE(matcher.Matches(Float4(1.00001f)));
    EXPECT_FALSE(matcher.Matches(Float4(-1.00001f)));
  }

  // Check more realistic behavior.
  {
    Matcher<Float4> matcher = VectorizedNear(Float4(1, 2, 3, 4), 1e-6f);
    EXPECT_TRUE(matcher.Matches(Float4(1, 2, 3, 4)));
    EXPECT_FALSE(matcher.Matches(Float4(-1, -2, -3, -4)));
    EXPECT_FALSE(matcher.Matches(
        Float4(1.0000011f, 2.0000011f, 3.0000011f, 4.0000011f)));
    EXPECT_TRUE(
        matcher.Matches(Float4(1.000001f, 2.000001f, 3.000001f, 4.000001f)));
  }
}

TEST(VectorizedNear, VectorizedNearCanDescribeSelf) {
  {
    Matcher<Float4> matcher = VectorizedNear(Float4(1, 2, 3, 4), 0.5f);
    EXPECT_EQ("is approximately {1, 2, 3, 4} (absolute error <= 0.5)",
              Describe(matcher));
    EXPECT_EQ("isn't approximately {1, 2, 3, 4} (absolute error > 0.5)",
              DescribeNegation(matcher));
  }
}

TEST(VectorizedNear2, VectorizedNear2Matches) {
  typedef std::tuple<Float4, Float4> Tuple;

  // Check behavior around zero expected value, with the zero epsilon.
  {
    Matcher<const Tuple&> matcher = VectorizedNear(0.0f);
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(0.0f), Float4(0.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(0.0f), Float4(-0.0f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Float4(0.0f), Float4(1e-18f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Float4(0.0f), Float4(-1e-18f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Float4(0.0f), Float4(1.0f))));
  }

  // Check behavior around zero expected value, with the high epsilon.
  {
    Matcher<const Tuple&> matcher = VectorizedNear(1.0f);
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(0.0f), Float4(0.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(0.0f), Float4(-0.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(0.0f), Float4(1.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(0.0f), Float4(-1.0f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Float4(0.0f), Float4(1.00001f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Float4(0.0f), Float4(-1.00001f))));
  }

  // Check more realistic behavior.
  {
    Matcher<const Tuple&> matcher = VectorizedNear(1e-6f);
    EXPECT_TRUE(matcher.Matches(Tuple(Float4(1, 2, 3, 4), Float4(1, 2, 3, 4))));
    EXPECT_FALSE(
        matcher.Matches(Tuple(Float4(1, 2, 3, 4), Float4(-1, -2, -3, -4))));
    EXPECT_FALSE(matcher.Matches(
        Tuple(Float4(1, 2, 3, 4),
              Float4(1.0000011f, 2.0000011f, 3.0000011f, 4.0000011f))));
    EXPECT_TRUE(matcher.Matches(
        Tuple(Float4(1, 2, 3, 4),
              Float4(1.000001f, 2.000001f, 3.000001f, 4.000001f))));
  }
}

TEST(VectorizedNear2, VectorizedNear2CanDescribeSelf) {
  {
    typedef std::tuple<Float4, Float4> Tuple;
    Matcher<const Tuple&> matcher = VectorizedNear(0.5f);
    EXPECT_EQ("are an almost-equal pair", Describe(matcher));
    EXPECT_EQ("aren't an almost-equal pair", DescribeNegation(matcher));
  }
}

}  // namespace radio_core::testing
