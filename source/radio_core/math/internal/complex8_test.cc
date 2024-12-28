// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/complex8.h"

#include "radio_core/math/math.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/math/unittest/vectorized_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::VectorizedNear;

TEST(Complex8, Load) {
  {
    const Complex values[8] = {Complex(2, 3),
                               Complex(4, 5),
                               Complex(6, 7),
                               Complex(8, 9),
                               Complex(10, 11),
                               Complex(12, 13),
                               Complex(14, 15),
                               Complex(16, 17)};
    const Complex8 complex8(values);

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 complex8(Complex(2, 3));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(Complex(2, 3), 1e-6f));

    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(Complex(2, 3), 1e-6f));
  }

  {
    const Complex8 complex8(Float8(2, 4, 6, 8, 10, 12, 14, 16),
                            Float8(3, 5, 7, 9, 11, 13, 15, 17));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }
}

TEST(Complex8, Store) {
  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    Complex data[8];
    complex8.Store(data);

    EXPECT_THAT(data[0], ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(data[1], ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(data[2], ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(data[3], ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(data[4], ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(data[5], ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(data[6], ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(data[7], ComplexNear(Complex(16, 17), 1e-6f));
  }  // namespace radio_core

  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    Complex data;

    complex8.Store<0>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(2, 3), 1e-6f));

    complex8.Store<1>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(4, 5), 1e-6f));

    complex8.Store<2>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(6, 7), 1e-6f));

    complex8.Store<3>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(8, 9), 1e-6f));

    complex8.Store<4>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(10, 11), 1e-6f));

    complex8.Store<5>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(12, 13), 1e-6f));

    complex8.Store<6>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(14, 15), 1e-6f));

    complex8.Store<7>(&data);
    EXPECT_THAT(data, ComplexNear(Complex(16, 17), 1e-6f));
  }
}

