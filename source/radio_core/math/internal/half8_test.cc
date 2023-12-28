// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/half8.h"

#  include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Half8, Load) {
  {
    const Half values[8] = {2, 3, 4, 5, 6, 7, 8, 9};
    const Half8 value = Half8(values);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<4>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<5>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<6>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<7>()), 2, 1e-6f);
  }

  {
    const Half8 value = Half8(Half4(2, 3, 4, 5), Half4(6, 7, 8, 9));

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(value.Extract<7>()), 9, 1e-6f);
  }
}

TEST(Half8, Store) {
  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    Half data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    value.Store(data);
    EXPECT_NEAR(float(data[0]), 2, 1e-6f);
    EXPECT_NEAR(float(data[1]), 3, 1e-6f);
    EXPECT_NEAR(float(data[2]), 4, 1e-6f);
    EXPECT_NEAR(float(data[3]), 5, 1e-6f);
    EXPECT_NEAR(float(data[4]), 6, 1e-6f);
    EXPECT_NEAR(float(data[5]), 7, 1e-6f);
    EXPECT_NEAR(float(data[6]), 8, 1e-6f);
    EXPECT_NEAR(float(data[7]), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    Half data;

    value.Store<0>(&data);
    EXPECT_NEAR(float(data), 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(float(data), 3, 1e-6f);

    value.Store<2>(&data);
    EXPECT_NEAR(float(data), 4, 1e-6f);

    value.Store<3>(&data);
    EXPECT_NEAR(float(data), 5, 1e-6f);

    value.Store<4>(&data);
    EXPECT_NEAR(float(data), 6, 1e-6f);

    value.Store<5>(&data);
    EXPECT_NEAR(float(data), 7, 1e-6f);

    value.Store<6>(&data);
    EXPECT_NEAR(float(data), 8, 1e-6f);

    value.Store<7>(&data);
    EXPECT_NEAR(float(data), 9, 1e-6f);
  }
}

TEST(Half8, Extract) {
  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(value.Extract<7>()), 9, 1e-6f);
  }
}

TEST(Half8, ExtractLow) {
  const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);

  const Half4 low = value.ExtractLow();

  EXPECT_NEAR(float(low.Extract<0>()), 2, 1e-6f);
  EXPECT_NEAR(float(low.Extract<1>()), 3, 1e-6f);
  EXPECT_NEAR(float(low.Extract<2>()), 4, 1e-6f);
  EXPECT_NEAR(float(low.Extract<3>()), 5, 1e-6f);
}

TEST(Half8, ExtractHigh) {
  const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);

  const Half4 low = value.ExtractHigh();

  EXPECT_NEAR(float(low.Extract<0>()), 6, 1e-6f);
  EXPECT_NEAR(float(low.Extract<1>()), 7, 1e-6f);
  EXPECT_NEAR(float(low.Extract<2>()), 8, 1e-6f);
  EXPECT_NEAR(float(low.Extract<3>()), 9, 1e-6f);
}

TEST(Half8, SetLane) {
  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<0>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<1>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<2>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<3>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<4>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<5>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<6>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 99, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 value = Half8(2, 3, 4, 5, 6, 7, 8, 9);
    const Half8 new_value = value.SetLane<7>(99);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<7>()), 99, 1e-6f);
  }
}

TEST(Half8, Negate) {
  {
    const Half8 result = -Half8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), -6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), -7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), -8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), -9, 1e-6f);
  }
}

TEST(Half8, MultiplyScalar) {
  // Half8 * scalar
  {
    const Half8 result = Half8(2, 3, 4, 5, 6, 7, 8, 9) * Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 10, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 18, 1e-6f);
  }

  // scalar * Half8
  {
    const Half8 result = Half(2) * Half8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 10, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 18, 1e-6f);
  }
}

TEST(Half8, DivideScalar) {
  // Half8 / scalar
  {
    const Half8 result = Half8(2, 3, 4, 5, 6, 7, 8, 9) / Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 1.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1.5f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 2.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 2.5f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 3.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 3.5f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 4.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 4.5f, 1e-6f);
  }
}

TEST(Half8, Add) {
  {
    const Half8 result =
        Half8(2, 3, 4, 5, 6, 7, 8, 9) + Half8(10, 20, 30, 40, 50, 60, 70, 80);
    EXPECT_NEAR(float(result.Extract<0>()), 12.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 23.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 34.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 45.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 56.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 67.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 78.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 89.0f, 1e-6f);
  }
}

TEST(Half8, Subtract) {
  {
    const Half8 result =
        Half8(10, 20, 30, 40, 50, 60, 70, 80) - Half8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(float(result.Extract<0>()), 8.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 17.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 26.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 35.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 44.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 53.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 62.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 71.0f, 1e-6f);
  }
}

