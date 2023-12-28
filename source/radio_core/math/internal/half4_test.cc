// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half4.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Half4, Load) {
  {
    const Half values[4] = {2, 3, 4, 5};
    const Half4 value = Half4(values);

    EXPECT_NEAR(float(value.Extract<0>()), 2.0f, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3.0f, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4.0f, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5.0f, 1e-6f);
  }

  {
    const Half4 value = Half4(2, 3, 4, 5);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 value = Half4(2);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 2, 1e-6f);
  }
}

TEST(Half4, Store) {
  {
    const Half4 value = Half4(2, 3, 4, 5);
    Half data[4] = {0, 0, 0, 0};
    value.Store(data);
    EXPECT_NEAR(float(data[0]), 2, 1e-6f);
    EXPECT_NEAR(float(data[1]), 3, 1e-6f);
    EXPECT_NEAR(float(data[2]), 4, 1e-6f);
    EXPECT_NEAR(float(data[3]), 5, 1e-6f);
  }

  {
    const Half4 value = Half4(2, 3, 4, 5);
    Half data;

    value.Store<0>(&data);
    EXPECT_NEAR(float(data), 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(float(data), 3, 1e-6f);

    value.Store<2>(&data);
    EXPECT_NEAR(float(data), 4, 1e-6f);

    value.Store<3>(&data);
    EXPECT_NEAR(float(data), 5, 1e-6f);
  }
}

TEST(Half4, Extract) {
  {
    const Half4 value = Half4(2, 3, 4, 5);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
  }
}

TEST(Half4, ExtractLow) {
  const Half4 value = Half4(2, 3, 4, 5);

  const Half2 low = value.ExtractLow();

  EXPECT_NEAR(float(low.Extract<0>()), 2, 1e-6f);
  EXPECT_NEAR(float(low.Extract<1>()), 3, 1e-6f);
}

TEST(Half4, ExtractHigh) {
  const Half4 value = Half4(2, 3, 4, 5);

  const Half2 high = value.ExtractHigh();

  EXPECT_NEAR(float(high.Extract<0>()), 4, 1e-6f);
  EXPECT_NEAR(float(high.Extract<1>()), 5, 1e-6f);
}

TEST(Half4, ExtractXYZW) {
  {
    const Half4 value = Half4(2, 3, 4, 5);

    EXPECT_NEAR(float(value.x()), 2, 1e-6f);
    EXPECT_NEAR(float(value.y()), 3, 1e-6f);
    EXPECT_NEAR(float(value.z()), 4, 1e-6f);
    EXPECT_NEAR(float(value.w()), 5, 1e-6f);
  }
}

TEST(Half4, SetLane) {
  {
    const Half4 value = Half4(2, 3, 4, 5);
    const Half4 new_value = value.SetLane<0>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 9, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 value = Half4(2, 3, 4, 5);
    const Half4 new_value = value.SetLane<1>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 9, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 value = Half4(2, 3, 4, 5);
    const Half4 new_value = value.SetLane<2>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 9, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 value = Half4(2, 3, 4, 5);
    const Half4 new_value = value.SetLane<3>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<3>()), 9, 1e-6f);
  }
}

TEST(Half4, SetLaneXYZW) {
  {
    Half4 value = Half4(2, 3, 4, 5);
    value.x(9);

    EXPECT_NEAR(float(value.Extract<0>()), 9, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
  }

  {
    Half4 value = Half4(2, 3, 4, 5);
    value.y(9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 9, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
  }

  {
    Half4 value = Half4(2, 3, 4, 5);
    value.z(9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 9, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 5, 1e-6f);
  }

  {
    Half4 value = Half4(2, 3, 4, 5);
    value.w(9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(value.Extract<3>()), 9, 1e-6f);
  }
}

TEST(Half4, Negate) {
  {
    const Half4 result = -Half4(2, 3, 4, 5);
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -5, 1e-6f);
  }
}

TEST(Half4, MultiplyScalar) {
  // Half4 * scalar
  {
    const Half4 result = Half4(2, 3, 4, 5) * Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 10, 1e-6f);
  }

  // scalar * Half4
  {
    const Half4 result = Half(2) * Half4(2, 3, 4, 5);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 8, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 10, 1e-6f);
  }
}

