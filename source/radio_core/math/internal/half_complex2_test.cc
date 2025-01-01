// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half_complex2.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/math.h"
#  include "radio_core/math/unittest/complex_matchers.h"
#  include "radio_core/math/unittest/vectorized_matchers.h"
#  include "radio_core/unittest/mock.h"
#  include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

TEST(HalfComplex2, Load) {
  {
    const HalfComplex values[2] = {HalfComplex(2, 3), HalfComplex(4, 5)};
    const HalfComplex2 complex2(values);

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }

  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }

  {
    const HalfComplex2 complex2(HalfComplex(2, 3));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
  }

  {
    const HalfComplex2 complex2(Half2(2, 4), Half2(3, 5));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }

  {
    const HalfComplex2 complex2(123);

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
  }
}

TEST(HalfComplex2, Store) {
  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));

    HalfComplex data[2];
    complex2.Store(data);

    EXPECT_THAT(data[0], ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(HalfComplex(4, 5), 1e-6f));
  }

  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));

    HalfComplex data;

    complex2.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(2, 3), 1e-6f));

    complex2.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(4, 5), 1e-6f));
  }
}

TEST(HalfComplex2, Extract) {
  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));

    EXPECT_THAT(complex2.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }
}

TEST(HalfComplex2, ExtractXY) {
  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));

    EXPECT_THAT(complex2.x(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex2.y(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }
}

TEST(HalfComplex2, ExtractReal) {
  const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));
  EXPECT_THAT(complex2.ExtractReal(),
              testing::VectorizedNear<Half>(Half2(2, 4), 1e-6f));
}

TEST(HalfComplex2, ExtractImag) {
  const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));
  EXPECT_THAT(complex2.ExtractImag(),
              testing::VectorizedNear<Half>(Half2(3, 5), 1e-6f));
}

TEST(HalfComplex2, SetLane) {
  {
    const HalfComplex2 value(HalfComplex(2, 3), HalfComplex(4, 5));
    const HalfComplex2 new_value = value.SetLane<0>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }

  {
    const HalfComplex2 value(HalfComplex(2, 3), HalfComplex(4, 5));
    const HalfComplex2 new_value = value.SetLane<1>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex2, SetLaneXYZ) {
  {
    HalfComplex2 value(HalfComplex(2, 3), HalfComplex(4, 5));
    value.x(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }

  {
    HalfComplex2 value(HalfComplex(2, 3), HalfComplex(4, 5));
    value.y(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex2, Negate) {
  const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(-4, 5));
  const HalfComplex2 neg_complex2 = -complex2;

  EXPECT_THAT(neg_complex2.Extract<0>(),
              ComplexNear(HalfComplex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex2.Extract<1>(),
              ComplexNear(HalfComplex(4, -5), 1e-6f));
}

TEST(HalfComplex2, Add) {
  const HalfComplex2 a(HalfComplex(2, 3), HalfComplex(4, 10));
  const HalfComplex2 b(HalfComplex(3, 4), HalfComplex(5, 7));

  {
    const HalfComplex2 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
  }

  {
    HalfComplex2 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
  }
}

TEST(HalfComplex2, Subtract) {
  const HalfComplex2 a(HalfComplex(2, 3), HalfComplex(4, 10));
  const HalfComplex2 b(HalfComplex(3, 4), HalfComplex(5, 7));

  {
    const HalfComplex2 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
  }

  {
    HalfComplex2 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
  }
}

TEST(HalfComplex2, MultiplyComplex) {
  const HalfComplex2 a(HalfComplex(2, 3), HalfComplex(4, 10));
  const HalfComplex2 b(HalfComplex(3, 4), HalfComplex(5, 7));

  {
    const HalfComplex2 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
  }

  {
    HalfComplex2 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
  }
}

TEST(HalfComplex2, MultiplyScalar) {
  const HalfComplex2 a(HalfComplex(2, 3), HalfComplex(4, 10));
  const Half2 b{3, 5};

  {
    const HalfComplex2 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
  }

  {
    HalfComplex2 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
  }
}

TEST(HalfComplex2, Divide) {
  const HalfComplex2 a(HalfComplex(-6, 17), HalfComplex(-50, 78));
  const HalfComplex2 b(HalfComplex(2, 3), HalfComplex(4, 10));

  {
    const HalfComplex2 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(5, 7), 1e-5f));
  }
}

TEST(HalfComplex2, Abs) {
  {
    const HalfComplex2 complex2(HalfComplex(0), HalfComplex(0));
    const Half2 abs4 = Abs(complex2);

    EXPECT_NEAR(float(abs4.Extract<0>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), 0, 4e-3f);
  }

  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));
    const Half2 abs4 = Abs(complex2);

    EXPECT_NEAR(float(abs4.Extract<0>()), Sqrt(13.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), Sqrt(41.0f), 4e-3f);
  }
}

