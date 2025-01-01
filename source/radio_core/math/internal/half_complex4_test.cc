// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half_complex4.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/math.h"
#  include "radio_core/math/unittest/complex_matchers.h"
#  include "radio_core/math/unittest/vectorized_matchers.h"
#  include "radio_core/unittest/mock.h"
#  include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

TEST(HalfComplex4, Load) {
  {
    const HalfComplex values[4] = {HalfComplex(2, 3),
                                   HalfComplex(4, 5),
                                   HalfComplex(6, 7),
                                   HalfComplex(8, 9)};
    const HalfComplex4 complex4(values);

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 complex4(HalfComplex(2, 3));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
  }

  {
    const HalfComplex4 complex4(Half4(2, 4, 6, 8), Half4(3, 5, 7, 9));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 complex4(123);

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
  }
}

TEST(HalfComplex4, Store) {
  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    HalfComplex data[4];
    complex4.Store(data);

    EXPECT_THAT(data[0], ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(data[3], ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    HalfComplex data;

    complex4.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(2, 3), 1e-6f));

    complex4.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(4, 5), 1e-6f));

    complex4.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(6, 7), 1e-6f));

    complex4.Store<3>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(8, 9), 1e-6f));
  }
}

TEST(HalfComplex4, Extract) {
  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    EXPECT_THAT(complex4.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }
}

TEST(HalfComplex4, ExtractLow) {
  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    const HalfComplex2 low = complex4.ExtractLow();

    EXPECT_THAT(low.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(low.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
  }
}

TEST(HalfComplex4, ExtractHigh) {
  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    const HalfComplex2 high = complex4.ExtractHigh();

    EXPECT_THAT(high.Extract<0>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(high.Extract<1>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }
}

TEST(HalfComplex4, ExtractXYZW) {
  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));

    EXPECT_THAT(complex4.x(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex4.y(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex4.z(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex4.w(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }
}

TEST(HalfComplex4, ExtractReal) {
  const HalfComplex4 complex4(HalfComplex(2, 3),
                              HalfComplex(4, 5),
                              HalfComplex(6, 7),
                              HalfComplex(8, 9));
  EXPECT_THAT(complex4.ExtractReal(),
              testing::VectorizedNear<Half>(Half4(2, 4, 6, 8), 1e-6f));
}

TEST(HalfComplex4, ExtractImag) {
  const HalfComplex4 complex4(HalfComplex(2, 3),
                              HalfComplex(4, 5),
                              HalfComplex(6, 7),
                              HalfComplex(8, 9));
  EXPECT_THAT(complex4.ExtractImag(),
              testing::VectorizedNear<Half>(Half4(3, 5, 7, 9), 1e-6f));
}

TEST(HalfComplex4, SetLane) {
  {
    const HalfComplex4 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9));
    const HalfComplex4 new_value = value.SetLane<0>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9));
    const HalfComplex4 new_value = value.SetLane<1>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9));
    const HalfComplex4 new_value = value.SetLane<2>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    const HalfComplex4 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9));
    const HalfComplex4 new_value = value.SetLane<3>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex4, SetLaneXYZW) {
  {
    HalfComplex4 value(HalfComplex(2, 3),
                       HalfComplex(4, 5),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
    value.x(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    HalfComplex4 value(HalfComplex(2, 3),
                       HalfComplex(4, 5),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
    value.y(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    HalfComplex4 value(HalfComplex(2, 3),
                       HalfComplex(4, 5),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
    value.z(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }

  {
    HalfComplex4 value(HalfComplex(2, 3),
                       HalfComplex(4, 5),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
    value.w(HalfComplex(199, 299));

    EXPECT_THAT(value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(value.Extract<3>(), ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex4, Negate) {
  const HalfComplex4 complex4(HalfComplex(2, 3),
                              HalfComplex(-4, 5),
                              HalfComplex(6, -7),
                              HalfComplex(-8, -9));
  const HalfComplex4 neg_complex4 = -complex4;

  EXPECT_THAT(neg_complex4.Extract<0>(),
              ComplexNear(HalfComplex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<1>(),
              ComplexNear(HalfComplex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<2>(),
              ComplexNear(HalfComplex(-6, 7), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
}

TEST(HalfComplex4, Add) {
  const HalfComplex4 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
  const HalfComplex4 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10));

  {
    const HalfComplex4 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(10, 19), 1e-6f));
  }

  {
    HalfComplex4 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(10, 19), 1e-6f));
  }
}

TEST(HalfComplex4, Subtract) {
  const HalfComplex4 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
  const HalfComplex4 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10));

  {
    const HalfComplex4 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(6, -1), 1e-6f));
  }

  {
    HalfComplex4 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(6, -1), 1e-6f));
  }
}

TEST(HalfComplex4, MultiplyComplex) {
  const HalfComplex4 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
  const HalfComplex4 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10));

  {
    const HalfComplex4 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(-74, 98), 1e-6f));
  }

  {
    HalfComplex4 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(-74, 98), 1e-6f));
  }
}

