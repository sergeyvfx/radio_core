// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/float4.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Float4, Load) {
  {
    const float values[4] = {2, 3, 4, 5};
    const Float4 value = Float4(values);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 value = Float4(2, 3, 4, 5);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 value = Float4(2);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 2, 1e-6f);
  }
}

TEST(Float4, Store) {
  {
    const Float4 value = Float4(2, 3, 4, 5);
    float data[4] = {0, 0, 0, 0};
    value.Store(data);
    EXPECT_NEAR(data[0], 2, 1e-6f);
    EXPECT_NEAR(data[1], 3, 1e-6f);
    EXPECT_NEAR(data[2], 4, 1e-6f);
    EXPECT_NEAR(data[3], 5, 1e-6f);
  }

  {
    const Float4 value = Float4(2, 3, 4, 5);
    float data;

    value.Store<0>(&data);
    EXPECT_NEAR(data, 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(data, 3, 1e-6f);

    value.Store<2>(&data);
    EXPECT_NEAR(data, 4, 1e-6f);

    value.Store<3>(&data);
    EXPECT_NEAR(data, 5, 1e-6f);
  }
}

TEST(Float4, Extract) {
  {
    const Float4 value = Float4(2, 3, 4, 5);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
  }
}

TEST(Float4, ExtractLow) {
  const Float4 value = Float4(2, 3, 4, 5);

  const Float2 low = value.ExtractLow();

  EXPECT_NEAR(low.Extract<0>(), 2, 1e-6f);
  EXPECT_NEAR(low.Extract<1>(), 3, 1e-6f);
}

TEST(Float4, ExtractHigh) {
  const Float4 value = Float4(2, 3, 4, 5);

  const Float2 high = value.ExtractHigh();

  EXPECT_NEAR(high.Extract<0>(), 4, 1e-6f);
  EXPECT_NEAR(high.Extract<1>(), 5, 1e-6f);
}

TEST(Float4, ExtractXYZW) {
  {
    const Float4 value = Float4(2, 3, 4, 5);

    EXPECT_NEAR(value.x(), 2, 1e-6f);
    EXPECT_NEAR(value.y(), 3, 1e-6f);
    EXPECT_NEAR(value.z(), 4, 1e-6f);
    EXPECT_NEAR(value.w(), 5, 1e-6f);
  }
}

TEST(Float4, SetLane) {
  {
    const Float4 value = Float4(2, 3, 4, 5);
    const Float4 new_value = value.SetLane<0>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 9, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 value = Float4(2, 3, 4, 5);
    const Float4 new_value = value.SetLane<1>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 9, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 value = Float4(2, 3, 4, 5);
    const Float4 new_value = value.SetLane<2>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 9, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 value = Float4(2, 3, 4, 5);
    const Float4 new_value = value.SetLane<3>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 9, 1e-6f);
  }
}

TEST(Float4, SetLaneXYZW) {
  {
    Float4 value = Float4(2, 3, 4, 5);
    value.x(9);

    EXPECT_NEAR(value.Extract<0>(), 9, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
  }

  {
    Float4 value = Float4(2, 3, 4, 5);
    value.y(9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 9, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
  }

  {
    Float4 value = Float4(2, 3, 4, 5);
    value.z(9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 9, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
  }

  {
    Float4 value = Float4(2, 3, 4, 5);
    value.w(9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 9, 1e-6f);
  }
}

TEST(Float4, Negate) {
  {
    const Float4 result = -Float4(2, 3, 4, 5);
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -5, 1e-6f);
  }
}

TEST(Float4, MultiplyScalar) {
  // float4 * scalar
  {
    const Float4 result = Float4(2, 3, 4, 5) * 2;
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 10, 1e-6f);
  }

  // scalar * float4
  {
    const Float4 result = 2 * Float4(2, 3, 4, 5);
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 10, 1e-6f);
  }
}

