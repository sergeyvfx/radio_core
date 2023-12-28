// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half3.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Half3, Load) {
  {
    const Half values[3] = {2, 3, 4};
    const Half3 value = Half3(values);

    EXPECT_NEAR(float(value.Extract<0>()), 2.0f, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3.0f, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4.0f, 1e-6f);
  }

  {
    const Half3 value = Half3(2, 3, 4);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 value = Half3(2);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 2, 1e-6f);
  }
}

TEST(Half3, Store) {
  {
    const Half3 value = Half3(2, 3, 4);
    Half data[3] = {0, 0, 0};
    value.Store(data);
    EXPECT_NEAR(float(data[0]), 2, 1e-6f);
    EXPECT_NEAR(float(data[1]), 3, 1e-6f);
    EXPECT_NEAR(float(data[2]), 4, 1e-6f);
  }

  {
    const Half3 value = Half3(2, 3, 4);
    Half data;

    value.Store<0>(&data);
    EXPECT_NEAR(float(data), 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(float(data), 3, 1e-6f);

    value.Store<2>(&data);
    EXPECT_NEAR(float(data), 4, 1e-6f);
  }
}

TEST(Half3, Extract) {
  {
    const Half3 value = Half3(2, 3, 4);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
  }
}

TEST(Half3, ExtractXYZ) {
  {
    const Half3 value = Half3(2, 3, 4);

    EXPECT_NEAR(float(value.x()), 2, 1e-6f);
    EXPECT_NEAR(float(value.y()), 3, 1e-6f);
    EXPECT_NEAR(float(value.z()), 4, 1e-6f);
  }
}

TEST(Half3, SetLane) {
  {
    const Half3 value = Half3(2, 3, 4);
    const Half3 new_value = value.SetLane<0>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 9, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 value = Half3(2, 3, 4);
    const Half3 new_value = value.SetLane<1>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 9, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 value = Half3(2, 3, 4);
    const Half3 new_value = value.SetLane<2>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<2>()), 9, 1e-6f);
  }
}

TEST(Half3, SetLaneXYZ) {
  {
    Half3 value = Half3(2, 3, 4);
    value.x(9);

    EXPECT_NEAR(float(value.Extract<0>()), 9, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
  }

  {
    Half3 value = Half3(2, 3, 4);
    value.y(9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 9, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 4, 1e-6f);
  }

  {
    Half3 value = Half3(2, 3, 4);
    value.z(9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(value.Extract<2>()), 9, 1e-6f);
  }
}

TEST(Half3, Negate) {
  {
    const Half3 result = -Half3(2, 3, 4);
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -4, 1e-6f);
  }
}

TEST(Half3, MultiplyScalar) {
  // Half3 * scalar
  {
    const Half3 result = Half3(2, 3, 4) * Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 8, 1e-6f);
  }

  // scalar * Half3
  {
    const Half3 result = Half(2) * Half3(2, 3, 4);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 8, 1e-6f);
  }
}

TEST(Half3, DivideScalar) {
  // Half3 / scalar
  {
    const Half3 result = Half3(2, 3, 4) / Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 1.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1.5f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 2.0f, 1e-6f);
  }
}

TEST(Half3, Add) {
  {
    const Half3 result = Half3(2, 3, 4) + Half3(10, 20, 30);
    EXPECT_NEAR(float(result.Extract<0>()), 12.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 23.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 34.0f, 1e-6f);
  }
}

TEST(Half3, Subtract) {
  {
    const Half3 result = Half3(10, 20, 30) - Half3(2, 3, 4);
    EXPECT_NEAR(float(result.Extract<0>()), 8.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 17.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 26.0f, 1e-6f);
  }
}

TEST(Half3, Multiply) {
  {
    const Half3 result = Half3(10, 20, 30) * Half3(2, 3, 4);
    EXPECT_NEAR(float(result.Extract<0>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 60.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 120.0f, 1e-6f);
  }
}

TEST(Half3, Divide) {
  {
    const Half3 result = Half3(20, 60, 120) / Half3(2, 3, 4);
    EXPECT_NEAR(float(result.Extract<0>()), 10.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 30.0f, 1e-6f);
  }
}

TEST(Half3, LessThan) {
  {
    const UShort3 result = Half3(2, 4, 6) < Half3(1, 5, 6);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffu);
    EXPECT_EQ(result.Extract<2>(), 0);
  }
}