TEST(Half4, DivideScalar) {
  // Half4 / scalar
  {
    const Half4 result = Half4(2, 3, 4, 5) / Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 1.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1.5f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 2.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 2.5f, 1e-6f);
  }
}

TEST(Half4, Add) {
  {
    const Half4 result = Half4(2, 3, 4, 5) + Half4(10, 20, 30, 40);
    EXPECT_NEAR(float(result.Extract<0>()), 12.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 23.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 34.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 45.0f, 1e-6f);
  }
}

TEST(Half4, Subtract) {
  {
    const Half4 result = Half4(10, 20, 30, 40) - Half4(2, 3, 4, 5);
    EXPECT_NEAR(float(result.Extract<0>()), 8.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 17.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 26.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 35.0f, 1e-6f);
  }
}

TEST(Half4, Multiply) {
  {
    const Half4 result = Half4(10, 20, 30, 40) * Half4(2, 3, 4, 5);
    EXPECT_NEAR(float(result.Extract<0>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 60.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 120.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 200.0f, 1e-6f);
  }
}

TEST(Half4, Divide) {
  {
    const Half4 result = Half4(20, 60, 120, 200) / Half4(2, 3, 4, 5);
    EXPECT_NEAR(float(result.Extract<0>()), 10.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 30.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 40.0f, 1e-6f);
  }
}

TEST(Half4, LessThan) {
  {
    const UShort4 result = Half4(2, 4, 6, 8) < Half4(1, 5, 6, 9);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffu);
    EXPECT_EQ(result.Extract<2>(), 0);
    EXPECT_EQ(result.Extract<3>(), 0xffffu);
  }
}

