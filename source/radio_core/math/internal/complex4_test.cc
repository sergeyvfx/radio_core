// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/complex4.h"

#include "radio_core/math/math.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/math/unittest/vectorized_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

TEST(Complex4, Load) {
  {
    const Complex values[4] = {
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9)};
    const Complex4 complex4(values);

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 complex4(Complex(2, 3));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(Complex(2, 3), 1e-6f));
  }

  {
    const Complex4 complex4(Float4(2, 4, 6, 8), Float4(3, 5, 7, 9));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 complex4(123);

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(Complex(123, 0), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(Complex(123, 0), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(Complex(123, 0), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(Complex(123, 0), 1e-6f));
  }
}

TEST(Complex4, Store) {
  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    Complex data[4];
    complex4.Store(data);

    EXPECT_THAT(data[0], ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(data[3], ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    Complex data;

    complex4.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(2, 3), 1e-6f));

    complex4.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(4, 5), 1e-6f));

    complex4.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(6, 7), 1e-6f));

    complex4.Store<3>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(8, 9), 1e-6f));
  }
}

TEST(Complex4, Extract) {
  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }
}

TEST(Complex4, ExtractLow) {
  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    const Complex2 low = complex4.ExtractLow();

    EXPECT_THAT(low.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(low.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
  }
}

TEST(Complex4, ExtractHigh) {
  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    const Complex2 high = complex4.ExtractHigh();

    EXPECT_THAT(high.Extract<0>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(high.Extract<1>(), ComplexNear(Complex(8, 9), 1e-6f));
  }
}

TEST(Complex4, ExtractXYZW) {
  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));

    EXPECT_THAT(complex4.x(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.y(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.z(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.w(), ComplexNear(Complex(8, 9), 1e-6f));
  }
}

TEST(Complex4, ExtractReal) {
  const Complex4 complex4(
      Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
  EXPECT_THAT(complex4.ExtractReal(),
              testing::VectorizedNear(Float4(2, 4, 6, 8), 1e-6f));
}

TEST(Complex4, ExtractImag) {
  const Complex4 complex4(
      Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
  EXPECT_THAT(complex4.ExtractImag(),
              testing::VectorizedNear(Float4(3, 5, 7, 9), 1e-6f));
}

TEST(Complex4, SetLane) {
  {
    const Complex4 value(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    const Complex4 new_value = value.SetLane<0>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 value(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    const Complex4 new_value = value.SetLane<1>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 value(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    const Complex4 new_value = value.SetLane<2>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    const Complex4 value(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    const Complex4 new_value = value.SetLane<3>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex4, SetLaneXYZW) {
  {
    Complex4 value(Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    value.x(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    Complex4 value(Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    value.y(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    Complex4 value(Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    value.z(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }

  {
    Complex4 value(Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    value.w(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex4, Negate) {
  const Complex4 complex4(
      Complex(2, 3), Complex(-4, 5), Complex(6, -7), Complex(-8, -9));
  const Complex4 neg_complex4 = -complex4;

  EXPECT_THAT(neg_complex4.Extract<0>(), ComplexNear(Complex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<1>(), ComplexNear(Complex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<2>(), ComplexNear(Complex(-6, 7), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
}

TEST(Complex4, Add) {
  const Complex4 a(Complex(2, 3), Complex(4, 10), Complex(6, 7), Complex(8, 9));
  const Complex4 b(Complex(3, 4), Complex(5, 7), Complex(9, 6), Complex(2, 10));

  {
    const Complex4 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(10, 19), 1e-6f));
  }

  {
    Complex4 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(10, 19), 1e-6f));
  }
}

TEST(Complex4, Subtract) {
  const Complex4 a(Complex(2, 3), Complex(4, 10), Complex(6, 7), Complex(8, 9));
  const Complex4 b(Complex(3, 4), Complex(5, 7), Complex(9, 6), Complex(2, 10));

  {
    const Complex4 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(6, -1), 1e-6f));
  }

  {
    Complex4 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(6, -1), 1e-6f));
  }
}

TEST(Complex4, MultiplyComplex) {
  const Complex4 a(Complex(2, 3), Complex(4, 10), Complex(6, 7), Complex(8, 9));
  const Complex4 b(Complex(3, 4), Complex(5, 7), Complex(9, 6), Complex(2, 10));

  {
    const Complex4 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(-74, 98), 1e-6f));
  }

  {
    Complex4 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(-74, 98), 1e-6f));
  }
}

TEST(Complex4, MultiplyScalar) {
  const Complex4 a(Complex(2, 3), Complex(4, 10), Complex(6, 7), Complex(8, 9));
  const Float4 b(3, 5, 9, 2);

  {
    const Complex4 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(16, 18), 1e-6f));
  }

  {
    Complex4 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(16, 18), 1e-6f));
  }
}

TEST(Complex4, Divide) {
  const Complex4 a(
      Complex(-6, 17), Complex(-50, 78), Complex(12, 99), Complex(-74, 98));
  const Complex4 b(Complex(2, 3), Complex(4, 10), Complex(6, 7), Complex(8, 9));

  {
    const Complex4 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(5, 7), 1e-5f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(9, 6), 1e-5f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(2, 10), 1e-5f));
  }
}

TEST(Complex4, Abs) {
  {
    const Complex4 complex4(Complex(0), Complex(0), Complex(0), Complex(0));
    const Float4 abs4 = Abs(complex4);

    EXPECT_NEAR(abs4.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<3>(), 0.0f, 1e-6f);
  }

  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    const Float4 abs4 = Abs(complex4);

    EXPECT_NEAR(abs4.Extract<0>(), Sqrt(13.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), Sqrt(41.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), Sqrt(85.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<3>(), Sqrt(145.0f), 1e-6f);
  }
}

TEST(Complex4, FastAbs) {
  {
    const Complex4 complex4(Complex(0), Complex(0), Complex(0), Complex(0));
    const Float4 abs4 = FastAbs(complex4);

    EXPECT_NEAR(abs4.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<3>(), 0.0f, 1e-6f);
  }

  {
    const Complex4 complex4(
        Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
    const Float4 fast_abs4 = FastAbs(complex4);

    EXPECT_NEAR(fast_abs4.Extract<0>(), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(fast_abs4.Extract<1>(), Sqrt(41.0f), 2e-2f);
    EXPECT_NEAR(fast_abs4.Extract<2>(), Sqrt(85.0f), 2e-2f);
    EXPECT_NEAR(fast_abs4.Extract<3>(), Sqrt(145.0f), 2e-2f);
  }
}

TEST(Complex4, Norm) {
  const Complex4 complex4(
      Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9));
  const Float4 norm4 = Norm(complex4);

  EXPECT_NEAR(norm4.Extract<0>(), 13, 1e-6f);
  EXPECT_NEAR(norm4.Extract<1>(), 41, 1e-6f);
  EXPECT_NEAR(norm4.Extract<2>(), 85, 1e-6f);
  EXPECT_NEAR(norm4.Extract<3>(), 145, 1e-6f);
}

TEST(Complex4, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(Complex4(
                  Complex(2, 3), Complex(4, 5), Complex(6, 7), Complex(8, 9))),
              ComplexNear(Complex(20, 24), 1e-6f));
}

TEST(Complex4, MultiplyAdd) {
  const Complex4 a(Complex(2, 3), Complex(4, 10), Complex(6, 7), Complex(8, 9));
  const Complex4 b(Complex(3, 4), Complex(5, 7), Complex(9, 6), Complex(2, 10));
  const Float4 c(7, 4, 3, 2);

  const Complex4 result = MultiplyAdd(a, b, c);
  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(24, 38), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(33, 25), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(Complex(12, 29), 1e-6f));
}

TEST(Complex4, FastArg) {
  const Complex4 a(Complex(1.0f, 0.0f),
                   Complex(0.0f, 1.0f),
                   Complex(-3.0f, 2.0f),
                   Complex(3.0f, -2.0f));

  const Float4 arg4 = FastArg(a);

  float arg_values[4];
  arg4.Store(arg_values);

  EXPECT_NEAR(arg_values[0], 0.0f, 0.005f);
  EXPECT_NEAR(arg_values[1], 1.57079637f, 0.005f);
  EXPECT_NEAR(arg_values[2], 2.55359006f, 0.005f);
  EXPECT_NEAR(arg_values[3], -0.58800262f, 0.005f);
}

TEST(Complex4, Conj) {
  const Complex4 a(Complex(1.0f, 0.0f),
                   Complex(0.0f, 1.0f),
                   Complex(-3.0f, 2.0f),
                   Complex(3.0f, -2.0f));

  const Complex4 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(-3.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(Complex(3.0f, 2.0f), 1e-6f));
}

TEST(Complex4, ComplexExp) {
  const Float4 x(0.0, 0.1f, -0.2f, -0.3f);

  const Complex4 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(Complex(0.9950041652780258, 0.09983341664682815f), 1e-6f));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(Complex(0.9800665778412416f, -0.19866933079506122f), 1e-6f));
  EXPECT_THAT(
      result.Extract<3>(),
      ComplexNear(Complex(0.955336489125606f, -0.29552020666133955f), 1e-6f));
}

