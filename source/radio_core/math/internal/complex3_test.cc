// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/complex3.h"

#include "radio_core/math/math.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/math/unittest/vectorized_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

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

  {
    const Complex3 complex3(Float3(2, 4, 6), Float3(3, 5, 7));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
  }

  {
    const Complex3 complex3(123);

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(Complex(123, 0), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(Complex(123, 0), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(Complex(123, 0), 1e-6f));
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

TEST(Complex3, ExtractReal) {
  const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));
  EXPECT_THAT(complex3.ExtractReal(),
              testing::VectorizedNear(Float3(2, 4, 6), 1e-6f));
}

TEST(Complex3, ExtractImag) {
  const Complex3 complex3(Complex(2, 3), Complex(4, 5), Complex(6, 7));
  EXPECT_THAT(complex3.ExtractImag(),
              testing::VectorizedNear(Float3(3, 5, 7), 1e-6f));
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

TEST(Complex3, MultiplyComplex) {
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

TEST(Complex3, MultiplyScalar) {
  const Complex3 a(Complex(2, 3), Complex(4, 10), Complex(6, 7));
  const Float3 b(3, 5, 9);

  {
    const Complex3 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(54, 63), 1e-6f));
  }

  {
    Complex3 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(54, 63), 1e-6f));
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

TEST(Complex3, Conj) {
  const Complex3 a(
      Complex(1.0f, 0.0f), Complex(0.0f, 1.0f), Complex(-3.0f, 2.0f));

  const Complex3 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(-3.0f, -2.0f), 1e-6f));
}

TEST(Complex3, ComplexExp) {
  const Float3 x(0.0, 0.1f, -0.2f);

  const Complex3 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(Complex(0.9950041652780258, 0.09983341664682815f), 1e-6f));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(Complex(0.9800665778412416f, -0.19866933079506122f), 1e-6f));
}

TEST(Complex3, Exp) {
  const Complex3 z(
      Complex(0.0f, 0.0f), Complex(-0.3f, 0.4f), Complex(0.1f, -0.2f));

  const Complex3 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(Complex(0.6823387667165518f, 0.2884882034499186f), 1e-6f));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(Complex(1.0831410796080632f, -0.21956356670825236f), 1e-6f));
}

TEST(Complex3, Reverse) {
  const Complex3 result = Reverse(
      Complex3(Complex(1.0f, 2.0f), Complex(3.0f, 4.0f), Complex(5.0f, 6.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(1.0f, 2.0f), 1e-6f));
}

TEST(Complex3, FastIntPow) {
  const Complex3 base(
      Complex(0.0f, 0.0f), Complex(0.1f, 0.2f), Complex(-0.3f, 0.4f));
  const Complex3 result = FastIntPow(base, 4);

  // >>> import numpy as np
  // >>> np.array([ 0.0+0.0j, 0.1+0.2j, -0.3+0.4j]) ** 4
  // array([ 0.    +0.j    , -0.0007-0.0024j, -0.0527+0.0336j ])
  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(0.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(Complex(-0.0007f, -0.0024f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(Complex(-0.0527f, 0.0336f), 1e-6f));
}

}  // namespace radio_core
