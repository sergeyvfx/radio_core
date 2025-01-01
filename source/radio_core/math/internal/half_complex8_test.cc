// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half_complex8.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/math.h"
#  include "radio_core/math/unittest/complex_matchers.h"
#  include "radio_core/math/unittest/vectorized_matchers.h"
#  include "radio_core/unittest/mock.h"
#  include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

TEST(HalfComplex8, Load) {
  {
    const HalfComplex values[8] = {HalfComplex(2, 3),
                                   HalfComplex(4, 5),
                                   HalfComplex(6, 7),
                                   HalfComplex(8, 9),
                                   HalfComplex(10, 11),
                                   HalfComplex(12, 13),
                                   HalfComplex(14, 15),
                                   HalfComplex(16, 17)};
    const HalfComplex8 complex8(values);

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 complex8(HalfComplex(2, 3));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(HalfComplex(2, 3), 1e-6f));

    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
  }

  {
    const HalfComplex8 complex8(Half8(2, 4, 6, 8, 10, 12, 14, 16),
                                Half8(3, 5, 7, 9, 11, 13, 15, 17));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 complex8(123);

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(HalfComplex(123, 0), 1e-6f));
  }
}

TEST(HalfComplex8, Store) {
  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    HalfComplex data[8];
    complex8.Store(data);

    EXPECT_THAT(data[0], ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(data[3], ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(data[4], ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(data[5], ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(data[6], ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(data[7], ComplexNear(HalfComplex(16, 17), 1e-6f));
  }  // namespace radio_core

  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    HalfComplex data;

    complex8.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(2, 3), 1e-6f));

    complex8.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(4, 5), 1e-6f));

    complex8.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(6, 7), 1e-6f));

    complex8.Store<3>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(8, 9), 1e-6f));

    complex8.Store<4>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(10, 11), 1e-6f));

    complex8.Store<5>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(12, 13), 1e-6f));

    complex8.Store<6>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(14, 15), 1e-6f));

    complex8.Store<7>(&data);
    EXPECT_THAT(data, ComplexNear(HalfComplex(16, 17), 1e-6f));
  }
}

TEST(HalfComplex8, Extract) {
  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(HalfComplex(16, 17), 1e-6f));
  }
}