TEST(HalfComplex4, MultiplyScalar) {
  const HalfComplex4 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
  const Half4 b(3, 5, 9, 2);

  {
    const HalfComplex4 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(16, 18), 1e-6f));
  }

  {
    HalfComplex4 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(16, 18), 1e-6f));
  }
}

TEST(HalfComplex4, Divide) {
  const HalfComplex4 a(HalfComplex(-6, 17),
                       HalfComplex(-50, 78),
                       HalfComplex(12, 99),
                       HalfComplex(-74, 98));
  const HalfComplex4 b(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));

  {
    const HalfComplex4 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(5, 7), 1e-5f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(9, 6), 1e-5f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(2, 10), 1e-5f));
  }
}

TEST(HalfComplex4, Abs) {
  {
    const HalfComplex4 complex4(
        HalfComplex(0), HalfComplex(0), HalfComplex(0), HalfComplex(0));
    const Half4 abs4 = Abs(complex4);

    EXPECT_NEAR(float(abs4.Extract<0>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<2>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<3>()), 0, 6e-3f);
  }

  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));
    const Half4 abs4 = Abs(complex4);

    EXPECT_NEAR(float(abs4.Extract<0>()), Sqrt(13.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<1>()), Sqrt(41.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<2>()), Sqrt(85.0f), 4e-3f);
    EXPECT_NEAR(float(abs4.Extract<3>()), Sqrt(145.0f), 6e-3f);
  }
}

TEST(HalfComplex4, FastAbs) {
  {
    const HalfComplex4 complex4(
        HalfComplex(0), HalfComplex(0), HalfComplex(0), HalfComplex(0));
    const Half4 fast_abs4 = FastAbs(complex4);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<2>()), 0, 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<3>()), 0, 2e-2f);
  }

  {
    const HalfComplex4 complex4(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9));
    const Half4 fast_abs4 = FastAbs(complex4);

    EXPECT_NEAR(float(fast_abs4.Extract<0>()), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<1>()), Sqrt(41.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<2>()), Sqrt(85.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs4.Extract<3>()), Sqrt(145.0f), 2e-2f);
  }
}

TEST(HalfComplex4, Norm) {
  const HalfComplex4 complex4(HalfComplex(2, 3),
                              HalfComplex(4, 5),
                              HalfComplex(6, 7),
                              HalfComplex(8, 9));
  const Half4 norm4 = Norm(complex4);

  EXPECT_NEAR(float(norm4.Extract<0>()), 13, 1e-6f);
  EXPECT_NEAR(float(norm4.Extract<1>()), 41, 1e-6f);
  EXPECT_NEAR(float(norm4.Extract<2>()), 85, 1e-6f);
  EXPECT_NEAR(float(norm4.Extract<3>()), 145, 1e-6f);
}

TEST(HalfComplex4, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(HalfComplex4(HalfComplex(2, 3),
                                         HalfComplex(4, 5),
                                         HalfComplex(6, 7),
                                         HalfComplex(8, 9))),
              ComplexNear(HalfComplex(20, 24), 1e-6f));
}

