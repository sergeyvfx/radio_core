// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/float3.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Float3, Load) {
  {
    const float values[4] = {2, 3, 4};
    const Float3 value = Float3(values);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 value = Float3(2, 3, 4);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 value = Float3(2);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 2, 1e-6f);
  }
}

TEST(Float3, Store) {
  {
    const Float3 value = Float3(2, 3, 4);
    float data[4] = {0, 0, 0};
    value.Store(data);
    EXPECT_NEAR(data[0], 2, 1e-6f);
    EXPECT_NEAR(data[1], 3, 1e-6f);
    EXPECT_NEAR(data[2], 4, 1e-6f);
  }

  {
    const Float3 value = Float3(2, 3, 4);
    float data;

    value.Store<0>(&data);
    EXPECT_NEAR(data, 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(data, 3, 1e-6f);

    value.Store<2>(&data);
    EXPECT_NEAR(data, 4, 1e-6f);
  }
}

TEST(Float3, Extract) {
  {
    const Float3 value = Float3(2, 3, 4);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
  }
}

TEST(Float3, ExtractXYZ) {
  {
    const Float3 value = Float3(2, 3, 4);

    EXPECT_NEAR(value.x(), 2, 1e-6f);
    EXPECT_NEAR(value.y(), 3, 1e-6f);
    EXPECT_NEAR(value.z(), 4, 1e-6f);
  }
}

TEST(Float3, SetLane) {
  {
    const Float3 value = Float3(2, 3, 4);
    const Float3 new_value = value.SetLane<0>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 9, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 value = Float3(2, 3, 4);
    const Float3 new_value = value.SetLane<1>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 9, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 value = Float3(2, 3, 4);
    const Float3 new_value = value.SetLane<2>(9);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 9, 1e-6f);
  }
}

TEST(Float3, SetLaneXYZ) {
  {
    Float3 value = Float3(2, 3, 4);
    value.x(9);

    EXPECT_NEAR(value.Extract<0>(), 9, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
  }

  {
    Float3 value = Float3(2, 3, 4);
    value.y(9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 9, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
  }

  {
    Float3 value = Float3(2, 3, 4);
    value.z(9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 9, 1e-6f);
  }
}

TEST(Float3, Negate) {
  {
    const Float3 result = -Float3(2, 3, 4);
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -4, 1e-6f);
  }
}

TEST(Float3, MultiplyScalar) {
  // float3 * scalar
  {
    const Float3 result = Float3(2, 3, 4) * 2;
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 8, 1e-6f);
  }

  // scalar * float3
  {
    const Float3 result = 2 * Float3(2, 3, 4);
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 8, 1e-6f);
  }
}

TEST(Float3, DivideScalar) {
  // float3 / scalar
  {
    const Float3 result = Float3(2, 3, 4) / 2;
    EXPECT_NEAR(result.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1.5f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 2.0f, 1e-6f);
  }
}

TEST(Float3, Add) {
  {
    const Float3 result = Float3(2, 3, 4) + Float3(10, 20, 30);
    EXPECT_NEAR(result.Extract<0>(), 12.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 23.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 34.0f, 1e-6f);
  }
}

TEST(Float3, Subtract) {
  {
    const Float3 result = Float3(10, 20, 30) - Float3(2, 3, 4);
    EXPECT_NEAR(result.Extract<0>(), 8.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 17.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 26.0f, 1e-6f);
  }
}

TEST(Float3, Multiply) {
  {
    const Float3 result = Float3(10, 20, 30) * Float3(2, 3, 4);
    EXPECT_NEAR(result.Extract<0>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 60.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 120.0f, 1e-6f);
  }
}

TEST(Float3, Divide) {
  {
    const Float3 result = Float3(20, 60, 120) / Float3(2, 3, 4);
    EXPECT_NEAR(result.Extract<0>(), 10.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 30.0f, 1e-6f);
  }
}

TEST(Float3, LessThan) {
  {
    const UInt3 result = Float3(2, 4, 6) < Float3(1, 5, 6);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<2>(), 0);
  }
}

TEST(Float3, GreaterThan) {
  {
    const UInt3 result = Float3(2, 4, 6) > Float3(1, 5, 3);
    EXPECT_EQ(result.Extract<0>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
    EXPECT_EQ(result.Extract<2>(), 0xffffffffu);
  }
}

TEST(Float3, FastLog10) {
  {
    const Float3 result = FastLog10(Float3(10, 100, 1000));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 3, 1e-6f);
  }

  {
    const Float3 result = FastLog10(Float3(1, 2, 3));
    EXPECT_NEAR(result.Extract<0>(), 0, 2e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.301029995664f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.477121254720f, 1e-6f);
  }
}