TEST(Complex8, Extract) {
  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    EXPECT_THAT(complex8.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(complex8.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(complex8.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(complex8.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(complex8.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(complex8.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(complex8.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(complex8.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }
}

TEST(Complex8, ExtractLow) {
  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    const Complex4 low = complex8.ExtractLow();

    EXPECT_THAT(low.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(low.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(low.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(low.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  }
}

TEST(Complex8, ExtractHigh) {
  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    const Complex4 high = complex8.ExtractHigh();

    EXPECT_THAT(high.Extract<0>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(high.Extract<1>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(high.Extract<2>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(high.Extract<3>(), ComplexNear(Complex(16, 17), 1e-6f));
  }
}

TEST(Complex4, ExtractReal) {
  const Complex8 complex8(Complex(2, 3),
                          Complex(4, 5),
                          Complex(6, 7),
                          Complex(8, 9),
                          Complex(10, 11),
                          Complex(12, 13),
                          Complex(14, 15),
                          Complex(16, 17));
  EXPECT_THAT(
      complex8.ExtractReal(),
      testing::VectorizedNear(Float8(2, 4, 6, 8, 10, 12, 14, 16), 1e-6f));
}

TEST(Complex4, ExtractImag) {
  const Complex8 complex8(Complex(2, 3),
                          Complex(4, 5),
                          Complex(6, 7),
                          Complex(8, 9),
                          Complex(10, 11),
                          Complex(12, 13),
                          Complex(14, 15),
                          Complex(16, 17));
  EXPECT_THAT(
      complex8.ExtractImag(),
      testing::VectorizedNear(Float8(3, 5, 7, 9, 11, 13, 15, 17), 1e-6f));
}

TEST(Complex8, SetLane) {
  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<0>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<1>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<2>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<3>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<4>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<5>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<6>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(199, 299), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
  }

  {
    const Complex8 value(Complex(2, 3),
                         Complex(4, 5),
                         Complex(6, 7),
                         Complex(8, 9),
                         Complex(10, 11),
                         Complex(12, 13),
                         Complex(14, 15),
                         Complex(16, 17));
    const Complex8 new_value = value.SetLane<7>(Complex(199, 299));

    EXPECT_THAT(new_value.Extract<0>(), ComplexNear(Complex(2, 3), 1e-6f));
    EXPECT_THAT(new_value.Extract<1>(), ComplexNear(Complex(4, 5), 1e-6f));
    EXPECT_THAT(new_value.Extract<2>(), ComplexNear(Complex(6, 7), 1e-6f));
    EXPECT_THAT(new_value.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
    EXPECT_THAT(new_value.Extract<4>(), ComplexNear(Complex(10, 11), 1e-6f));
    EXPECT_THAT(new_value.Extract<5>(), ComplexNear(Complex(12, 13), 1e-6f));
    EXPECT_THAT(new_value.Extract<6>(), ComplexNear(Complex(14, 15), 1e-6f));
    EXPECT_THAT(new_value.Extract<7>(), ComplexNear(Complex(199, 299), 1e-6f));
  }
}

TEST(Complex8, Negate) {
  const Complex8 complex8(Complex(2, 3),
                          Complex(-4, 5),
                          Complex(6, -7),
                          Complex(-8, -9),
                          Complex(10, 11),
                          Complex(-12, 13),
                          Complex(14, -15),
                          Complex(-16, -17));
  const Complex8 neg_complex4 = -complex8;

  EXPECT_THAT(neg_complex4.Extract<0>(), ComplexNear(Complex(-2, -3), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<1>(), ComplexNear(Complex(4, -5), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<2>(), ComplexNear(Complex(-6, 7), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<3>(), ComplexNear(Complex(8, 9), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<4>(), ComplexNear(Complex(-10, -11), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<5>(), ComplexNear(Complex(12, -13), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<6>(), ComplexNear(Complex(-14, 15), 1e-6f));
  EXPECT_THAT(neg_complex4.Extract<7>(), ComplexNear(Complex(16, 17), 1e-6f));
}

TEST(Complex8, Add) {
  const Complex8 a(Complex(2, 3),
                   Complex(4, 10),
                   Complex(6, 7),
                   Complex(8, 9),
                   Complex(10, 11),
                   Complex(12, 13),
                   Complex(14, 15),
                   Complex(16, 17));
  const Complex8 b(Complex(3, 4),
                   Complex(5, 7),
                   Complex(9, 6),
                   Complex(2, 10),
                   Complex(3, 11),
                   Complex(4, 12),
                   Complex(5, 13),
                   Complex(6, 14));

  {
    const Complex8 c = a + b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(10, 19), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(13, 22), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(16, 25), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(19, 28), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(22, 31), 1e-6f));
  }

  {
    Complex8 c = a;
    c += b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(5, 7), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(9, 17), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(15, 13), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(10, 19), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(13, 22), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(16, 25), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(19, 28), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(22, 31), 1e-6f));
  }
}

TEST(Complex8, Subtract) {
  const Complex8 a(Complex(2, 3),
                   Complex(4, 10),
                   Complex(6, 7),
                   Complex(8, 9),
                   Complex(10, 11),
                   Complex(12, 13),
                   Complex(14, 15),
                   Complex(16, 17));
  const Complex8 b(Complex(3, 4),
                   Complex(5, 7),
                   Complex(9, 6),
                   Complex(2, 10),
                   Complex(3, 11),
                   Complex(4, 12),
                   Complex(5, 13),
                   Complex(6, 14));

  {
    const Complex8 c = a - b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(6, -1), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(7, 0), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(8, 1), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(9, 2), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(10, 3), 1e-6f));
  }

  {
    Complex8 c = a;
    c -= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-1, -1), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-1, 3), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(-3, 1), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(6, -1), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(7, 0), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(8, 1), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(9, 2), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(10, 3), 1e-6f));
  }
}

TEST(Complex8, MultiplyComplex) {
  const Complex8 a(Complex(2, 3),
                   Complex(4, 10),
                   Complex(6, 7),
                   Complex(8, 9),
                   Complex(10, 11),
                   Complex(12, 13),
                   Complex(14, 15),
                   Complex(16, 17));
  const Complex8 b(Complex(3, 4),
                   Complex(5, 7),
                   Complex(9, 6),
                   Complex(2, 10),
                   Complex(3, 11),
                   Complex(4, 12),
                   Complex(5, 13),
                   Complex(6, 14));

  {
    const Complex8 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(-74, 98), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(-91, 143), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(-108, 196), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(-125, 257), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(-142, 326), 1e-6f));
  }

  {
    Complex8 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(-6, 17), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(-50, 78), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(12, 99), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(-74, 98), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(-91, 143), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(-108, 196), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(-125, 257), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(-142, 326), 1e-6f));
  }
}

TEST(Complex8, MultiplyScalar) {
  const Complex8 a(Complex(2, 3),
                   Complex(4, 10),
                   Complex(6, 7),
                   Complex(8, 9),
                   Complex(10, 11),
                   Complex(12, 13),
                   Complex(14, 15),
                   Complex(16, 17));
  const Float8 b(3, 5, 9, 2, 3, 4, 5, 6);

  {
    const Complex8 c = a * b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(16, 18), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(30, 33), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(48, 52), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(70, 75), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(96, 102), 1e-6f));
  }

  {
    Complex8 c = a;
    c *= b;

    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(6, 9), 1e-6f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(20, 50), 1e-6f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(54, 63), 1e-6f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(16, 18), 1e-6f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(30, 33), 1e-6f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(48, 52), 1e-6f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(70, 75), 1e-6f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(96, 102), 1e-6f));
  }
}

