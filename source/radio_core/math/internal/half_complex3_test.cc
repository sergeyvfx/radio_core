// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half_complex3.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/math.h"
#  include "radio_core/math/unittest/complex_matchers.h"
#  include "radio_core/unittest/mock.h"
#  include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;

TEST(HalfComplex3, Load) {
  {
    const HalfComplex values[3] = {
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7)};
    const HalfComplex3 complex4(values);

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex4(HalfComplex(2, 3));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
  }
}

TEST(HalfComplex3, Store) {
  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    HalfComplex data[3];
    complex4.Store(data);

    EXPECT_THAT(data[0], ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    HalfComplex data;

    complex4.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(2, 3), 1e-6f));

    complex4.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(4, 5), 1e-6f));

    complex4.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(6, 7), 1e-6f));
  }
}

TEST(HalfComplex3, Extract) {
  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }
}

TEST(HalfComplex3, ExtractXYZ) {
  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    EXPECT_THAT(complex4.x(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.y(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.z(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }
}

TEST(HalfComplex3, Negate) {
  const HalfComplex3 complex4(
      HalfComplex(2, 3), HalfComplex(-4, 5), HalfComplex(6, -7));
  const HalfComplex3 neg_complex4 = -complex4;

  EXPECT_THAT(neg_complex4.Extract<0>(),
              ComplexNear(HalfComplex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<1>(),
              ComplexNear(HalfComplex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<2>(),
              ComplexNear(HalfComplex(-6, 7), 1e-6f));
}

TEST(HalfComplex3, Add) {
  const HalfComplex3 a(
      HalfComplex(2, 3), HalfComplex(4, 10), HalfComplex(6, 7));
  const HalfComplex3 b(HalfComplex(3, 4), HalfComplex(5, 7), HalfComplex(9, 6));

  {
    const HalfComplex3 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(15, 13), 1e-6f));
  }

  {
    HalfComplex3 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(15, 13), 1e-6f));
  }
}

TEST(HalfComplex3, Subtract) {
  const HalfComplex3 a(
      HalfComplex(2, 3), HalfComplex(4, 10), HalfComplex(6, 7));
  const HalfComplex3 b(HalfComplex(3, 4), HalfComplex(5, 7), HalfComplex(9, 6));

  {
    const HalfComplex3 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(-3, 1), 1e-6f));
  }

  {
    HalfComplex3 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(-3, 1), 1e-6f));
  }
}

TEST(HalfComplex3, Multiply) {
  const HalfComplex3 a(
      HalfComplex(2, 3), HalfComplex(4, 10), HalfComplex(6, 7));
  const HalfComplex3 b(HalfComplex(3, 4), HalfComplex(5, 7), HalfComplex(9, 6));

  {
    const HalfComplex3 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(12, 99), 1e-6f));
  }

  {
    HalfComplex3 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(12, 99), 1e-6f));
  }
}

TEST(HalfComplex3, Divide) {
  const HalfComplex3 a(
      HalfComplex(-6, 17), HalfComplex(-50, 78), HalfComplex(12, 99));
  const HalfComplex3 b(
      HalfComplex(2, 3), HalfComplex(4, 10), HalfComplex(6, 7));

  {
    const HalfComplex3 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(5, 7), 1e-5f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(9, 6), 1e-5f));
  }
}

TEST(HalfComplex3, Abs) {
  {
    const HalfComplex3 complex4(HalfComplex(0), HalfComplex(0), HalfComplex(0));
    const Half3 abs4 = Abs(complex4);

    EXPECT_NEAR(float(abs4.Extract<0>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<2>()), 0, 4e-3f);
  }

  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const Half3 abs4 = Abs(complex4);

    EXPECT_NEAR(float(abs4.Extract<0>()), Sqrt(13.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), Sqrt(41.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<2>()), Sqrt(85.0f), 4e-3f);
  }
}

TEST(HalfComplex3, FastAbs) {
  {
    const HalfComplex3 complex4(HalfComplex(0), HalfComplex(0), HalfComplex(0));
    const Half3 fast_abs4 = FastAbs(complex4);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<2>()), 0, 2e-2f);
  }

  {
    const HalfComplex3 complex4(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const Half3 fast_abs4 = FastAbs(complex4);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), Sqrt(41.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<2>()), Sqrt(85.0f), 2e-2f);
  }
}

TEST(HalfComplex3, Norm) {
  const HalfComplex3 complex4(
      HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
  const Half3 norm4 = Norm(complex4);

  EXPECT_NEAR(float(norm4.Extract<0>()), 13, 1e-6f);
  EXPECT_NEAR(float(norm4.Extract<1>()), 41, 1e-6f);
  EXPECT_NEAR(float(norm4.Extract<2>()), 85, 1e-6f);
}

TEST(HalfComplex3, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(HalfComplex3(
                  HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7))),
              ComplexNear(HalfComplex(12, 15), 1e-6f));
}

TEST(HalfComplex3, MultiplyAdd) {
  const HalfComplex3 a(
      HalfComplex(2, 3), HalfComplex(4, 10), HalfComplex(6, 7));
  const HalfComplex3 b(HalfComplex(3, 4), HalfComplex(5, 7), HalfComplex(9, 6));
  const Half3 c(7, 4, 3);

  const HalfComplex3 result = MultiplyAdd(a, b, c);
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(24, 38), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(HalfComplex(33, 25), 1e-6f));
}

TEST(HalfComplex3, FastArg) {
  const HalfComplex3 a(HalfComplex(1.0f, 0.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(-3.0f, 2.0f));

  const Half3 arg4 = FastArg(a);

  Half arg_values[3];
  arg4.Store(arg_values);

  EXPECT_NEAR(float(arg_values[0]), 0.0f, 0.005f);
  EXPECT_NEAR(float(arg_values[1]), 1.57079637f, 0.005f);
  EXPECT_NEAR(float(arg_values[2]), 2.55359006f, 0.005f);
}

TEST(HalfComplex3, Conj) {
  const HalfComplex3 a(HalfComplex(1.0f, 0.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(-3.0f, 2.0f));

  const HalfComplex3 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(-3.0f, -2.0f), 1e-6f));
}

TEST(Complex3, Reverse) {
  const HalfComplex3 result = Reverse(HalfComplex3(HalfComplex(1.0f, 2.0f),
                                                   HalfComplex(3.0f, 4.0f),
                                                   HalfComplex(5.0f, 6.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(HalfComplex(1.0f, 2.0f), 1e-6f));
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