TEST(Half8, Multiply) {
  {
    const Half8 result =
        Half8(10, 20, 30, 40, 50, 60, 70, 80) * Half8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(float(result.Extract<0>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 60.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 120.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 200.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 300.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 420.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 560.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 720.0f, 1e-6f);
  }
}

TEST(Half8, Divide) {
  {
    const Half8 result = Half8(20, 60, 120, 200, 300, 420, 560, 720) /
                         Half8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(float(result.Extract<0>()), 10.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 30.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 40.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 50.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 60.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 70.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 80.0f, 1e-6f);
  }
}

TEST(Half8, LessThan) {
  {
    const UShort8 result =
        Half8(2, 4, 6, 8, 10, 12, 14, 16) < Half8(1, 5, 6, 9, 8, 13, 12, 18);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffu);
    EXPECT_EQ(result.Extract<2>(), 0);
    EXPECT_EQ(result.Extract<3>(), 0xffffu);
    EXPECT_EQ(result.Extract<4>(), 0);
    EXPECT_EQ(result.Extract<5>(), 0xffffu);
    EXPECT_EQ(result.Extract<6>(), 0);
    EXPECT_EQ(result.Extract<7>(), 0xffffu);
  }
}

TEST(Half8, GreaterThan) {
  {
    const UShort8 result =
        Half8(2, 4, 6, 9, 10, 12, 14, 16) > Half8(1, 5, 3, 9, 8, 13, 12, 18);
    EXPECT_EQ(result.Extract<0>(), 0xffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
    EXPECT_EQ(result.Extract<2>(), 0xffffu);
    EXPECT_EQ(result.Extract<3>(), 0);
    EXPECT_EQ(result.Extract<4>(), 0xffffu);
    EXPECT_EQ(result.Extract<5>(), 0);
    EXPECT_EQ(result.Extract<6>(), 0xffffu);
    EXPECT_EQ(result.Extract<7>(), 0);
  }
}

TEST(Half8, FastLog10) {
  {
    const Half8 result =
        FastLog10(Half8(10, 100, 1000, 10000, 1, 10, 100, 1000));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 3, 1e-6f);
  }

  {
    const Half8 result = FastLog10(Half8(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.301025390625f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0.47705078125f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<3>()), 0.60205078125f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<4>()), 0.69921875f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<5>()), 0.7783203125f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<6>()), 0.8447265625f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<7>()), 0.9033203125f, 1e-3f);
  }
}

TEST(Half8, Abs) {
  {
    const Half8 result = Abs(Half8(0, 0, 0, 0, 0, 0, 0, 0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 0, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(-2, -3, -4, -5, -6, -7, -8, -9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(-2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, -3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, -4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, 4, -5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, 4, 5, -6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, 4, 5, 6, -7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, 4, 5, 6, 7, -8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = Abs(Half8(2, 3, 4, 5, 6, 7, 8, -9));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }
}

TEST(Half8, Min) {
  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 23, 24, 25, 26, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(1, 22, 23, 24, 25, 26, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 2, 23, 24, 25, 26, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 3, 24, 25, 26, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 23, 4, 25, 26, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 23, 24, 5, 26, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 23, 24, 25, 6, 27, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 23, 24, 25, 26, 7, 28));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 19, 1e-6f);
  }

  {
    const Half8 result = Min(Half8(12, 13, 14, 15, 16, 17, 18, 19),
                             Half8(21, 22, 23, 24, 25, 26, 27, 8));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 16, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 17, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 18, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 8, 1e-6f);
  }
}

TEST(Half8, Max) {
  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, -3, -4, -5, -6, -7, -8));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(5, -2, -3, -4, -5, -6, -7, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, 6, -3, -4, -5, -6, -7, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, 7, -4, -5, -6, -7, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, -3, 8, -5, -6, -7, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, -3, -4, 10, -6, -7, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 10, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, -3, -4, -5, 10, -7, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 10, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, -3, -4, -5, -6, 10, -8));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 10, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 7, 1e-6f);
  }

  {
    const Half8 result = Max(Half8(2, 1, 4, 3, 6, 5, 8, 7),
                             Half8(-1, -2, -3, -4, -5, -6, -7, 10));

    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 10, 1e-6f);
  }
}

TEST(Half8, HorizontalMax) {
  EXPECT_NEAR(
      float(HorizontalMax(Half8(12, 3, 4, 5, 6, 7, 8, 9))), 12.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 13, 4, 5, 6, 7, 8, 9))), 13.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 3, 14, 5, 6, 7, 8, 9))), 14.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 3, 4, 15, 6, 7, 8, 9))), 15.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 3, 4, 5, 16, 7, 8, 9))), 16.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 3, 4, 5, 6, 17, 8, 9))), 17.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 3, 4, 5, 6, 7, 18, 9))), 18.0f, 1e-6f);
  EXPECT_NEAR(
      float(HorizontalMax(Half8(2, 3, 4, 5, 6, 7, 8, 19))), 19.0f, 1e-6f);
}