TEST(HalfComplex8, ExtractLow) {
  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    const HalfComplex4 low = complex8.ExtractLow();

    EXPECT_THAT(low.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(low.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(low.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(low.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  }
}

TEST(HalfComplex8, ExtractHigh) {
  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    const HalfComplex4 high = complex8.ExtractHigh();

    EXPECT_THAT(high.Extract<0>(), ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(high.Extract<1>(), ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(high.Extract<2>(), ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(high.Extract<3>(), ComplexNear(HalfComplex(16, 17), 1e-6f));
  }
}

TEST(HalfComplex8, ExtractReal) {
  const HalfComplex8 complex8(HalfComplex(2, 3),
                              HalfComplex(4, 5),
                              HalfComplex(6, 7),
                              HalfComplex(8, 9),
                              HalfComplex(10, 11),
                              HalfComplex(12, 13),
                              HalfComplex(14, 15),
                              HalfComplex(16, 17));
  EXPECT_THAT(
      complex8.ExtractReal(),
      testing::VectorizedNear<Half>(Half8(2, 4, 6, 8, 10, 12, 14, 16), 1e-6f));
}

TEST(HalfComplex8, ExtractImag) {
  const HalfComplex8 complex8(HalfComplex(2, 3),
                              HalfComplex(4, 5),
                              HalfComplex(6, 7),
                              HalfComplex(8, 9),
                              HalfComplex(10, 11),
                              HalfComplex(12, 13),
                              HalfComplex(14, 15),
                              HalfComplex(16, 17));
  EXPECT_THAT(
      complex8.ExtractImag(),
      testing::VectorizedNear<Half>(Half8(3, 5, 7, 9, 11, 13, 15, 17), 1e-6f));
}

TEST(HalfComplex8, SetLane) {
  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<0>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<1>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<2>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<3>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<4>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<5>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<6>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(16, 17), 1e-6f));
  }

  {
    const HalfComplex8 value(HalfComplex(2, 3),
                             HalfComplex(4, 5),
                             HalfComplex(6, 7),
                             HalfComplex(8, 9),
                             HalfComplex(10, 11),
                             HalfComplex(12, 13),
                             HalfComplex(14, 15),
                             HalfComplex(16, 17));
    const HalfComplex8 new_value = value.SetLane<7>(HalfComplex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(HalfComplex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(HalfComplex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(HalfComplex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(),
                ComplexNear(HalfComplex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(),
                ComplexNear(HalfComplex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(),
                ComplexNear(HalfComplex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(),
                ComplexNear(HalfComplex(199, 299), 1e-6f));
  }
}

TEST(HalfComplex8, Negate) {
  const HalfComplex8 complex8(HalfComplex(2, 3),
                              HalfComplex(-4, 5),
                              HalfComplex(6, -7),
                              HalfComplex(-8, -9),
                              HalfComplex(10, 11),
                              HalfComplex(-12, 13),
                              HalfComplex(14, -15),
                              HalfComplex(-16, -17));
  const HalfComplex8 neg_complex4 = -complex8;

  EXPECT_THAT(neg_complex4.Extract<0>(),
              ComplexNear(HalfComplex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<1>(),
              ComplexNear(HalfComplex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<2>(),
              ComplexNear(HalfComplex(-6, 7), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<3>(), ComplexNear(HalfComplex(8, 9), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<4>(),
              ComplexNear(HalfComplex(-10, -11), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<5>(),
              ComplexNear(HalfComplex(12, -13), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<6>(),
              ComplexNear(HalfComplex(-14, 15), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<7>(),
              ComplexNear(HalfComplex(16, 17), 1e-6f));
}

TEST(HalfComplex8, Add) {
  const HalfComplex8 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9),
                       HalfComplex(10, 11),
                       HalfComplex(12, 13),
                       HalfComplex(14, 15),
                       HalfComplex(16, 17));
  const HalfComplex8 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10),
                       HalfComplex(3, 11),
                       HalfComplex(4, 12),
                       HalfComplex(5, 13),
                       HalfComplex(6, 14));

  {
    const HalfComplex8 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(10, 19), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(13, 22), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(16, 25), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(19, 28), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(22, 31), 1e-6f));
  }

  {
    HalfComplex8 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(10, 19), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(13, 22), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(16, 25), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(19, 28), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(22, 31), 1e-6f));
  }
}

TEST(HalfComplex8, Subtract) {
  const HalfComplex8 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9),
                       HalfComplex(10, 11),
                       HalfComplex(12, 13),
                       HalfComplex(14, 15),
                       HalfComplex(16, 17));
  const HalfComplex8 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10),
                       HalfComplex(3, 11),
                       HalfComplex(4, 12),
                       HalfComplex(5, 13),
                       HalfComplex(6, 14));

  {
    const HalfComplex8 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(6, -1), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(7, 0), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(8, 1), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(9, 2), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(10, 3), 1e-6f));
  }

  {
    HalfComplex8 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(6, -1), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(7, 0), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(8, 1), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(9, 2), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(10, 3), 1e-6f));
  }
}

TEST(HalfComplex8, MultiplyComplex) {
  const HalfComplex8 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9),
                       HalfComplex(10, 11),
                       HalfComplex(12, 13),
                       HalfComplex(14, 15),
                       HalfComplex(16, 17));
  const HalfComplex8 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10),
                       HalfComplex(3, 11),
                       HalfComplex(4, 12),
                       HalfComplex(5, 13),
                       HalfComplex(6, 14));

  {
    const HalfComplex8 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(-74, 98), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(-91, 143), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(-108, 196), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(-125, 257), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(-142, 326), 1e-6f));
  }

  {
    HalfComplex8 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(-74, 98), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(-91, 143), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(-108, 196), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(-125, 257), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(-142, 326), 1e-6f));
  }
}