TEST(Complex8, Divide) {
  const Complex8 a(Complex(-6, 17),
                   Complex(-50, 78),
                   Complex(12, 99),
                   Complex(-74, 98),
                   Complex(-91, 143),
                   Complex(-108, 196),
                   Complex(-125, 257),
                   Complex(-142, 326));

  const Complex8 b(Complex(2, 3),
                   Complex(4, 10),
                   Complex(6, 7),
                   Complex(8, 9),
                   Complex(10, 11),
                   Complex(12, 13),
                   Complex(14, 15),
                   Complex(16, 17));
  {
    const Complex8 c = a / b;

    // NOTE: On some platforms division and reciprocal does not have enough
    // prevision to grant typical 1e-6f tolerance.
    EXPECT_THAT(c.Extract<0>(), ComplexNear(Complex(3, 4), 1e-5f));
    EXPECT_THAT(c.Extract<1>(), ComplexNear(Complex(5, 7), 1e-5f));
    EXPECT_THAT(c.Extract<2>(), ComplexNear(Complex(9, 6), 1e-5f));
    EXPECT_THAT(c.Extract<3>(), ComplexNear(Complex(2, 10), 1e-5f));
    EXPECT_THAT(c.Extract<4>(), ComplexNear(Complex(3, 11), 1e-5f));
    EXPECT_THAT(c.Extract<5>(), ComplexNear(Complex(4, 12), 1e-5f));
    EXPECT_THAT(c.Extract<6>(), ComplexNear(Complex(5, 13), 1e-5f));
    EXPECT_THAT(c.Extract<7>(), ComplexNear(Complex(6, 14), 1e-5f));
  }
}