TEST(Float4, DivideScalar) {
  // float4 / scalar
  {
    const Float4 result = Float4(2, 3, 4, 5) / 2;
    EXPECT_NEAR(result.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1.5f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 2.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 2.5f, 1e-6f);
  }
}

TEST(Float4, Add) {
  {
    const Float4 result = Float4(2, 3, 4, 5) + Float4(10, 20, 30, 40);
    EXPECT_NEAR(result.Extract<0>(), 12.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 23.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 34.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 45.0f, 1e-6f);
  }
}

TEST(Float4, Subtract) {
  {
    const Float4 result = Float4(10, 20, 30, 40) - Float4(2, 3, 4, 5);
    EXPECT_NEAR(result.Extract<0>(), 8.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 17.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 26.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 35.0f, 1e-6f);
  }
}

TEST(Float4, Multiply) {
  {
    const Float4 result = Float4(10, 20, 30, 40) * Float4(2, 3, 4, 5);
    EXPECT_NEAR(result.Extract<0>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 60.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 120.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 200.0f, 1e-6f);
  }
}

TEST(Float4, Divide) {
  {
    const Float4 result = Float4(20, 60, 120, 200) / Float4(2, 3, 4, 5);
    EXPECT_NEAR(result.Extract<0>(), 10.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 30.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 40.0f, 1e-6f);
  }
}

TEST(Float4, LessThan) {
  {
    const UInt4 result = Float4(2, 4, 6, 8) < Float4(1, 5, 6, 9);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<2>(), 0);
    EXPECT_EQ(result.Extract<3>(), 0xffffffffu);
  }
}

TEST(Float4, GreaterThan) {
  {
    const UInt4 result = Float4(2, 4, 6, 9) > Float4(1, 5, 3, 9);
    EXPECT_EQ(result.Extract<0>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
    EXPECT_EQ(result.Extract<2>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<3>(), 0);
  }
}

TEST(Float4, FastLog10) {
  {
    const Float4 result = FastLog10(Float4(10, 100, 1000, 10000));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 4, 1e-6f);
  }

  {
    const Float4 result = FastLog10(Float4(1, 2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), 0, 2e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.301029995664f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.477121254720f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.602059991328f, 1e-6f);
  }
}