TEST(HalfComplex2, FastAbs) {
  {
    const HalfComplex2 complex2(HalfComplex(0), HalfComplex(0));
    const Half2 fast_abs4 = FastAbs(complex2);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), 0, 2e-2f);
  }

  {
    const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));
    const Half2 fast_abs4 = FastAbs(complex2);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), Sqrt(41.0f), 2e-2f);
  }
}

TEST(HalfComplex2, Norm) {
  const HalfComplex2 complex2(HalfComplex(2, 3), HalfComplex(4, 5));
  const Half2 norm4 = Norm(complex2);

  EXPECT_NEAR(float(norm4.Extract<0>()), 13, 1e-6f);
  EXPECT_NEAR(float(norm4.Extract<1>()), 41, 1e-6f);
}

TEST(HalfComplex2, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(HalfComplex2(HalfComplex(2, 3), HalfComplex(4, 5))),
              ComplexNear(HalfComplex(6, 8), 1e-6f));
}

TEST(HalfComplex2, MultiplyAdd) {
  const HalfComplex2 a(HalfComplex(2, 3), HalfComplex(4, 10));
  const HalfComplex2 b(HalfComplex(3, 4), HalfComplex(5, 7));
  const Half2 c(7, 4);

  const HalfComplex2 result = MultiplyAdd(a, b, c);
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(24, 38), 1e-6f));
}

TEST(HalfComplex2, FastArg) {
  const HalfComplex2 a(HalfComplex(1.0f, 0.0f), HalfComplex(0.0f, 1.0f));

  const Half2 arg4 = FastArg(a);

  Half arg_values[2];
  arg4.Store(arg_values);

  EXPECT_NEAR(float(arg_values[0]), 0.0f, 0.005f);
  EXPECT_NEAR(float(arg_values[1]), 1.57079637f, 0.005f);
}

TEST(HalfComplex2, Conj) {
  const HalfComplex2 a(HalfComplex(1.0f, 2.0f), HalfComplex(0.0f, -1.0f));

  const HalfComplex2 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(0.0f, 1.0f), 1e-6f));
}

TEST(HalfComplex2, ComplexExp) {
  constexpr float kEpsilon = 1e-3f;

  const Half2 x(0.0, -0.2f);

  const HalfComplex2 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(HalfComplex(0.9800665778412416f, -0.19866933079506122f),
                  kEpsilon));
}

TEST(HalfComplex2, Exp) {
  constexpr float kEpsilon = 1e-3f;

  const HalfComplex2 z(HalfComplex(0.0f, 0.0f), HalfComplex(0.1f, -0.2f));

  const HalfComplex2 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(HalfComplex(1.0831410796080632f, -0.21956356670825236f),
                  kEpsilon));
}

TEST(HalfComplex2, Reverse) {
  const HalfComplex2 result =
      Reverse(HalfComplex2(HalfComplex(1.0f, 2.0f), HalfComplex(3.0f, 4.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(1.0f, 2.0f), 1e-6f));
}

TEST(HalfComplex2, FastIntPow) {
  const HalfComplex2 base(HalfComplex(0.0f, 0.0f), HalfComplex(0.1f, 0.2f));
  const HalfComplex2 result = FastIntPow(base, 4);

  // >>> import numpy as np
  // >>> np.array([ 0.0+0.0j, 0.1+0.2j]) ** 4
  // array([ 0.    +0.j    , -0.0007-0.0024j ])
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(0.0f, 0.0f), 1e-3f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(-0.0007f, -0.0024f), 1e-3f));
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
