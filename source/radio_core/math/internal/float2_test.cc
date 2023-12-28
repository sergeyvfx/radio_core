// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/float2.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Float2, Load) {
  {
    const float values[2] = {2, 3};
    const Float2 value = Float2(values);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 value = Float2(2, 3);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 value = Float2(2);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 2, 1e-6f);
  }
}

TEST(Float2, Store) {
  {
    const Float2 value = Float2(2, 3);
    float data[2] = {0, 0};
    value.Store(data);
    EXPECT_NEAR(data[0], 2, 1e-6f);
    EXPECT_NEAR(data[1], 3, 1e-6f);
  }

  {
    const Float2 value = Float2(2, 3);
    float data;

    value.Store<0>(&data);
    EXPECT_NEAR(data, 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(data, 3, 1e-6f);
  }
}

TEST(Float2, Extract) {
  {
    const Float2 value = Float2(2, 3);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
  }
}

TEST(Float2, ExtractXY) {
  {
    const Float2 value = Float2(2, 3);

    EXPECT_NEAR(value.x(), 2, 1e-6f);
    EXPECT_NEAR(value.y(), 3, 1e-6f);
  }
}

TEST(Float2, SetLane) {
  {
    const Float2 value = Float2(2, 3);
    const Float2 new_value = value.SetLane<0>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 9, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 value = Float2(2, 3);
    const Float2 new_value = value.SetLane<1>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 9, 1e-6f);
  }
}

TEST(Float2, SetLaneXY) {
  {
    Float2 value = Float2(2, 3);
    value.x(9);

    EXPECT_NEAR(value.Extract<0>(), 9, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
  }

  {
    Float2 value = Float2(2, 3);
    value.y(9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 9, 1e-6f);
  }
}

TEST(Float2, Negate) {
  {
    const Float2 result = -Float2(2, 3);
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
  }
}

TEST(Float2, MultiplyScalar) {
  // float2 * scalar
  {
    const Float2 result = Float2(2, 3) * 2;
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
  }

  // scalar * float2
  {
    const Float2 result = 2 * Float2(2, 3);
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
  }
}

TEST(Float2, DivideScalar) {
  // float2 / scalar
  {
    const Float2 result = Float2(2, 3) / 2;
    EXPECT_NEAR(result.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1.5f, 1e-6f);
  }
}

TEST(Float2, Add) {
  {
    const Float2 result = Float2(2, 3) + Float2(10, 20);
    EXPECT_NEAR(result.Extract<0>(), 12.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 23.0f, 1e-6f);
  }
}

TEST(Float2, Subtract) {
  {
    const Float2 result = Float2(10, 20) - Float2(2, 3);
    EXPECT_NEAR(result.Extract<0>(), 8.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 17.0f, 1e-6f);
  }
}

TEST(Float2, Multiply) {
  {
    const Float2 result = Float2(10, 20) * Float2(2, 3);
    EXPECT_NEAR(result.Extract<0>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 60.0f, 1e-6f);
  }
}

TEST(Float2, Divide) {
  {
    const Float2 result = Float2(20, 60) / Float2(2, 3);
    EXPECT_NEAR(result.Extract<0>(), 10.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 20.0f, 1e-6f);
  }
}

TEST(Float2, LessThan) {
  {
    const UInt2 result = Float2(2, 4) < Float2(1, 5);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffffffu);
  }
}

TEST(Float2, GreaterThan) {
  {
    const UInt2 result = Float2(2, 4) > Float2(1, 5);
    EXPECT_EQ(result.Extract<0>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
  }
}

TEST(Float2, FastLog10) {
  {
    const Float2 result = FastLog10(Float2(10, 100));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
  }

  {
    const Float2 result = FastLog10(Float2(1, 2));
    EXPECT_NEAR(result.Extract<0>(), 0, 2e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.301029995664f, 1e-6f);
  }
}

TEST(Float2, Abs) {
  {
    const Float2 result = Abs(Float2(0, 0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
  }

  {
    const Float2 result = Abs(Float2(2, 3));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 result = Abs(Float2(-2, -3));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 result = Abs(Float2(-2, 3));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 result = Abs(Float2(2, -3));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
  }
}

TEST(Float2, Min) {
  {
    const Float2 result = Min(Float2(12, 13), Float2(21, 22));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
  }

  {
    const Float2 result = Min(Float2(12, 13), Float2(1, 22));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
  }

  {
    const Float2 result = Min(Float2(12, 13), Float2(21, 2));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
  }
}

TEST(Float2, Max) {
  {
    const Float2 result = Max(Float2(2, 1), Float2(-1, -2));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
  }

  {
    const Float2 result = Max(Float2(2, 1), Float2(5, -2));
    EXPECT_NEAR(result.Extract<0>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
  }

  {
    const Float2 result = Max(Float2(2, 1), Float2(-1, 6));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
  }
}

TEST(Float2, HorizontalMax) {
  EXPECT_NEAR(HorizontalMax(Float2(12, 1)), 12, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float2(2, 11)), 11, 1e-6f);
}

TEST(Float2, HorizontalSum) {
  EXPECT_NEAR(HorizontalSum(Float2(2, 1)), 3, 1e-6f);
}

TEST(Float2, MultiplyAdd) {
  const Float2 a(2, 4);
  const Float2 b(3, 5);
  const Float2 c(7, 4);

  const Float2 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(result.Extract<0>(), 23, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 24, 1e-6f);
}

TEST(Float2, Select) {
  const UInt2 mask(0xffffffffu, 0);
  const Float2 source1(2, 3);
  const Float2 source2(6, 7);

  const Float2 result = Select(mask, source1, source2);
  EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 7, 1e-6f);
}

TEST(Float2, Sign) {
  {
    const Float2 result = Sign(Float2(0.0f, -0.0f));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
  }

  {
    const Float2 result = Sign(Float2(2, 3));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
  }

  {
    const Float2 result = Sign(Float2(-2, 3));
    EXPECT_NEAR(result.Extract<0>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
  }

  {
    const Float2 result = Sign(Float2(2, -3));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
  }
}

TEST(Float2, CopySign) {
  {
    const Float2 result = CopySign(Float2(2, 3), Float2(10, 20));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 result = CopySign(Float2(2, 3), Float2(-10, 20));
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
  }

  {
    const Float2 result = CopySign(Float2(2, 3), Float2(10, -20));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
  }
}

TEST(Float2, Reverse) {
  {
    const Float2 result = Reverse(Float2(1, 2));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
  }
}

TEST(Float2, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3], [3, 4])
  EXPECT_NEAR(Dot(Float2(2, 3), Float2(3, 4)), 18.0f, 1e-6f);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Float2, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3])
  EXPECT_NEAR(linalg::Norm(Float2(2, 3)), 3.605551275463989f, 1e-6f);
}

TEST(Float2, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3])
  EXPECT_NEAR(linalg::Length(Float2(2, 3)), 3.605551275463989f, 1e-6f);
}

TEST(Float2, SquaredLength) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3]) ** 2
  EXPECT_NEAR(linalg::SquaredLength(Float2(2, 3)), 13.0f, 1e-6f);
}

TEST(Float2, Normalize) {
  {
    const Float2 result = linalg::Normalize(Float2(0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3])
    // >>> a / numpy.linalg.norm(a)
    const Float2 result = linalg::Normalize(Float2(2, 3));
    EXPECT_NEAR(result.Extract<0>(), 0.5547002f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.83205029f, 1e-6f);
  }
}

}  // namespace radio_core