TEST(Half4, GreaterThan) {
  {
    const UShort4 result = Half4(2, 4, 6, 9) > Half4(1, 5, 3, 9);
    EXPECT_EQ(result.Extract<0>(), 0xffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
    EXPECT_EQ(result.Extract<2>(), 0xffffu);
    EXPECT_EQ(result.Extract<3>(), 0);
  }
}

TEST(Half4, FastLog10) {
  {
    const Half4 result = FastLog10(Half4(10, 100, 1000, 10000));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 4, 1e-6f);
  }

  {
    const Half4 result = FastLog10(Half4(1, 2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 2e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.301025390625f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 0.47705078125f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 0.60205078125f, 1e-6f);
  }
}

TEST(Half4, Abs) {
  {
    const Half4 result = Abs(Half4(0, 0, 0, 0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 0, 1e-6f);
  }

  {
    const Half4 result = Abs(Half4(2, 3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = Abs(Half4(-2, -3, -4, -5));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = Abs(Half4(-2, 3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = Abs(Half4(2, -3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = Abs(Half4(2, 3, -4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = Abs(Half4(2, 3, 4, -5));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }
}

TEST(Half4, Min) {
  {
    const Half4 result = Min(Half4(12, 13, 14, 15), Half4(21, 22, 23, 24));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
  }

  {
    const Half4 result = Min(Half4(12, 13, 14, 15), Half4(1, 22, 23, 24));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
  }

  {
    const Half4 result = Min(Half4(12, 13, 14, 15), Half4(21, 2, 23, 24));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
  }

  {
    const Half4 result = Min(Half4(12, 13, 14, 15), Half4(21, 22, 3, 24));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 15, 1e-6f);
  }

  {
    const Half4 result = Min(Half4(12, 13, 14, 15), Half4(21, 22, 23, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 4, 1e-6f);
  }
}

TEST(Half4, Max) {
  {
    const Half4 result = Max(Half4(2, 1, 4, 3), Half4(-1, -2, -3, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
  }

  {
    const Half4 result = Max(Half4(2, 1, 4, 3), Half4(5, -2, -3, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
  }

  {
    const Half4 result = Max(Half4(2, 1, 4, 3), Half4(-1, 6, -3, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
  }

  {
    const Half4 result = Max(Half4(2, 1, 4, 3), Half4(-1, -2, 7, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 7, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 3, 1e-6f);
  }

  {
    const Half4 result = Max(Half4(2, 1, 4, 3), Half4(-1, -2, -3, 8));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 8, 1e-6f);
  }
}

TEST(Half4, HorizontalMax) {
  EXPECT_NEAR(float(HorizontalMax(Half4(12, 1, 4, 3))), 12, 1e-6f);
  EXPECT_NEAR(float(HorizontalMax(Half4(2, 11, 4, 3))), 11, 1e-6f);
  EXPECT_NEAR(float(HorizontalMax(Half4(2, 1, 14, 3))), 14, 1e-6f);
  EXPECT_NEAR(float(HorizontalMax(Half4(2, 1, 4, 13))), 13, 1e-6f);
}

TEST(Half4, HorizontalSum) {
  EXPECT_NEAR(float(HorizontalSum(Half4(2, 1, 4, 3))), 10, 1e-6f);
}

TEST(Half4, MultiplyAdd) {
  const Half4 a(2, 4, 6, 8);
  const Half4 b(3, 5, 9, 2);
  const Half4 c(7, 4, 3, 2);

  const Half4 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(float(result.Extract<0>()), 23, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 24, 1e-6f);
  EXPECT_NEAR(float(result.Extract<2>()), 33, 1e-6f);
  EXPECT_NEAR(float(result.Extract<3>()), 12, 1e-6f);
}

TEST(Half4, Select) {
  const UShort4 mask(0xffffu, 0, 0xffffu, 8);
  const Half4 source1(2, 3, 4, 5);
  const Half4 source2(6, 7, 8, 9);

  const Half4 result = Select(mask, source1, source2);
  EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 7, 1e-6f);
  EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  EXPECT_NEAR(float(result.Extract<3>()), 9, 1e-6f);
}

TEST(Half4, Sign) {
  {
    const Half4 result = Sign(Half4(0.0f, -0.0f, 0.0f, -0.0f));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -1, 1e-6f);
  }

  {
    const Half4 result = Sign(Half4(2, 3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
  }

  {
    const Half4 result = Sign(Half4(-2, 3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
  }

  {
    const Half4 result = Sign(Half4(2, -3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
  }

  {
    const Half4 result = Sign(Half4(2, 3, -4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
  }

  {
    const Half4 result = Sign(Half4(2, 3, 4, -5));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -1, 1e-6f);
  }
}

TEST(Half4, CopySign) {
  {
    const Half4 result = CopySign(Half4(2, 3, 4, 5), Half4(10, 20, 30, 40));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = CopySign(Half4(2, 3, 4, 5), Half4(-10, 20, 30, 40));
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = CopySign(Half4(2, 3, 4, 5), Half4(10, -20, 30, 40));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = CopySign(Half4(2, 3, 4, 5), Half4(10, 20, -30, 40));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 5, 1e-6f);
  }

  {
    const Half4 result = CopySign(Half4(2, 3, 4, 5), Half4(10, 20, 30, -40));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), -5, 1e-6f);
  }
}

TEST(Half4, Reverse) {
  {
    const Half4 result = Reverse(Half4(1, 2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<3>()), 1, 1e-6f);
  }
}

TEST(Half4, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3, 4, 5], [3, 4, 5, 6])
  EXPECT_NEAR(float(Dot(Half4(2, 3, 4, 5), Half4(3, 4, 5, 6))), 68.0f, 1e-6f);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Half4, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5])
  EXPECT_NEAR(
      float(linalg::Norm(Half4(2, 3, 4, 5))), 7.3484692283495345f, 1e-2f);
}

TEST(Half4, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5])
  EXPECT_NEAR(
      float(linalg::Length(Half4(2, 3, 4, 5))), 7.3484692283495345f, 1e-2f);
}

TEST(Half4, Normalize) {
  {
    const Half4 result = linalg::Normalize(Half4(0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<3>()), 0, 1e-3f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3, 4, 5])
    // >>> a / numpy.linalg.norm(a)
    const Half4 result = linalg::Normalize(Half4(2, 3, 4, 5));
    EXPECT_NEAR(float(result.Extract<0>()), 0.27216553f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.40824829f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0.54433105f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<3>()), 0.68041382f, 1e-3f);
  }
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
