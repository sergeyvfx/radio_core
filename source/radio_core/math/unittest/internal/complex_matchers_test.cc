// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/unittest/complex_matchers.h"

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/mock_unittest.h"
#include "radio_core/unittest/test.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::testing {

TEST(ComplexNear, ComplexNearMatches) {
  // Check behavior around zero expected value, with the zero epsilon.
  {
    Matcher<Complex> matcher = ComplexNear(Complex(0.0f), 0.0f);
    EXPECT_TRUE(matcher.Matches(Complex(0.0f)));
    EXPECT_TRUE(matcher.Matches(Complex(-0.0f)));
    EXPECT_FALSE(matcher.Matches(Complex(1e-18f)));
    EXPECT_FALSE(matcher.Matches(Complex(-1e-18f)));
    EXPECT_FALSE(matcher.Matches(Complex(1.0f)));
  }

  // Check behavior around zero expected value, with the high epsilon.
  {
    Matcher<Complex> matcher = ComplexNear(Complex(0.0f), 1.0f);
    EXPECT_TRUE(matcher.Matches(Complex(0.0f)));
    EXPECT_TRUE(matcher.Matches(Complex(-0.0f)));
    EXPECT_TRUE(matcher.Matches(Complex(1.0f)));
    EXPECT_TRUE(matcher.Matches(Complex(-1.0f)));
    EXPECT_FALSE(matcher.Matches(Complex(1.00001f)));
    EXPECT_FALSE(matcher.Matches(Complex(-1.00001f)));
  }

  // Check more realistic behavior.
  {
    Matcher<Complex> matcher = ComplexNear(Complex(1.0f, 2.0f), 1e-6f);
    EXPECT_TRUE(matcher.Matches(Complex(1.0f, 2.0f)));
    EXPECT_FALSE(matcher.Matches(Complex(-1.0f, -2.0f)));
    EXPECT_FALSE(matcher.Matches(Complex(1.0000011f, 2.0000011f)));
    EXPECT_TRUE(matcher.Matches(Complex(1.000001f, 2.000001f)));
  }
}

TEST(ComplexNear, ComplexNearCanDescribeSelf) {
  {
    Matcher<Complex> matcher = ComplexNear(Complex(1.0f, 2.0f), 0.5f);
    EXPECT_EQ("is approximately 1+2j (absolute error <= 0.5)",
              Describe(matcher));
    EXPECT_EQ("isn't approximately 1+2j (absolute error > 0.5)",
              DescribeNegation(matcher));
  }
}

TEST(ComplexNear2, ComplexNear2Matches) {
  typedef std::tuple<Complex, Complex> Tuple;

  // Check behavior around zero expected value, with the zero epsilon.
  {
    Matcher<const Tuple&> matcher = ComplexNear(0.0f);
    EXPECT_TRUE(matcher.Matches(Tuple(Complex(0.0f), Complex(0.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Complex(0.0f), Complex(-0.0f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Complex(0.0f), Complex(1e-18f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Complex(0.0f), Complex(-1e-18f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Complex(0.0f), Complex(1.0f))));
  }

  // Check behavior around zero expected value, with the high epsilon.
  {
    Matcher<const Tuple&> matcher = ComplexNear(1.0f);
    EXPECT_TRUE(matcher.Matches(Tuple(Complex(0.0f), Complex(0.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Complex(0.0f), Complex(-0.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Complex(0.0f), Complex(1.0f))));
    EXPECT_TRUE(matcher.Matches(Tuple(Complex(0.0f), Complex(-1.0f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Complex(0.0f), Complex(1.00001f))));
    EXPECT_FALSE(matcher.Matches(Tuple(Complex(0.0f), Complex(-1.00001f))));
  }

  // Check more realistic behavior.
  {
    Matcher<const Tuple&> matcher = ComplexNear(1e-6f);
    EXPECT_TRUE(
        matcher.Matches(Tuple(Complex(1.0f, 2.0f), Complex(1.0f, 2.0f))));
    EXPECT_FALSE(
        matcher.Matches(Tuple(Complex(1.0f, 2.0f), Complex(-1.0f, -2.0f))));
    EXPECT_FALSE(matcher.Matches(
        Tuple(Complex(1.0f, 2.0f), Complex(1.0000011f, 2.0000011f))));
    EXPECT_TRUE(matcher.Matches(
        Tuple(Complex(1.0f, 2.0f), Complex(1.000001f, 2.000001f))));
  }

#if RADIO_CORE_HAVE_HALF
  typedef std::tuple<HalfComplex, HalfComplex> HalfTuple;

  // Matcher of half floats.
  {
    Matcher<const HalfTuple&> matcher = ComplexNear(1e-6f);
    EXPECT_TRUE(
        matcher.Matches(HalfTuple(HalfComplex(0.0f), HalfComplex(0.0f))));
  }
#endif
}

TEST(ComplexNear2, ComplexNear2CanDescribeSelf) {
  typedef std::tuple<Complex, Complex> Tuple;

  {
    Matcher<const Tuple&> matcher = ComplexNear(0.5f);
    EXPECT_EQ("are an almost-equal pair", Describe(matcher));
    EXPECT_EQ("aren't an almost-equal pair", DescribeNegation(matcher));
  }
}

}  // namespace radio_core::testing
