// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/complex3.h"

#include "radio_core/math/math.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;

TEST(Complex3, Load) {
  {
    const Complex values[3] = {Complex(2, 3), Complex(4, 5), Complex(6, 7)};
    const Complex3 complex3(values);

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    const Complex3 complex3(Complex(2, 3));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(Complex(2, 3), 1e-6f));
  }
}

TEST(Complex3, Store) {
  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));

    Complex data[3];
    complex3.Store(data);

    EXPECT_THAT(data[0], ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));

    Complex data;

    complex3.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(2, 3), 1e-6f));

    complex3.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(4, 5), 1e-6f));

    complex3.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(6, 7), 1e-6f));
  }
}

TEST(Complex3, Extract) {
  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }
}

TEST(Complex3, ExtractXYZ) {
  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));

    EXPECT_THAT(complex3.x(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.y(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.z(), ComplexNear(Complex(6, 7), 1e-6f));
  }
}

TEST(Complex3, SetLane) {
  {
    const Complex3 value(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    const Complex3 new_value = value.SetLane<0>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    const Complex3 value(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    const Complex3 new_value = value.SetLane<1>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    const Complex3 value(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    const Complex3 new_value = value.SetLane<2>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex3, SetLaneXYZ) {
  {
    Complex3 value(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    value.x(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    Complex3 value(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    value.y(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    Complex3 value(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    value.z(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex3, Negate) {
  const Complex3 complex3(Complex(2, 3), Complex(-4, 5), Complex(6, -7));
  const Complex3 neg_complex3 = -complex3;

  EXPECT_THAT(neg_complex3.Extract<0>(), ComplexNear(Complex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex3.Extract<1>(), ComplexNear(Complex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex3.Extract<2>(), ComplexNear(Complex(-6, 7), 1e-6f));
}

TEST(Complex3, Add) {
  const Complex3 a(Complex(2, 3), Complex(4, 10), Complex(6, 7));
  const Complex3 b(Complex(3, 4), Complex(5, 7), Complex(9, 6));

  {
    const Complex3 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(15, 13), 1e-6f));
  }

  {
    Complex3 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(15, 13), 1e-6f));
  }
}

TEST(Complex3, Subtract) {
  const Complex3 a(Complex(2, 3), Complex(4, 10), Complex(6, 7));
  const Complex3 b(Complex(3, 4), Complex(5, 7), Complex(9, 6));

  {
    const Complex3 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(-3, 1), 1e-6f));
  }

  {
    Complex3 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(-3, 1), 1e-6f));
  }
}

TEST(Complex3, Multiply) {
  const Complex3 a(Complex(2, 3), Complex(4, 10), Complex(6, 7));
  const Complex3 b(Complex(3, 4), Complex(5, 7), Complex(9, 6));

  {
    const Complex3 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(12, 99), 1e-6f));
  }

  {
    Complex3 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(12, 99), 1e-6f));
  }
}

TEST(Complex3, Divide) {
  const Complex3 a(Complex(-6, 17), Complex(-50, 78), Complex(12, 99));
  const Complex3 b(Complex(2, 3), Complex(4, 10), Complex(6, 7));

  {
    const Complex3 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(5, 7), 1e-5f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(9, 6), 1e-5f));
  }
}

TEST(Complex3, Abs) {
  {
    const Complex3 complex3(Complex(0), Complex(0), Complex(0));
    const Float3 abs4 = Abs(complex3);

    EXPECT_NEAR(abs4.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), 0.0f, 1e-6f);
  }

  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    const Float3 abs4 = Abs(complex3);

    EXPECT_NEAR(abs4.Extract<0>(), Sqrt(13.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), Sqrt(41.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), Sqrt(85.0f), 1e-6f);
  }
}

TEST(Complex3, FastAbs) {
  {
    const Complex3 complex3(Complex(0), Complex(0), Complex(0));
    const Float3 abs4 = FastAbs(complex3);

    EXPECT_NEAR(abs4.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), 0.0f, 1e-6f);
  }

  {
    const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));
    const Float3 fast_abs4 = FastAbs(complex3);

    EXPECT_NEAR(fast_abs4.Extract<0>(), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(fast_abs4.Extract<1>(), Sqrt(41.0f), 2e-2f);
    EXPECT_NEAR(fast_abs4.Extract<2>(), Sqrt(85.0f), 2e-2f);
  }
}

TEST(Complex3, Norm) {
  const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));
  const Float3 norm4 = Norm(complex3);

  EXPECT_NEAR(norm4.Extract<0>(), 13, 1e-6f);
  EXPECT_NEAR(norm4.Extract<1>(), 41, 1e-6f);
  EXPECT_NEAR(norm4.Extract<2>(), 85, 1e-6f);
}

TEST(Complex3, HorizontalSum) {
  EXPECT_THAT(
      HorizontalSum(Complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7))),
      ComplexNear(Complex(12, 15), 1e-6f));
}

TEST(Complex3, MultiplyAdd) {
  const Complex3 a(Complex(2, 3), Complex(4, 10), Complex(6, 7));
  const Complex3 b(Complex(3, 4), Complex(5, 7), Complex(9, 6));
  const Float3 c(7, 4, 3);

  const Complex3 result = MultiplyAdd(a, b, c);
  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(24, 38), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(33, 25), 1e-6f));
}

TEST(Complex3, FastArg) {
  const Complex3 a(
      Complex(1.0f, 0.0f), Complex(0.0f, 1.0f), Complex(-3.0f, 2.0f));

  const Float3 arg4 = FastArg(a);

  float arg_values[3];
  arg4.Store(arg_values);

  EXPECT_NEAR(arg_values[0], 0.0f, 0.005f);
  EXPECT_NEAR(arg_values[1], 1.57079637f, 0.005f);
  EXPECT_NEAR(arg_values[2], 2.55359006f, 0.005f);
}

}  // namespace radio_core