TEST(Complex8, Abs) {
  {
    const Complex8 complex8(Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0));
    const Float8 abs8 = Abs(complex8);

    EXPECT_NEAR(abs8.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<1>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<2>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<3>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<4>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<5>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<6>(), 0.0f, 1e-6f);
    EXPECT_NEAR(abs8.Extract<7>(), 0.0f, 1e-6f);
  }

  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    const Float8 abs4 = Abs(complex8);

    EXPECT_NEAR(abs4.Extract<0>(), Sqrt(13.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<1>(), Sqrt(41.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<2>(), Sqrt(85.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<3>(), Sqrt(145.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<4>(), Sqrt(221.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<5>(), Sqrt(313.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<6>(), Sqrt(421.0f), 1e-6f);
    EXPECT_NEAR(abs4.Extract<7>(), Sqrt(545.0f), 1e-6f);
  }
}

TEST(Complex8, FastAbs) {
  {
    const Complex8 complex8(Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0),
                            Complex(0));
    const Float8 fast_abs8 = FastAbs(complex8);

    EXPECT_NEAR(fast_abs8.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<1>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<2>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<3>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<4>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<5>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<6>(), 0.0f, 1e-6f);
    EXPECT_NEAR(fast_abs8.Extract<7>(), 0.0f, 1e-6f);
  }
  {
    const Complex8 complex8(Complex(2, 3),
                            Complex(4, 5),
                            Complex(6, 7),
                            Complex(8, 9),
                            Complex(10, 11),
                            Complex(12, 13),
                            Complex(14, 15),
                            Complex(16, 17));

    const Float8 fast_abs8 = FastAbs(complex8);

    EXPECT_NEAR(fast_abs8.Extract<0>(), Sqrt(13.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<1>(), Sqrt(41.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<2>(), Sqrt(85.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<3>(), Sqrt(145.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<4>(), Sqrt(221.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<5>(), Sqrt(313.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<6>(), Sqrt(421.0f), 2e-2f);
    EXPECT_NEAR(fast_abs8.Extract<7>(), Sqrt(545.0f), 4e-2f);
  }
}

TEST(Complex8, Norm) {
  const Complex8 complex8(Complex(2, 3),
                          Complex(4, 5),
                          Complex(6, 7),
                          Complex(8, 9),
                          Complex(10, 11),
                          Complex(12, 13),
                          Complex(14, 15),
                          Complex(16, 17));
  const Float8 norm8 = Norm(complex8);

  EXPECT_NEAR(norm8.Extract<0>(), 13, 1e-6f);
  EXPECT_NEAR(norm8.Extract<1>(), 41, 1e-6f);
  EXPECT_NEAR(norm8.Extract<2>(), 85, 1e-6f);
  EXPECT_NEAR(norm8.Extract<3>(), 145, 1e-6f);
  EXPECT_NEAR(norm8.Extract<4>(), 221, 1e-6f);
  EXPECT_NEAR(norm8.Extract<5>(), 313, 1e-6f);
  EXPECT_NEAR(norm8.Extract<6>(), 421, 1e-6f);
  EXPECT_NEAR(norm8.Extract<7>(), 545, 1e-6f);
}

TEST(Complex8, HorizontalSum) {
  EXPECT_THAT(HorizontalSum(Complex8(Complex(2, 3),
                                     Complex(4, 5),
                                     Complex(6, 7),
                                     Complex(8, 9),
                                     Complex(10, 11),
                                     Complex(12, 13),
                                     Complex(14, 15),
                                     Complex(16, 17))),
              ComplexNear(Complex(72, 80), 1e-6f));
}

TEST(Complex8, MultiplyAdd) {
  const Complex8 a(Complex(2, 3),
                   Complex(4, 10),
                   Complex(6, 7),
                   Complex(8, 9),
                   Complex(10, 11),
                   Complex(12, 13),
                   Complex(14, 15),
                   Complex(16, 17));
  const Complex8 b(Complex(3, 4),
                   Complex(5, 7),
                   Complex(9, 6),
                   Complex(2, 10),
                   Complex(3, 11),
                   Complex(4, 12),
                   Complex(5, 13),
                   Complex(6, 14));

  const Float8 c(7, 4, 3, 2, 10, 7, 6, 5);

  const Complex8 result = MultiplyAdd(a, b, c);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(23, 31), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(24, 38), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(33, 25), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(Complex(12, 29), 1e-6f));
  EXPECT_THAT(result.Extract<4>(), ComplexNear(Complex(40, 121), 1e-6f));
  EXPECT_THAT(result.Extract<5>(), ComplexNear(Complex(40, 97), 1e-6f));
  EXPECT_THAT(result.Extract<6>(), ComplexNear(Complex(44, 93), 1e-6f));
  EXPECT_THAT(result.Extract<7>(), ComplexNear(Complex(46, 87), 1e-6f));
}

TEST(Complex8, FastArg) {
  const Complex8 a(Complex(1.0f, 0.0f),
                   Complex(0.0f, 1.0f),
                   Complex(-3.0f, 2.0f),
                   Complex(3.0f, -2.0f),
                   Complex(3.0f, -2.0f),
                   Complex(-3.0f, 2.0f),
                   Complex(0.0f, 1.0f),
                   Complex(1.0f, 0.0f));

  const Float8 arg4 = FastArg(a);

  float arg_values[8];
  arg4.Store(arg_values);

  EXPECT_NEAR(arg_values[0], 0.0f, 0.005f);
  EXPECT_NEAR(arg_values[1], 1.57079637f, 0.005f);
  EXPECT_NEAR(arg_values[2], 2.55359006f, 0.005f);
  EXPECT_NEAR(arg_values[3], -0.58800262f, 0.005f);
}

TEST(Complex8, Conj) {
  const Complex8 a(Complex(1.0f, 0.0f),
                   Complex(0.0f, 1.0f),
                   Complex(-3.0f, 2.0f),
                   Complex(3.0f, -2.0f),
                   Complex(3.0f, -2.0f),
                   Complex(-3.0f, 2.0f),
                   Complex(0.0f, 1.0f),
                   Complex(1.0f, 0.0f));

  const Complex8 result = Conj(a);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(-3.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(Complex(3.0f, 2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<4>(), ComplexNear(Complex(3.0f, 2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<5>(), ComplexNear(Complex(-3.0f, -2.0f), 1e-6f));
  EXPECT_THAT(result.Extract<6>(), ComplexNear(Complex(0.0f, -1.0f), 1e-6f));
  EXPECT_THAT(result.Extract<7>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
}

TEST(Complex8, ComplexExp) {
  const Float8 x(0.0, 0.1f, 0.2f, 0.3f, -0.2f, -0.3f, -0.4f, -0.5f);

  const Complex8 result = ComplexExp(x);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(Complex(0.9950041652780258, 0.09983341664682815f), 1e-6f));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(Complex(0.9800665778412416f, 0.19866933079506122f), 1e-6f));
  EXPECT_THAT(
      result.Extract<3>(),
      ComplexNear(Complex(0.955336489125606f, 0.29552020666133955f), 1e-6f));
  EXPECT_THAT(
      result.Extract<4>(),
      ComplexNear(Complex(0.9800665778412416f, -0.19866933079506122f), 1e-6f));
  EXPECT_THAT(
      result.Extract<5>(),
      ComplexNear(Complex(0.955336489125606f, -0.29552020666133955f), 1e-6f));
  EXPECT_THAT(
      result.Extract<6>(),
      ComplexNear(Complex(0.9210609940028851f, -0.3894183423086505f), 1e-6f));
  EXPECT_THAT(
      result.Extract<7>(),
      ComplexNear(Complex(0.8775825618903728f, -0.479425538604203f), 1e-6f));
}

TEST(Complex8, Exp) {
  const Complex8 z(Complex(0.0f, 0.0f),
                   Complex(0.1f, 0.2f),
                   Complex(-0.3f, 0.4f),
                   Complex(-0.4f, 0.5f),
                   Complex(0.1f, -0.2f),
                   Complex(-0.3f, -0.4f),
                   Complex(-0.4f, -0.5f),
                   Complex(0.5f, -0.6f));

  const Complex8 result = Exp(z);

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(1.0f, 0.0f), 1e-6f));
  EXPECT_THAT(
      result.Extract<1>(),
      ComplexNear(Complex(1.0831410796080632f, 0.21956356670825236f), 1e-6f));
  EXPECT_THAT(
      result.Extract<2>(),
      ComplexNear(Complex(0.6823387667165518f, 0.2884882034499186f), 1e-6f));
  EXPECT_THAT(
      result.Extract<3>(),
      ComplexNear(Complex(0.588261183286429f, 0.3213685491078305f), 1e-6f));
  EXPECT_THAT(
      result.Extract<4>(),
      ComplexNear(Complex(1.0831410796080632f, -0.21956356670825236f), 1e-6f));
  EXPECT_THAT(
      result.Extract<5>(),
      ComplexNear(Complex(0.6823387667165518f, -0.2884882034499186f), 1e-6f));
  EXPECT_THAT(
      result.Extract<6>(),
      ComplexNear(Complex(0.588261183286429f, -0.3213685491078305), 1e-6f));
  EXPECT_THAT(
      result.Extract<7>(),
      ComplexNear(Complex(1.3607483837679566f, -0.930938056227126f), 1e-6f));
}

TEST(Complex8, Reverse) {
  const Complex8 result = Reverse(Complex8(Complex(1.0f, 2.0f),
                                           Complex(3.0f, 4.0f),
                                           Complex(5.0f, 6.0f),
                                           Complex(7.0f, 8.0f),
                                           Complex(9.0f, 10.0f),
                                           Complex(11.0f, 12.0f),
                                           Complex(13.0f, 14.0f),
                                           Complex(15.0f, 16.0f)));

  EXPECT_THAT(result.Extract<0>(), ComplexNear(Complex(15.0f, 16.0f), 1e-6f));
  EXPECT_THAT(result.Extract<1>(), ComplexNear(Complex(13.0f, 14.0f), 1e-6f));
  EXPECT_THAT(result.Extract<2>(), ComplexNear(Complex(11.0f, 12.0f), 1e-6f));
  EXPECT_THAT(result.Extract<3>(), ComplexNear(Complex(9.0f, 10.0f), 1e-6f));
  EXPECT_THAT(result.Extract<4>(), ComplexNear(Complex(7.0f, 8.0f), 1e-6f));
  EXPECT_THAT(result.Extract<5>(), ComplexNear(Complex(5.0f, 6.0f), 1e-6f));
  EXPECT_THAT(result.Extract<6>(), ComplexNear(Complex(3.0f, 4.0f), 1e-6f));
  EXPECT_THAT(result.Extract<7>(), ComplexNear(Complex(1.0f, 2.0f), 1e-6f));
}

}  // namespace radio_core