TEST(Complex4, Exp) {
  const Complex4 z(Complex(0.0f, 0.0f),
                   Complex(0.1f, 0.2f),
                   Complex(-0.3f, 0.4f),
                   Complex(0.1f, -0.2f));

  const Complex4 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(Complex(1.0831410796080632f, 0.21956356670825236f), 1e-6f));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(Complex(0.6823387667165518f, 0.2884882034499186f), 1e-6f));
  EXPECT_THAT(
      result.Extract<3>(),
      ComplexNear(Complex(1.0831410796080632f, -0.21956356670825236f), 1e-6f));
}

TEST(Complex4, Reverse) {
  const Complex4 result = Reverse(Complex4(Complex(1.0f, 2.0f),
                                           Complex(3.0f, 4.0f),
                                           Complex(5.0f, 6.0f),
                                           Complex(7.0f, 8.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(7.0f, 8.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(Complex(1.0f, 2.0f), 1e-6f));
}

TEST(Complex4, FastIntPow) {
  const Complex4 base(Complex(0.0f, 0.0f),
                      Complex(0.1f, 0.2f),
                      Complex(-0.3f, 0.4f),
                      Complex(-0.4f, 0.5f));
  const Complex4 result = FastIntPow(base, 4);

  // >>> import numpy as np
  // >>> np.array([ 0.0+0.0j, 0.1+0.2j, -0.3+0.4j, -0.4+0.5j]) ** 4
  // array([ 0.    +0.j    , -0.0007-0.0024j, -0.0527+0.0336j, -0.1519+0.072j ])
  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(0.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(Complex(-0.0007f, -0.0024f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(Complex(-0.0527f, 0.0336f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(),
              ComplexNear(Complex(-0.1519f, 0.072f), 1e-6f));
}

}  // namespace radio_core