TEST(HalfComplex4, MultiplyAdd) {
  const HalfComplex4 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9));
  const HalfComplex4 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10));
  const Half4 c(7, 4, 3, 2);

  const HalfComplex4 result = MultiplyAdd(a, b, c);
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(24, 38), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(HalfComplex(33, 25), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(HalfComplex(12, 29), 1e-6f));
}

TEST(HalfComplex4, FastArg) {
  const HalfComplex4 a(HalfComplex(1.0f, 0.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(-3.0f, 2.0f),
                       HalfComplex(3.0f, -2.0f));

  const Half4 arg4 = FastArg(a);

  Half arg_values[4];
  arg4.Store(arg_values);

  EXPECT_NEAR(float(arg_values[0]), 0.0f, 0.005f);
  EXPECT_NEAR(float(arg_values[1]), 1.57079637f, 0.005f);
  EXPECT_NEAR(float(arg_values[2]), 2.55359006f, 0.005f);
  EXPECT_NEAR(float(arg_values[3]), -0.58800262f, 0.005f);
}

TEST(HalfComplex4, Conj) {
  const HalfComplex4 a(HalfComplex(1.0f, 0.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(-3.0f, 2.0f),
                       HalfComplex(3.0f, -2.0f));

  const HalfComplex4 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(-3.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(HalfComplex(3.0f, 2.0f), 1e-6f));
}

TEST(HalfComplex4, ComplexExp) {
  constexpr float kEpsilon = 1e-3f;

  const Half4 x(0.0, 0.1f, -0.2f, -0.3f);

  const HalfComplex4 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.9950041652780258, 0.09983341664682815f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(HalfComplex(0.9800665778412416f, -0.19866933079506122f),
                  kEpsilon));
  EXPECT_THAT(
      result.Extract<3>(),
      ComplexNear(HalfComplex(0.955336489125606f, -0.29552020666133955f),
                  kEpsilon));
}

TEST(HalfComplex4, Exp) {
  constexpr float kEpsilon = 1e-3f;

  const HalfComplex4 z(HalfComplex(0.0f, 0.0f),
                       HalfComplex(0.1f, 0.2f),
                       HalfComplex(-0.3f, 0.4f),
                       HalfComplex(0.1f, -0.2f));

  const HalfComplex4 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(HalfComplex(1.0831410796080632f, 0.21956356670825236f),
                  kEpsilon));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(0.6823387667165518f, 0.2884882034499186f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<3>(),
      ComplexNear(HalfComplex(1.0831410796080632f, -0.21956356670825236f),
                  kEpsilon));
}

TEST(HalfComplex4, Reverse) {
  const HalfComplex4 result = Reverse(HalfComplex4(HalfComplex(1.0f, 2.0f),
                                                   HalfComplex(3.0f, 4.0f),
                                                   HalfComplex(5.0f, 6.0f),
                                                   HalfComplex(7.0f, 8.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(7.0f, 8.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(HalfComplex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(HalfComplex(1.0f, 2.0f), 1e-6f));
}

TEST(HalfComplex4, FastIntPow) {
  const HalfComplex4 base(HalfComplex(0.0f, 0.0f),
                          HalfComplex(0.1f, 0.2f),
                          HalfComplex(-0.3f, 0.4f),
                          HalfComplex(-0.4f, 0.5f));
  const HalfComplex4 result = FastIntPow(base, 4);

  // >>> import numpy as np
  // >>> np.array([ 0.0+0.0j, 0.1+0.2j, -0.3+0.4j, -0.4+0.5j]) ** 4
  // array([ 0.    +0.j    , -0.0007-0.0024j, -0.0527+0.0336j, -0.1519+0.072j ])
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(0.0f, 0.0f), 1e-3f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(-0.0007f, -0.0024f), 1e-3f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(-0.0527f, 0.0336f), 1e-3f));
  EXPECT_THAT(result.Extract<3>(),
              ComplexNear(HalfComplex(-0.1519f, 0.072f), 1e-3f));
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
