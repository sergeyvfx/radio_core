// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/complex2.h"

#include "radio_core/math/math.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;

TEST(Complex2, Load) {
  {
    const Complex values[2] = {Complex(2, 3), Complex(4, 5)};
    const Complex2 complex2(values);

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
  }

  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
  }

  {
    const Complex2 complex2(Complex(2, 3));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(Complex(2, 3), 1e-6f));
  }
}

TEST(Complex2, Store) {
  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));

    Complex data[2];
    complex2.Store(data);

    EXPECT_THAT(data[0], ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(Complex(4, 5), 1e-6f));
  }

  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));

    Complex data;

    complex2.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(2, 3), 1e-6f));

    complex2.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(4, 5), 1e-6f));
  }
}

TEST(Complex2, Extract) {
  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
  }
}

TEST(Complex2, ExtractXY) {
  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));

    EXPECT_THAT(complex2.x(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.y(), ComplexNear(Complex(4, 5), 1e-6f));
  }
}

TEST(Complex2, SetLane) {
  {
    const Complex2 value(Complex(2, 3), Complex(4, 5));
    const Complex2 new_value = value.SetLane<0>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
  }

  {
    const Complex2 value(Complex(2, 3), Complex(4, 5));
    const Complex2 new_value = value.SetLane<1>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex2, SetLaneXYZ) {
  {
    Complex2 value(Complex(2, 3), Complex(4, 5));
    value.x(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
  }

  {
    Complex2 value(Complex(2, 3), Complex(4, 5));
    value.y(Complex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex2, Negate) {
  const Complex2 complex2(Complex(2, 3), Complex(-4, 5));
  const Complex2 neg_complex2 = -complex2;

  EXPECT_THAT(neg_complex2.Extract<0>(), ComplexNear(Complex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex2.Extract<1>(), ComplexNear(Complex(4, -5), 1e-6f));
}

TEST(Complex2, Add) {
  const Complex2 a(Complex(2, 3), Complex(4, 10));
  const Complex2 b(Complex(3, 4), Complex(5, 7));

  {
    const Complex2 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
  }

  {
    Complex2 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
  }
}

TEST(Complex2, Subtract) {
  const Complex2 a(Complex(2, 3), Complex(4, 10));
  const Complex2 b(Complex(3, 4), Complex(5, 7));

  {
    const Complex2 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
  }

  {
    Complex2 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
  }
}

TEST(Complex2, Multiply) {
  const Complex2 a(Complex(2, 3), Complex(4, 10));
  const Complex2 b(Complex(3, 4), Complex(5, 7));

  {
    const Complex2 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
  }

  {
    Complex2 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
  }
}

TEST(Complex2, Divide) {
  const Complex2 a(Complex(-6, 17), Complex(-50, 78));
  const Complex2 b(Complex(2, 3), Complex(4, 10));

  {
    const Complex2 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(5, 7), 1e-5f));
  }
}

TEST(Complex2, Abs) {
  {
    const Complex2 complex2(Complex(0), Complex(0));
    const Float2 abs4 = Abs(complex2);

    EXPECT_NEAR(abs4.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), 0.0f, 1e-6f);
  }

  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));
    const Float2 abs4 = Abs(complex2);

    EXPECT_NEAR(abs4.Extract<0>(), Sqrt(13.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), Sqrt(41.0f), 1e-6f);
  }
}

TEST(Complex2, FastAbs) {
  {
    const Complex2 complex2(Complex(0), Complex(0));
    const Float2 abs4 = FastAbs(complex2);

    EXPECT_NEAR(abs4.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), 0.0f, 1e-6f);
  }

  {
    const Complex2 complex2(Complex(2, 3), Complex(4, 5));
    const Float2 fast_abs4 = FastAbs(complex2);

    EXPECT_NEAR(fast_abs4.Extract<0>(), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(fast_abs4.Extract<1>(), Sqrt(41.0f), 2e-2f);
  }
}

TEST(Complex2, Norm) {
  const Complex2 complex2(Complex(2, 3), Complex(4, 5));
  const Float2 norm4 = Norm(complex2);

  EXPECT_NEAR(norm4.Extract<0>(), 13, 1e-6f);
  EXPECT_NEAR(norm4.Extract<1>(), 41, 1e-6f);
}

TEST(Complex2, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(Complex2(Complex(2, 3), Complex(4, 5))),
              ComplexNear(Complex(6, 8), 1e-6f));
}

TEST(Complex2, MultiplyAdd) {
  const Complex2 a(Complex(2, 3), Complex(4, 10));
  const Complex2 b(Complex(3, 4), Complex(5, 7));
  const Float2 c(7, 4);

  const Complex2 result = MultiplyAdd(a, b, c);
  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(24, 38), 1e-6f));
}

TEST(Complex2, FastArg) {
  const Complex2 a(Complex(1.0f, 0.0f), Complex(0.0f, 1.0f));

  const Float2 arg2 = FastArg(a);

  float arg_values[2];
  arg2.Store(arg_values);

  EXPECT_NEAR(arg_values[0], 0.0f, 0.005f);
  EXPECT_NEAR(arg_values[1], 1.57079637f, 0.005f);
}

TEST(Complex2, Conj) {
  const Complex2 a(Complex(1.0f, 2.0f), Complex(0.0f, -1.0f));

  const Complex2 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(0.0f, 1.0f), 1e-6f));
}

TEST(Complex2, Reverse) {
  const Complex2 result =
      Reverse(Complex2(Complex(1.0f, 2.0f), Complex(3.0f, 4.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(1.0f, 2.0f), 1e-6f));
}

}  // namespace radio_core