TEST(HalfComplex8, MultiplyScalar) {
  const HalfComplex8 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9),
                       HalfComplex(10, 11),
                       HalfComplex(12, 13),
                       HalfComplex(14, 15),
                       HalfComplex(16, 17));
  const Half8 b(3, 5, 9, 2, 3, 4, 5, 6);

  {
    const HalfComplex8 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(16, 18), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(30, 33), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(48, 52), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(70, 75), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(96, 102), 1e-6f));
  }

  {
    HalfComplex8 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(16, 18), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(30, 33), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(48, 52), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(70, 75), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(96, 102), 1e-6f));
  }
}

TEST(HalfComplex8, Divide) {
  const HalfComplex8 a(HalfComplex(-6, 17),
                       HalfComplex(-50, 78),
                       HalfComplex(12, 99),
                       HalfComplex(-74, 98),
                       HalfComplex(-91, 143),
                       HalfComplex(-108, 196),
                       HalfComplex(-125, 257),
                       HalfComplex(-142, 326));

  const HalfComplex8 b(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9),
                       HalfComplex(10, 11),
                       HalfComplex(12, 13),
                       HalfComplex(14, 15),
                       HalfComplex(16, 17));
  {
    const HalfComplex8 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(HalfComplex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(HalfComplex(5, 7), 1e-5f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(HalfComplex(9, 6), 1e-5f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(HalfComplex(2, 10), 1e-5f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(HalfComplex(3, 11), 1e-5f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(HalfComplex(4, 12), 1e-5f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(HalfComplex(5, 13), 1e-2f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(HalfComplex(6, 14), 1e-2f));
  }
}

TEST(HalfComplex8, Abs) {
  {
    const HalfComplex8 complex8(HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0));

    const Half8 abs8 = Abs(complex8);

    EXPECT_NEAR(float(abs8.Extract<0>()), 0, 1e-3f);
    EXPECT_NEAR(float(abs8.Extract<1>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs8.Extract<2>()), 0, 4e-3f);
    EXPECT_NEAR(float(abs8.Extract<3>()), 0, 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<4>()), 0, 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<5>()), 0, 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<6>()), 0, 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<7>()), 0, 2e-2f);
  }

  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    const Half8 abs8 = Abs(complex8);

    EXPECT_NEAR(float(abs8.Extract<0>()), Sqrt(13.0f), 1e-3f);
    EXPECT_NEAR(float(abs8.Extract<1>()), Sqrt(41.0f), 4e-3f);
    EXPECT_NEAR(float(abs8.Extract<2>()), Sqrt(85.0f), 4e-3f);
    EXPECT_NEAR(float(abs8.Extract<3>()), Sqrt(145.0f), 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<4>()), Sqrt(221.0f), 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<5>()), Sqrt(313.0f), 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<6>()), Sqrt(421.0f), 6e-3f);
    EXPECT_NEAR(float(abs8.Extract<7>()), Sqrt(545.0f), 2e-2f);
  }
}

