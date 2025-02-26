// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include <ostream>

#include "radio_core/math/half_complex.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/unittest/complex_matchers.h"
#  include "radio_core/unittest/mock.h"
#  include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;

TEST(HalfComplex, compare) {
  EXPECT_TRUE(HalfComplex(0.1f, 0.2f) == HalfComplex(0.1f, 0.2f));
  EXPECT_FALSE(HalfComplex(0.1f, 0.2f) == HalfComplex(0.4f, 0.3f));

  EXPECT_FALSE(HalfComplex(0.1f, 0.2f) != HalfComplex(0.1f, 0.2f));
  EXPECT_TRUE(HalfComplex(0.1f, 0.2f) != HalfComplex(0.4f, 0.3f));
}

TEST(HalfComplex, unary_minus) {
  EXPECT_THAT(-HalfComplex(2, 3),
              ComplexNear(HalfComplex(-2, -3), Half(1e-6f)));
}

////////////////////////////////////////////////////////////////////////////////
// Mathematical operation between complex value and a scalar.

TEST(HalfComplex, add_scalar) {
  // operator+
  EXPECT_THAT(HalfComplex(2, 3) + 4,
              ComplexNear(HalfComplex(6, 3), Half(1e-6f)));

  // operator+=
  {
    HalfComplex complex(2, 3);
    complex += 4;
    EXPECT_THAT(complex, ComplexNear(HalfComplex(6, 3), Half(1e-6f)));
  }
}

TEST(HalfComplex, sub_scalar) {
  // operator-
  EXPECT_THAT(HalfComplex(2, 3) - 4,
              ComplexNear(HalfComplex(-2, 3), Half(1e-6f)));

  // operator-=
  {
    HalfComplex complex(2, 3);
    complex -= 4;
    EXPECT_THAT(complex, ComplexNear(HalfComplex(-2, 3), Half(1e-6f)));
  }
}

TEST(HalfComplex, multiply_scalar) {
  // operator*
  EXPECT_THAT(HalfComplex(2, 3) * 4,
              ComplexNear(HalfComplex(8, 12), Half(1e-6f)));
  EXPECT_THAT(4 * HalfComplex(2, 3),
              ComplexNear(HalfComplex(8, 12), Half(1e-6f)));

  // operator*=
  {
    HalfComplex complex(2, 3);
    complex *= 4;
    EXPECT_THAT(complex, ComplexNear(HalfComplex(8, 12), Half(1e-6f)));
  }
}