TEST(Float4, Abs) {
  {
    const Float4 result = Abs(Float4(0, 0, 0, 0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0, 1e-6f);
  }

  {
    const Float4 result = Abs(Float4(2, 3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = Abs(Float4(-2, -3, -4, -5));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = Abs(Float4(-2, 3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = Abs(Float4(2, -3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = Abs(Float4(2, 3, -4, 5));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = Abs(Float4(2, 3, 4, -5));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }
}

TEST(Float4, Min) {
  {
    const Float4 result = Min(Float4(12, 13, 14, 15), Float4(21, 22, 23, 24));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
  }

  {
    const Float4 result = Min(Float4(12, 13, 14, 15), Float4(1, 22, 23, 24));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
  }

  {
    const Float4 result = Min(Float4(12, 13, 14, 15), Float4(21, 2, 23, 24));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
  }

  {
    const Float4 result = Min(Float4(12, 13, 14, 15), Float4(21, 22, 3, 24));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
  }

  {
    const Float4 result = Min(Float4(12, 13, 14, 15), Float4(21, 22, 23, 4));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 4, 1e-6f);
  }
}

TEST(Float4, Max) {
  {
    const Float4 result = Max(Float4(2, 1, 4, 3), Float4(-1, -2, -3, -4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
  }

  {
    const Float4 result = Max(Float4(2, 1, 4, 3), Float4(5, -2, -3, -4));
    EXPECT_NEAR(result.Extract<0>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
  }

  {
    const Float4 result = Max(Float4(2, 1, 4, 3), Float4(-1, 6, -3, -4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
  }

  {
    const Float4 result = Max(Float4(2, 1, 4, 3), Float4(-1, -2, 7, -4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
  }

  {
    const Float4 result = Max(Float4(2, 1, 4, 3), Float4(-1, -2, -3, 8));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 8, 1e-6f);
  }
}

TEST(Float4, HorizontalMax) {
  EXPECT_NEAR(HorizontalMax(Float4(12, 1, 4, 3)), 12, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float4(2, 11, 4, 3)), 11, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float4(2, 1, 14, 3)), 14, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float4(2, 1, 4, 13)), 13, 1e-6f);
}

TEST(Float4, HorizontalSum) {
  EXPECT_NEAR(HorizontalSum(Float4(2, 1, 4, 3)), 10, 1e-6f);
}

TEST(Float4, MultiplyAdd) {
  const Float4 a(2, 4, 6, 8);
  const Float4 b(3, 5, 9, 2);
  const Float4 c(7, 4, 3, 2);

  const Float4 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(result.Extract<0>(), 23, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 24, 1e-6f);
  EXPECT_NEAR(result.Extract<2>(), 33, 1e-6f);
  EXPECT_NEAR(result.Extract<3>(), 12, 1e-6f);
}

TEST(Float4, Select) {
  const UInt4 mask(0xffffffffu, 0, 0xffffffffu, 0);
  const Float4 source1(2, 3, 4, 5);
  const Float4 source2(6, 7, 8, 9);

  const Float4 result = Select(mask, source1, source2);
  EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 7, 1e-6f);
  EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  EXPECT_NEAR(result.Extract<3>(), 9, 1e-6f);
}

TEST(Float4, Sign) {
  {
    const Float4 result = Sign(Float4(0.0f, -0.0f, 0.0f, -0.0f));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -1, 1e-6f);
  }

  {
    const Float4 result = Sign(Float4(2, 3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
  }

  {
    const Float4 result = Sign(Float4(-2, 3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
  }

  {
    const Float4 result = Sign(Float4(2, -3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
  }

  {
    const Float4 result = Sign(Float4(2, 3, -4, 5));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
  }

  {
    const Float4 result = Sign(Float4(2, 3, 4, -5));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -1, 1e-6f);
  }
}

TEST(Float4, CopySign) {
  {
    const Float4 result = CopySign(Float4(2, 3, 4, 5), Float4(10, 20, 30, 40));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = CopySign(Float4(2, 3, 4, 5), Float4(-10, 20, 30, 40));
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = CopySign(Float4(2, 3, 4, 5), Float4(10, -20, 30, 40));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = CopySign(Float4(2, 3, 4, 5), Float4(10, 20, -30, 40));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
  }

  {
    const Float4 result = CopySign(Float4(2, 3, 4, 5), Float4(10, 20, 30, -40));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -5, 1e-6f);
  }
}

TEST(Float4, Reverse) {
  {
    const Float4 result = Reverse(Float4(1, 2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
  }
}

TEST(Float4, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3, 4, 5], [3, 4, 5, 6])
  EXPECT_NEAR(Dot(Float4(2, 3, 4, 5), Float4(3, 4, 5, 6)), 68.0f, 1e-6f);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Float4, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5])
  EXPECT_NEAR(linalg::Norm(Float4(2, 3, 4, 5)), 7.3484692283495345f, 1e-6f);
}

TEST(Float4, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5])
  EXPECT_NEAR(linalg::Length(Float4(2, 3, 4, 5)), 7.3484692283495345f, 1e-6f);
}

TEST(Float4, SquaredLength) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5]) ** 2
  EXPECT_NEAR(linalg::SquaredLength(Float4(2, 3, 4, 5)), 54.0f, 1e-6f);
}

TEST(Float4, Normalize) {
  {
    const Float4 result = linalg::Normalize(Float4(0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0, 1e-6f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3, 4, 5])
    // >>> a / numpy.linalg.norm(a)
    const Float4 result = linalg::Normalize(Float4(2, 3, 4, 5));
    EXPECT_NEAR(result.Extract<0>(), 0.27216553f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.40824829f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.54433105f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.68041382f, 1e-6f);
  }
}

}  // namespace radio_core