TEST(HalfComplex8, FastAbs) {
  {
    const HalfComplex8 complex8(HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0),
                                HalfComplex(0));

    const Half8 fast_abs8 = FastAbs(complex8);

    EXPECT_NEAR(float(fast_abs8.Extract<0>()), 0, 1e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<1>()), 0, 4e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<2>()), 0, 4e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<3>()), 0, 6e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<4>()), 0, 6e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<5>()), 0, 6e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<6>()), 0, 6e-3f);
    EXPECT_NEAR(float(fast_abs8.Extract<7>()), 0, 2e-2f);
  }

  {
    const HalfComplex8 complex8(HalfComplex(2, 3),
                                HalfComplex(4, 5),
                                HalfComplex(6, 7),
                                HalfComplex(8, 9),
                                HalfComplex(10, 11),
                                HalfComplex(12, 13),
                                HalfComplex(14, 15),
                                HalfComplex(16, 17));

    const Half8 fast_abs8 = FastAbs(complex8);

    EXPECT_NEAR(float(fast_abs8.Extract<0>()), Sqrt(13.0f), 1e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<1>()), Sqrt(41.0f), 1e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<2>()), Sqrt(85.0f), 1e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<3>()), Sqrt(145.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<4>()), Sqrt(221.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<5>()), Sqrt(313.0f), 2e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<6>()), Sqrt(421.0f), 4e-2f);
    EXPECT_NEAR(float(fast_abs8.Extract<7>()), Sqrt(545.0f), 4e-2f);
  }
}

TEST(HalfComplex8, Norm) {
  const HalfComplex8 complex8(HalfComplex(2, 3),
                              HalfComplex(4, 5),
                              HalfComplex(6, 7),
                              HalfComplex(8, 9),
                              HalfComplex(10, 11),
                              HalfComplex(12, 13),
                              HalfComplex(14, 15),
                              HalfComplex(16, 17));
  const Half8 norm8 = Norm(complex8);

  EXPECT_NEAR(float(norm8.Extract<0>()), 13, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<1>()), 41, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<2>()), 85, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<3>()), 145, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<4>()), 221, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<5>()), 313, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<6>()), 421, 1e-6f);
  EXPECT_NEAR(float(norm8.Extract<7>()), 545, 1e-6f);
}

TEST(HalfComplex8, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(HalfComplex8(HalfComplex(2, 3),
                                         HalfComplex(4, 5),
                                         HalfComplex(6, 7),
                                         HalfComplex(8, 9),
                                         HalfComplex(10, 11),
                                         HalfComplex(12, 13),
                                         HalfComplex(14, 15),
                                         HalfComplex(16, 17))),
              ComplexNear(HalfComplex(72, 80), 1e-6f));
}