TEST(Half3, GreaterThan) {
  {
    const UShort3 result = Half3(2, 4, 6) > Half3(1, 5, 3);
    EXPECT_EQ(result.Extract<0>(), 0xffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
    EXPECT_EQ(result.Extract<2>(), 0xffffu);
  }
}

TEST(Half3, FastLog10) {
  {
    const Half3 result = FastLog10(Half3(10, 100, 1000));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 3, 1e-6f);
  }

  {
    const Half3 result = FastLog10(Half3(1, 2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 2e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.301025390625f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 0.47705078125f, 1e-6f);
  }
}

TEST(Half3, Abs) {
  {
    const Half3 result = Abs(Half3(0, 0, 0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 0, 1e-6f);
  }

  {
    const Half3 result = Abs(Half3(2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Abs(Half3(-2, -3, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Abs(Half3(-2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Abs(Half3(2, -3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Abs(Half3(2, 3, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }
}

TEST(Half3, Min) {
  {
    const Half3 result = Min(Half3(12, 13, 14), Half3(21, 22, 23));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
  }

  {
    const Half3 result = Min(Half3(12, 13, 14), Half3(1, 22, 23));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
  }

  {
    const Half3 result = Min(Half3(12, 13, 14), Half3(21, 2, 23));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 14, 1e-6f);
  }

  {
    const Half3 result = Min(Half3(12, 13, 14), Half3(21, 22, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 3, 1e-6f);
  }
}

TEST(Half3, Max) {
  {
    const Half3 result = Max(Half3(2, 1, 4), Half3(-1, -2, -3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Max(Half3(2, 1, 4), Half3(5, -2, -3));
    EXPECT_NEAR(float(result.Extract<0>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Max(Half3(2, 1, 4), Half3(-1, 6, -3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = Max(Half3(2, 1, 4), Half3(-1, -2, 7));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 7, 1e-6f);
  }
}

TEST(Half3, HorizontalMax) {
  EXPECT_NEAR(float(HorizontalMax(Half3(12, 1, 4))), 12, 1e-6f);
  EXPECT_NEAR(float(HorizontalMax(Half3(2, 11, 4))), 11, 1e-6f);
  EXPECT_NEAR(float(HorizontalMax(Half3(2, 1, 14))), 14, 1e-6f);
}

TEST(Half3, HorizontalSum) {
  EXPECT_NEAR(float(HorizontalSum(Half3(2, 1, 4))), 7, 1e-6f);
}

TEST(Half3, MultiplyAdd) {
  const Half3 a(2, 4, 6);
  const Half3 b(3, 5, 9);
  const Half3 c(7, 4, 3);

  const Half3 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(float(result.Extract<0>()), 23, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 24, 1e-6f);
  EXPECT_NEAR(float(result.Extract<2>()), 33, 1e-6f);
}

TEST(Half3, Select) {
  const UShort3 mask(0xffffu, 0, 8);
  const Half3 source1(2, 3, 5);
  const Half3 source2(6, 7, 9);

  const Half3 result = Select(mask, source1, source2);
  EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 7, 1e-6f);
  EXPECT_NEAR(float(result.Extract<2>()), 9, 1e-6f);
}

TEST(Half3, Sign) {
  {
    const Half3 result = Sign(Half3(0.0f, -0.0f, 0.0f));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
  }

  {
    const Half3 result = Sign(Half3(2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
  }

  {
    const Half3 result = Sign(Half3(-2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
  }

  {
    const Half3 result = Sign(Half3(2, -3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
  }

  {
    const Half3 result = Sign(Half3(2, 3, -4));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -1, 1e-6f);
  }
}

TEST(Half3, CopySign) {
  {
    const Half3 result = CopySign(Half3(2, 3, 4), Half3(10, 20, 30));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = CopySign(Half3(2, 3, 4), Half3(-10, 20, 30));
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = CopySign(Half3(2, 3, 4), Half3(10, -20, 30));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 4, 1e-6f);
  }

  {
    const Half3 result = CopySign(Half3(2, 3, 4), Half3(10, 20, -30));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), -4, 1e-6f);
  }
}

TEST(Half3, Reverse) {
  {
    const Half3 result = Reverse(Half3(1, 2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 3, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<2>()), 1, 1e-6f);
  }
}

TEST(Half3, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3, 4], [3, 4, 5])
  EXPECT_NEAR(float(Dot(Half3(2, 3, 4), Half3(3, 4, 5))), 38.0f, 1e-6f);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Half3, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4])
  EXPECT_NEAR(float(linalg::Norm(Half3(2, 3, 4))), 5.385164807134504f, 1e-2f);
}

TEST(Half3, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4])
  EXPECT_NEAR(float(linalg::Length(Half3(2, 3, 4))), 5.385164807134504f, 1e-2f);
}

TEST(Half3, Normalize) {
  {
    const Half3 result = linalg::Normalize(Half3(0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0, 1e-3f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3, 4])
    // >>> a / numpy.linalg.norm(a)
    const Half3 result = linalg::Normalize(Half3(2, 3, 4));
    EXPECT_NEAR(float(result.Extract<0>()), 0.37139068f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.55708601f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<2>()), 0.74278135f, 1e-3f);
  }
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