TEST(Half8, HorizontalSum) {
  EXPECT_NEAR(float(HorizontalSum(Half8(2, 1, 4, 3, 6, 5, 8, 7))), 36, 1e-6f);
}

TEST(Half8, MultiplyAdd) {
  const Half8 a(2, 4, 6, 8, 3, 5, 9, 2);
  const Half8 b(3, 5, 9, 2, 7, 4, 3, 2);
  const Half8 c(7, 4, 3, 2, 2, 4, 6, 8);

  const Half8 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(float(result.Extract<0>()), 23, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 24, 1e-6f);
  EXPECT_NEAR(float(result.Extract<2>()), 33, 1e-6f);
  EXPECT_NEAR(float(result.Extract<3>()), 12, 1e-6f);
  EXPECT_NEAR(float(result.Extract<4>()), 17, 1e-6f);
  EXPECT_NEAR(float(result.Extract<5>()), 21, 1e-6f);
  EXPECT_NEAR(float(result.Extract<6>()), 27, 1e-6f);
  EXPECT_NEAR(float(result.Extract<7>()), 18, 1e-6f);
}

TEST(Half8, Select) {
  const UShort8 mask(0xffffu, 0, 0xffffu, 0, 0xffffu, 0, 0xffffu, 8);
  const Half8 source1(2, 3, 4, 5, 10, 11, 12, 13);
  const Half8 source2(6, 7, 8, 9, 14, 15, 16, 17);

  const Half8 result = Select(mask, source1, source2);
  EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 7, 1e-6f);
  EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  EXPECT_NEAR(float(result.Extract<3>()), 9, 1e-6f);
  EXPECT_NEAR(float(result.Extract<4>()), 10, 1e-6f);
  EXPECT_NEAR(float(result.Extract<5>()), 15, 1e-6f);
  EXPECT_NEAR(float(result.Extract<6>()), 12, 1e-6f);
  EXPECT_NEAR(float(result.Extract<7>()), 17, 1e-6f);
}

TEST(Half8, Sign) {
  {
    const Half8 result =
        Sign(Half8(0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), -1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(-2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, -3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, -4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, 4, -5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, 4, 5, -6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, 4, 5, 6, -7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, 4, 5, 6, 7, -8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }

  {
    const Half8 result = Sign(Half8(2, 3, 4, 5, 6, 7, 8, -9));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), -1, 1e-6f);
  }
}

TEST(Half8, CopySign) {
  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, 30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(-10, 20, 30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, -20, 30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, -30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, 30, -40, 50, 60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, 30, 40, -50, 60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), -6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, 30, 40, 50, -60, 70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), -7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, 30, 40, 50, 60, -70, 80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), -8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 9, 1e-6f);
  }

  {
    const Half8 result = CopySign(Half8(2, 3, 4, 5, 6, 7, 8, 9),
                                  Half8(10, 20, 30, 40, 50, 60, 70, -80));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), -9, 1e-6f);
  }
}

TEST(Half8, Reverse) {
  {
    const Half8 result = Reverse(Half8(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_NEAR(float(result.Extract<0>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<4>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<5>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<6>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<7>()), 1, 1e-6f);
  }
}

TEST(Half8, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3, 4, 5, 6, 7, 8, 9], [3, 4, 5, 6, 7, 8, 9, 10])
  EXPECT_NEAR(
      float(Dot(Half8(2, 3, 4, 5, 6, 7, 8, 9), Half8(3, 4, 5, 6, 7, 8, 9, 10))),
      328.0f,
      1e-6f);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Half8, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5, 6, 7, 8, 9])
  EXPECT_NEAR(float(linalg::Norm(Half8(2, 3, 4, 5, 6, 7, 8, 9))),
              16.852299546352718f,
              1e-2f);
}

TEST(Half8, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5, 6, 7, 8, 9])
  EXPECT_NEAR(float(linalg::Length(Half8(2, 3, 4, 5, 6, 7, 8, 9))),
              16.852299546352718f,
              1e-2f);
}

TEST(Half8, Normalize) {
  {
    const Half8 result = linalg::Normalize(Half8(0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<3>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<4>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<5>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<6>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<7>()), 0, 1e-3f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3, 4, 5, 6, 7, 8, 9])
    // >>> a / numpy.linalg.norm(a)
    const Half8 result = linalg::Normalize(Half8(2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(float(result.Extract<0>()), 0.11867817f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.17801725f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0.23735633f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<3>()), 0.29669541f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<4>()), 0.3560345f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<5>()), 0.41537358f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<6>()), 0.47471266f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<7>()), 0.53405175f, 1e-3f);
  }
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
