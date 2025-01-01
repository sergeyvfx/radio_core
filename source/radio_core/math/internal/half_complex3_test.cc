// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half_complex3.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/math.h"
#  include "radio_core/math/unittest/complex_matchers.h"
#  include "radio_core/math/unittest/vectorized_matchers.h"
#  include "radio_core/unittest/mock.h"
#  include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

TEST(HalfComplex3, Load) {
  {
    const HalfComplex values[3] = {
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7)};
    const HalfComplex3 complex3(values);

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex3(HalfComplex(2, 3));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
  }

  {
    const HalfComplex3 complex3(Half3(2, 4, 6), Half3(3, 5, 7));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex3(123);

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
  }
}

TEST(HalfComplex3, Store) {
  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    HalfComplex data[3];
    complex3.Store(data);

    EXPECT_THAT(data[0], ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    HalfComplex data;

    complex3.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(2, 3), 1e-6f));

    complex3.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(4, 5), 1e-6f));

    complex3.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(6, 7), 1e-6f));
  }
}

TEST(HalfComplex3, Extract) {
  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    EXPECT_THAT(complex3.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }
}

TEST(HalfComplex3, ExtractXYZ) {
  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));

    EXPECT_THAT(complex3.x(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex3.y(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex3.z(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }
}

TEST(HalfComplex3, ExtractReal) {
  const HalfComplex3 complex3(
      HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
  EXPECT_THAT(complex3.ExtractReal(),
              testing::VectorizedNear<Half>(Half3(2, 4, 6), 1e-6f));
}

TEST(HalfComplex3, ExtractImag) {
  const HalfComplex3 complex3(
      HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
  EXPECT_THAT(complex3.ExtractImag(),
              testing::VectorizedNear<Half>(Half3(3, 5, 7), 1e-6f));
}

TEST(HalfComplex3, HalfSetLane) {
  {
    const HalfComplex3 value(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const HalfComplex3 new_value = value.SetLane<0>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 value(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const HalfComplex3 new_value = value.SetLane<1>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    const HalfComplex3 value(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const HalfComplex3 new_value = value.SetLane<2>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex3, SetLaneXYZ) {
  {
    HalfComplex3 value(HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    value.x(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    HalfComplex3 value(HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    value.y(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
  }

  {
    HalfComplex3 value(HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    value.z(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex3, Negate) {
  const HalfComplex3 complex3(
      HalfComplex(2, 3), HalfComplex(-4, 5), HalfComplex(6, -7));
  const HalfComplex3 neg_complex3 = -complex3;

  EXPECT_THAT(neg_complex3.Extract<0>(),
              ComplexNear(HalfComplex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex3.Extract<1>(),
              ComplexNear(HalfComplex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex3.Extract<2>(),
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

TEST(HalfComplex3, MultiplyComplex) {
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

TEST(HalfComplex3, MultiplyScalar) {
  const HalfComplex3 a(
      HalfComplex(2, 3), HalfComplex(4, 10), HalfComplex(6, 7));
  const Half3 b(3, 5, 9);

  {
    const HalfComplex3 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(54, 63), 1e-6f));
  }

  {
    HalfComplex3 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(54, 63), 1e-6f));
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
    const HalfComplex3 complex3(HalfComplex(0), HalfComplex(0), HalfComplex(0));
    const Half3 abs4 = Abs(complex3);

    EXPECT_NEAR(float(abs4.Extract<0>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<2>()), 0, 4e-3f);
  }

  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const Half3 abs4 = Abs(complex3);

    EXPECT_NEAR(float(abs4.Extract<0>()), Sqrt(13.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), Sqrt(41.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<2>()), Sqrt(85.0f), 4e-3f);
  }
}

TEST(HalfComplex3, FastAbs) {
  {
    const HalfComplex3 complex3(HalfComplex(0), HalfComplex(0), HalfComplex(0));
    const Half3 fast_abs4 = FastAbs(complex3);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<2>()), 0, 2e-2f);
  }

  {
    const HalfComplex3 complex3(
        HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
    const Half3 fast_abs4 = FastAbs(complex3);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), Sqrt(41.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<2>()), Sqrt(85.0f), 2e-2f);
  }
}

TEST(HalfComplex3, Norm) {
  const HalfComplex3 complex3(
      HalfComplex(2, 3), HalfComplex(4, 5), HalfComplex(6, 7));
  const Half3 norm4 = Norm(complex3);

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

TEST(HalfComplex3, ComplexExp) {
  constexpr float kEpsilon = 1e-3f;

  const Half3 x(0.0, 0.1f, -0.2f);

  const HalfComplex3 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.9950041652780258, 0.09983341664682815f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(HalfComplex(0.9800665778412416f, -0.19866933079506122f),
                  kEpsilon));
}

TEST(HalfComplex3, Exp) {
  constexpr float kEpsilon = 1e-3f;

  const HalfComplex3 z(HalfComplex(0.0f, 0.0f),
                       HalfComplex(-0.3f, 0.4f),
                       HalfComplex(0.1f, -0.2f));

  const HalfComplex3 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.6823387667165518f, 0.2884882034499186f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(HalfComplex(1.0831410796080632f, -0.21956356670825236f),
                  kEpsilon));
}

TEST(HalfComplex3, Reverse) {
  const HalfComplex3 result = Reverse(HalfComplex3(HalfComplex(1.0f, 2.0f),
                                                   HalfComplex(3.0f, 4.0f),
                                                   HalfComplex(5.0f, 6.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(HalfComplex(1.0f, 2.0f), 1e-6f));
}

TEST(HalfComplex3, FastIntPow) {
  const HalfComplex3 base(HalfComplex(0.0f, 0.0f),
                          HalfComplex(0.1f, 0.2f),
                          HalfComplex(-0.3f, 0.4f));
  const HalfComplex3 result = FastIntPow(base, 4);

  // >>> import numpy as np
  // >>> np.array([ 0.0+0.0j, 0.1+0.2j, -0.3+0.4j]) ** 4
  // array([ 0.    +0.j    , -0.0007-0.0024j, -0.0527+0.0336j ])
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(0.0f, 0.0f), 1e-3f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(-0.0007f, -0.0024f), 1e-3f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(-0.0527f, 0.0336f), 1e-3f));
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