TEST(HalfComplex8, MultiplyAdd) {
  const HalfComplex8 a(HalfComplex(2, 3),
                       HalfComplex(4, 10),
                       HalfComplex(6, 7),
                       HalfComplex(8, 9),
                       HalfComplex(10, 11),
                       HalfComplex(12, 13),
                       HalfComplex(14, 15),
                       HalfComplex(16, 17));
  const HalfComplex8 b(HalfComplex(3, 4),
                       HalfComplex(5, 7),
                       HalfComplex(9, 6),
                       HalfComplex(2, 10),
                       HalfComplex(3, 11),
                       HalfComplex(4, 12),
                       HalfComplex(5, 13),
                       HalfComplex(6, 14));

  const Half8 c(7, 4, 3, 2, 10, 7, 6, 5);

  const HalfComplex8 result = MultiplyAdd(a, b, c);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(HalfComplex(24, 38), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(HalfComplex(33, 25), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(HalfComplex(12, 29), 1e-6f));
  EXPECT_THAT(result.Extract<4>(), ComplexNear(HalfComplex(40, 121), 1e-6f));
  EXPECT_THAT(result.Extract<5>(), ComplexNear(HalfComplex(40, 97), 1e-6f));
  EXPECT_THAT(result.Extract<6>(), ComplexNear(HalfComplex(44, 93), 1e-6f));
  EXPECT_THAT(result.Extract<7>(), ComplexNear(HalfComplex(46, 87), 1e-6f));
}

TEST(HalfComplex8, FastArg) {
  const HalfComplex8 a(HalfComplex(1.0f, 0.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(-3.0f, 2.0f),
                       HalfComplex(3.0f, -2.0f),
                       HalfComplex(3.0f, -2.0f),
                       HalfComplex(-3.0f, 2.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(1.0f, 0.0f));

  const Half8 arg8 = FastArg(a);

  Half arg_values[8];
  arg8.Store(arg_values);

  EXPECT_NEAR(float(arg_values[0]), 0.0f, 0.005f);
  EXPECT_NEAR(float(arg_values[1]), 1.57079637f, 0.005f);
  EXPECT_NEAR(float(arg_values[2]), 2.55359006f, 0.005f);
  EXPECT_NEAR(float(arg_values[3]), -0.58800262f, 0.005f);
}

TEST(HalfComplex8, Conj) {
  const HalfComplex8 a(HalfComplex(1.0f, 0.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(-3.0f, 2.0f),
                       HalfComplex(3.0f, -2.0f),
                       HalfComplex(3.0f, -2.0f),
                       HalfComplex(-3.0f, 2.0f),
                       HalfComplex(0.0f, 1.0f),
                       HalfComplex(1.0f, 0.0f));

  const HalfComplex8 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(-3.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(HalfComplex(3.0f, 2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<4>(), ComplexNear(HalfComplex(3.0f, 2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<5>(),
              ComplexNear(HalfComplex(-3.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<6>(),
              ComplexNear(HalfComplex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<7>(), ComplexNear(HalfComplex(1.0f, 0.0f), 1e-6f));
}

TEST(HalfComplex8, ComplexExp) {
  constexpr float kEpsilon = 1e-3f;

  const Half8 x(0.0, 0.1f, 0.2f, 0.3f, -0.2f, -0.3f, -0.4f, -0.5f);

  const HalfComplex8 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(0.9950041652780258, 0.09983341664682815f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(HalfComplex(0.9800665778412416f, 0.19866933079506122f),
                  kEpsilon));
  EXPECT_THAT(result.Extract<3>(),
              ComplexNear(HalfComplex(0.955336489125606f, 0.29552020666133955f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<4>(),
      ComplexNear(HalfComplex(0.9800665778412416f, -0.19866933079506122f),
                  kEpsilon));
  EXPECT_THAT(
      result.Extract<5>(),
      ComplexNear(HalfComplex(0.955336489125606f, -0.29552020666133955f),
                  kEpsilon));
  EXPECT_THAT(
      result.Extract<6>(),
      ComplexNear(HalfComplex(0.9210609940028851f, -0.3894183423086505f),
                  kEpsilon));
  EXPECT_THAT(result.Extract<7>(),
              ComplexNear(HalfComplex(0.8775825618903728f, -0.479425538604203f),
                          kEpsilon));
}

TEST(HalfComplex8, Exp) {
  constexpr float kEpsilon = 1e-3f;

  const HalfComplex8 z(HalfComplex(0.0f, 0.0f),
                       HalfComplex(0.1f, 0.2f),
                       HalfComplex(-0.3f, 0.4f),
                       HalfComplex(-0.4f, 0.5f),
                       HalfComplex(0.1f, -0.2f),
                       HalfComplex(-0.3f, -0.4f),
                       HalfComplex(-0.4f, -0.5f),
                       HalfComplex(0.5f, -0.6f));

  const HalfComplex8 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(1.0f, 0.0f), kEpsilon));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(HalfComplex(1.0831410796080632f, 0.21956356670825236f),
                  kEpsilon));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(0.6823387667165518f, 0.2884882034499186f),
                          kEpsilon));
  EXPECT_THAT(result.Extract<3>(),
              ComplexNear(HalfComplex(0.588261183286429f, 0.3213685491078305f),
                          kEpsilon));
  EXPECT_THAT(
      result.Extract<4>(),
      ComplexNear(HalfComplex(1.0831410796080632f, -0.21956356670825236f),
                  kEpsilon));
  EXPECT_THAT(
      result.Extract<5>(),
      ComplexNear(HalfComplex(0.6823387667165518f, -0.2884882034499186f),
                  kEpsilon));
  EXPECT_THAT(result.Extract<6>(),
              ComplexNear(HalfComplex(0.588261183286429f, -0.3213685491078305),
                          kEpsilon));
  EXPECT_THAT(result.Extract<7>(),
              ComplexNear(HalfComplex(1.3607483837679566f, -0.930938056227126f),
                          kEpsilon));
}

TEST(HalfComplex8, Reverse) {
  const HalfComplex8 result = Reverse(HalfComplex8(HalfComplex(1.0f, 2.0f),
                                                   HalfComplex(3.0f, 4.0f),
                                                   HalfComplex(5.0f, 6.0f),
                                                   HalfComplex(7.0f, 8.0f),
                                                   HalfComplex(9.0f, 10.0f),
                                                   HalfComplex(11.0f, 12.0f),
                                                   HalfComplex(13.0f, 14.0f),
                                                   HalfComplex(15.0f, 16.0f)));

  EXPECT_THAT(result.Extract<0>(),
              ComplexNear(HalfComplex(15.0f, 16.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(13.0f, 14.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(11.0f, 12.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(),
              ComplexNear(HalfComplex(9.0f, 10.0f), 1e-6f));
  EXPECT_THAT(result.Extract<4>(), ComplexNear(HalfComplex(7.0f, 8.0f), 1e-6f));
  EXPECT_THAT(result.Extract<5>(), ComplexNear(HalfComplex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<6>(), ComplexNear(HalfComplex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<7>(), ComplexNear(HalfComplex(1.0f, 2.0f), 1e-6f));
}

TEST(HalfComplex8, FastIntPow) {
  const HalfComplex8 base(HalfComplex(0.0f, 0.0f),
                          HalfComplex(0.1f, 0.2f),
                          HalfComplex(-0.3f, 0.4f),
                          HalfComplex(-0.4f, 0.5f),
                          HalfComplex(0.1f, -0.2f),
                          HalfComplex(-0.3f, -0.4f),
                          HalfComplex(-0.4f, -0.5f),
                          HalfComplex(0.5f, -0.6f));
  const HalfComplex8 result = FastIntPow(base, 4);

  // >>> import numpy as np
  // >>> np.array([ 0.0+0.0j, 0.1+0.2j, -0.3+0.4j, -0.4+0.5j, 0.1-0.2j,
  // ...           -0.3-0.4j, -0.4-0.5j, 0.5-0.6j]) ** 4
  // array([ 0.    +0.j    , -0.0007-0.0024j, -0.0527+0.0336j, -0.1519+0.072j ,
  //        -0.0007+0.0024j, -0.0527-0.0336j, -0.1519-0.072j , -0.3479+0.132j ])
  EXPECT_THAT(result.Extract<0>(), ComplexNear(HalfComplex(0.0f, 0.0f), 1e-3f));
  EXPECT_THAT(result.Extract<1>(),
              ComplexNear(HalfComplex(-0.0007f, -0.0024f), 1e-3f));
  EXPECT_THAT(result.Extract<2>(),
              ComplexNear(HalfComplex(-0.0527f, 0.0336f), 1e-3f));
  EXPECT_THAT(result.Extract<3>(),
              ComplexNear(HalfComplex(-0.1519f, 0.072f), 1e-3f));
  EXPECT_THAT(result.Extract<4>(),
              ComplexNear(HalfComplex(-0.0007f, 0.0024f), 1e-3f));
  EXPECT_THAT(result.Extract<5>(),
              ComplexNear(HalfComplex(-0.0527f, -0.0336f), 1e-3f));
  EXPECT_THAT(result.Extract<6>(),
              ComplexNear(HalfComplex(-0.1519f, -0.072f), 1e-3f));
  EXPECT_THAT(result.Extract<7>(),
              ComplexNear(HalfComplex(-0.3479, 0.132f), 1e-3f));
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