TEST(Float3, Abs) {
  {
    const Float3 result = Abs(Float3(0, 0, 0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0, 1e-6f);
  }

  {
    const Float3 result = Abs(Float3(2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Abs(Float3(-2, -3, -4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Abs(Float3(-2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Abs(Float3(2, -3, 4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Abs(Float3(2, 3, -4));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }
}

TEST(Float3, Min) {
  {
    const Float3 result = Min(Float3(12, 13, 14), Float3(21, 22, 23));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
  }

  {
    const Float3 result = Min(Float3(12, 13, 14), Float3(1, 22, 23));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
  }

  {
    const Float3 result = Min(Float3(12, 13, 14), Float3(21, 2, 23));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
  }

  {
    const Float3 result = Min(Float3(12, 13, 14), Float3(21, 22, 3));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 3, 1e-6f);
  }
}

TEST(Float3, Max) {
  {
    const Float3 result = Max(Float3(2, 1, 4), Float3(-1, -2, -3));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Max(Float3(2, 1, 4), Float3(5, -2, -3));
    EXPECT_NEAR(result.Extract<0>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Max(Float3(2, 1, 4), Float3(-1, 6, -3));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = Max(Float3(2, 1, 4), Float3(-1, -2, 7));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 7, 1e-6f);
  }
}

TEST(Float3, HorizontalMax) {
  EXPECT_NEAR(HorizontalMax(Float3(12, 1, 4)), 12, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float3(2, 11, 4)), 11, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float3(2, 1, 14)), 14, 1e-6f);
}

TEST(Float3, HorizontalSum) {
  EXPECT_NEAR(HorizontalSum(Float3(2, 1, 4)), 7, 1e-6f);
}

TEST(Float3, MultiplyAdd) {
  const Float3 a(2, 4, 6);
  const Float3 b(3, 5, 9);
  const Float3 c(7, 4, 3);

  const Float3 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(result.Extract<0>(), 23, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 24, 1e-6f);
  EXPECT_NEAR(result.Extract<2>(), 33, 1e-6f);
}

TEST(Float3, Select) {
  const UInt3 mask(0xffffffffu, 0, 8);
  const Float3 source1(2, 3, 5);
  const Float3 source2(6, 7, 9);

  const Float3 result = Select(mask, source1, source2);
  EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 7, 1e-6f);
  EXPECT_NEAR(result.Extract<2>(), 9, 1e-6f);
}

TEST(Float3, Sign) {
  {
    const Float3 result = Sign(Float3(0.0f, -0.0f, 0.0f));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
  }

  {
    const Float3 result = Sign(Float3(2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
  }

  {
    const Float3 result = Sign(Float3(-2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
  }

  {
    const Float3 result = Sign(Float3(2, -3, 4));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
  }

  {
    const Float3 result = Sign(Float3(2, 3, -4));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -1, 1e-6f);
  }
}

TEST(Float3, CopySign) {
  {
    const Float3 result = CopySign(Float3(2, 3, 4), Float3(10, 20, 30));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = CopySign(Float3(2, 3, 4), Float3(-10, 20, 30));
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = CopySign(Float3(2, 3, 4), Float3(10, -20, 30));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  }

  {
    const Float3 result = CopySign(Float3(2, 3, 4), Float3(10, 20, -30));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -4, 1e-6f);
  }
}

TEST(Float3, Reverse) {
  {
    const Float3 result = Reverse(Float3(1, 2, 3));
    EXPECT_NEAR(result.Extract<0>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
  }
}

TEST(Float3, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3, 4], [3, 4, 5])
  EXPECT_NEAR(Dot(Float3(2, 3, 4), Float3(3, 4, 5)), 38.0f, 1e-6f);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Float3, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4])
  EXPECT_NEAR(linalg::Norm(Float3(2, 3, 4)), 5.385164807134504f, 1e-6f);
}

TEST(Float3, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4])
  EXPECT_NEAR(linalg::Length(Float3(2, 3, 4)), 5.385164807134504f, 1e-6f);
}

TEST(Float3, SquaredLength) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4]) ** 2
  EXPECT_NEAR(linalg::SquaredLength(Float3(2, 3, 4)), 29.0f, 1e-6f);
}

TEST(Float3, Normalize) {
  {
    const Float3 result = linalg::Normalize(Float3(0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0, 1e-6f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3, 4])
    // >>> a / numpy.linalg.norm(a)
    const Float3 result = linalg::Normalize(Float3(2, 3, 4));
    EXPECT_NEAR(result.Extract<0>(), 0.37139068f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.55708601f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.74278135f, 1e-6f);
  }
}

}  // namespace radio_core