TEST(HalfComplex, divide_scalar) {
  // operator/
  EXPECT_THAT(HalfComplex(8, 12) / 4,
              ComplexNear(HalfComplex(2, 3), Half(1e-6f)));

  // operator/=
  {
    HalfComplex complex(8, 12);
    complex /= 4;
    EXPECT_THAT(complex, ComplexNear(HalfComplex(2, 3), Half(1e-6f)));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Mathematical operation between two complex values.

TEST(HalfComplex, add_complex) {
  // operator+
  EXPECT_THAT(HalfComplex(2, 3) + HalfComplex(7, 4),
              ComplexNear(HalfComplex(9, 7), Half(1e-6f)));

  // operator+=
  {
    HalfComplex complex(2, 3);
    complex += HalfComplex(7, 4);
    EXPECT_THAT(complex, ComplexNear(HalfComplex(9, 7), Half(1e-6f)));
  }
}

TEST(HalfComplex, sub_complex) {
  // operator-
  EXPECT_THAT(HalfComplex(2, 3) - HalfComplex(7, 4),
              ComplexNear(HalfComplex(-5, -1), Half(1e-6f)));

  // operator-=
  {
    HalfComplex complex(2, 3);
    complex -= HalfComplex(7, 4);
    EXPECT_THAT(complex, ComplexNear(HalfComplex(-5, -1), Half(1e-6f)));
  }
}

TEST(HalfComplex, multiply_complex) {
  // operator*
  EXPECT_THAT(HalfComplex(2, 3) * HalfComplex(7, 4),
              ComplexNear(HalfComplex(2, 29), Half(1e-6f)));

  // operator*=
  {
    HalfComplex complex(2, 3);
    complex *= HalfComplex(7, 4);
    EXPECT_THAT(complex, ComplexNear(HalfComplex(2, 29), Half(1e-6f)));
  }
}

TEST(HalfComplex, divide_complex) {
  // operator*
  EXPECT_THAT(HalfComplex(2, 3) / HalfComplex(7, 4),
              ComplexNear(HalfComplex(0.4f, 0.2f), Half(1e-6f)));

  // operator*=
  {
    HalfComplex complex(2, 3);
    complex /= HalfComplex(7, 4);
    EXPECT_THAT(complex, ComplexNear(HalfComplex(0.4f, 0.2f), Half(1e-6f)));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Non-class functions.

TEST(HalfComplex, Norm) {
  EXPECT_NEAR(float(Norm(HalfComplex(0))), 0.0f, 1e-6f);

  EXPECT_NEAR(float(Norm(HalfComplex(0.1f))), 0.0099945068359375f, 1e-6f);

  EXPECT_NEAR(float(Norm(HalfComplex(0.1f, 2.0f))), 4.01171875f, 1e-6f);
}

TEST(HalfComplex, Abs) {
  EXPECT_NEAR(float(Abs(HalfComplex(0))), 0.0f, 1e-6f);

  EXPECT_NEAR(float(Abs(HalfComplex(0.1f))), 0.0999755859375f, 1e-6f);

  EXPECT_NEAR(float(Abs(HalfComplex(2.0f, 3.0f))), 3.60546875f, 1e-6f);
}

TEST(HalfComplex, FastAbs) {
  EXPECT_NEAR(float(FastAbs(HalfComplex(0))), 0.0f, 1e-6f);

  EXPECT_NEAR(float(FastAbs(HalfComplex(0.1f))), 0.0999755859375f, 1e-6f);

  EXPECT_NEAR(float(FastAbs(HalfComplex(2.0f, 3.0f))), 3.60546875f, 1e-6f);
}

TEST(HalfComplex, Arg) {
  EXPECT_NEAR(float(Arg(HalfComplex(1.0f, 0.0f))), 0.0f, 0.001f);
  EXPECT_NEAR(float(Arg(HalfComplex(0.0f, 1.0f))), 1.57079637f, 0.001f);
  EXPECT_NEAR(float(Arg(HalfComplex(-3.0f, 2.0f))), 2.55359006f, 0.001f);
  EXPECT_NEAR(float(Arg(HalfComplex(3.0f, -2.0f))), -0.58800262f, 0.001f);
}

TEST(HalfComplex, FastArg) {
  EXPECT_NEAR(float(FastArg(HalfComplex(1.0f, 0.0f))), 0.0f, 0.005f);
  EXPECT_NEAR(float(FastArg(HalfComplex(0.0f, 1.0f))), 1.57079637f, 0.005f);
  EXPECT_NEAR(float(FastArg(HalfComplex(-3.0f, 2.0f))), 2.55359006f, 0.005f);
  EXPECT_NEAR(float(FastArg(HalfComplex(3.0f, -2.0f))), -0.58800262f, 0.005f);
}

TEST(HalfComplex, Conj) {
  EXPECT_EQ(Conj(HalfComplex(1.2f, 3.4f)), HalfComplex(1.2f, -3.4f));
  EXPECT_EQ(Conj(HalfComplex(1.2f, -3.4f)), HalfComplex(1.2f, 3.4f));

  EXPECT_EQ(Conj(HalfComplex(-1.2f, 3.4f)), HalfComplex(-1.2f, -3.4f));
  EXPECT_EQ(Conj(HalfComplex(-1.2f, -3.4f)), HalfComplex(-1.2f, 3.4f));
}

TEST(HalfComplex, FastIntPow) {
  EXPECT_THAT(FastIntPow(HalfComplex(0, 0), 0),
              ComplexNear(HalfComplex(1, 0), 1e-6f));
  EXPECT_THAT(FastIntPow(HalfComplex(2, 0), 0),
              ComplexNear(HalfComplex(1, 0), 1e-6f));
  EXPECT_THAT(FastIntPow(HalfComplex(2, 0), 4),
              ComplexNear(HalfComplex(16, 0), 1e-6f));

  // >>> (0.2-1j) ** 3
  // (-0.592+0.88j)
  EXPECT_THAT(FastIntPow(HalfComplex(0.2f, -1.0f), 3),
              ComplexNear(HalfComplex(-0.592f, 0.88f), 1e-6f));
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
