// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/complex.h"

#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;

TEST(Complex, compare) {
  EXPECT_TRUE(Complex(0.1f, 0.2f) == Complex(0.1f, 0.2f));
  EXPECT_FALSE(Complex(0.1f, 0.2f) == Complex(0.4f, 0.3f));

  EXPECT_FALSE(Complex(0.1f, 0.2f) != Complex(0.1f, 0.2f));
  EXPECT_TRUE(Complex(0.1f, 0.2f) != Complex(0.4f, 0.3f));
}

TEST(Complex, unary_minus) {
  EXPECT_THAT(-Complex(2, 3), ComplexNear(Complex(-2, -3), 1e-6f));
}

////////////////////////////////////////////////////////////////////////////////
// Mathematical operation between complex value and a scalar.

TEST(Complex, add_scalar) {
  // operator+
  EXPECT_THAT(Complex(2, 3) + 4, ComplexNear(Complex(6, 3), 1e-6f));

  // operator+=
  {
    Complex complex(2, 3);
    complex += 4;
    EXPECT_THAT(complex, ComplexNear(Complex(6, 3), 1e-6f));
  }
}

TEST(Complex, sub_scalar) {
  // operator-
  EXPECT_THAT(Complex(2, 3) - 4, ComplexNear(Complex(-2, 3), 1e-6f));

  // operator-=
  {
    Complex complex(2, 3);
    complex -= 4;
    EXPECT_THAT(complex, ComplexNear(Complex(-2, 3), 1e-6f));
  }
}

TEST(Complex, multiply_scalar) {
  // operator*
  EXPECT_THAT(Complex(2, 3) * 4, ComplexNear(Complex(8, 12), 1e-6f));
  EXPECT_THAT(4 * Complex(2, 3), ComplexNear(Complex(8, 12), 1e-6f));

  // operator*=
  {
    Complex complex(2, 3);
    complex *= 4;
    EXPECT_THAT(complex, ComplexNear(Complex(8, 12), 1e-6f));
  }
}

TEST(Complex, divide_scalar) {
  // operator/
  EXPECT_THAT(Complex(8, 12) / 4, ComplexNear(Complex(2, 3), 1e-6f));

  // operator/=
  {
    Complex complex(8, 12);
    complex /= 4;
    EXPECT_THAT(complex, ComplexNear(Complex(2, 3), 1e-6f));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Mathematical operation between two complex values.

TEST(Complex, add_complex) {
  // operator+
  EXPECT_THAT(Complex(2, 3) + Complex(7, 4), ComplexNear(Complex(9, 7), 1e-6f));

  // operator+=
  {
    Complex complex(2, 3);
    complex += Complex(7, 4);
    EXPECT_THAT(complex, ComplexNear(Complex(9, 7), 1e-6f));
  }
}

TEST(Complex, sub_complex) {
  // operator-
  EXPECT_THAT(Complex(2, 3) - Complex(7, 4),
              ComplexNear(Complex(-5, -1), 1e-6f));

  // operator-=
  {
    Complex complex(2, 3);
    complex -= Complex(7, 4);
    EXPECT_THAT(complex, ComplexNear(Complex(-5, -1), 1e-6f));
  }
}

TEST(Complex, multiply_complex) {
  // operator*
  EXPECT_THAT(Complex(2, 3) * Complex(7, 4),
              ComplexNear(Complex(2, 29), 1e-6f));

  // operator*=
  {
    Complex complex(2, 3);
    complex *= Complex(7, 4);
    EXPECT_THAT(complex, ComplexNear(Complex(2, 29), 1e-6f));
  }
}

TEST(Complex, divide_complex) {
  // operator*
  EXPECT_THAT(Complex(2, 3) / Complex(7, 4),
              ComplexNear(Complex(0.4f, 0.2f), 1e-6f));

  // operator*=
  {
    Complex complex(2, 3);
    complex /= Complex(7, 4);
    EXPECT_THAT(complex, ComplexNear(Complex(0.4f, 0.2f), 1e-6f));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Non-class functions.

TEST(Complex, MultiplyAdd) {
  const Complex a(2, 3);
  const Complex b(7, 4);
  const float c = 9;

  EXPECT_THAT(MultiplyAdd(a, b, c), ComplexNear(Complex(65, 39), 1e-6f));

  EXPECT_THAT(MultiplyAdd(a, c, b), ComplexNear(Complex(65, 39), 1e-6f));
}

TEST(Complex, Norm) {
  EXPECT_NEAR(Norm(Complex(0)), 0.0f, 1e-6f);

  EXPECT_NEAR(Norm(Complex(0.1f)), 0.01f, 1e-6f);

  EXPECT_NEAR(Norm(Complex(0.1f, 2.0f)), 4.01f, 1e-6f);
}

TEST(Complex, Abs) {
  EXPECT_NEAR(Abs(Complex(0)), 0.0f, 1e-6f);

  EXPECT_NEAR(Abs(Complex(0.1f)), 0.1f, 1e-6f);

  EXPECT_NEAR(Abs(Complex(2.0f, 3.0f)), 3.6055513f, 1e-6f);
}

TEST(Complex, FastAbs) {
  EXPECT_NEAR(FastAbs(Complex(0)), 0.0f, 1e-6f);

  EXPECT_NEAR(FastAbs(Complex(0.1f)), 0.1f, 1e-6f);

  EXPECT_NEAR(FastAbs(Complex(2.0f, 3.0f)), 3.6055513f, 1e-6f);
}

TEST(Complex, FastArg) {
  EXPECT_NEAR(FastArg(Complex(1.0f, 0.0f)), 0.0f, 0.005f);
  EXPECT_NEAR(FastArg(Complex(0.0f, 1.0f)), 1.57079637f, 0.005f);
  EXPECT_NEAR(FastArg(Complex(-3.0f, 2.0f)), 2.55359006f, 0.005f);
  EXPECT_NEAR(FastArg(Complex(3.0f, -2.0f)), -0.58800262f, 0.005f);
}

}  // namespace radio_core
