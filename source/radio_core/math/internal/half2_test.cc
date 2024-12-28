// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/half2.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Half2, Load) {
  {
    const Half values[2] = {2, 3};
    const Half2 value = Half2(values);

    EXPECT_NEAR(float(value.Extract<0>()), 2.0f, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3.0f, 1e-6f);
  }

  {
    const Half2 value = Half2(2, 3);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 value = Half2(2);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 2, 1e-6f);
  }
}

TEST(Half2, Store) {
  {
    const Half2 value = Half2(2, 3);
    Half data[2] = {0, 0};
    value.Store(data);
    EXPECT_NEAR(float(data[0]), 2, 1e-6f);
    EXPECT_NEAR(float(data[1]), 3, 1e-6f);
  }

  {
    const Half2 value = Half2(2, 3);
    Half data;

    value.Store<0>(&data);
    EXPECT_NEAR(float(data), 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(float(data), 3, 1e-6f);
  }
}

TEST(Half2, Extract) {
  {
    const Half2 value = Half2(2, 3);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
  }
}

TEST(Half2, ExtractXY) {
  {
    const Half2 value = Half2(2, 3);

    EXPECT_NEAR(float(value.x()), 2, 1e-6f);
    EXPECT_NEAR(float(value.y()), 3, 1e-6f);
  }
}

TEST(Half2, SetLane) {
  {
    const Half2 value = Half2(2, 3);
    const Half2 new_value = value.SetLane<0>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 9, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 value = Half2(2, 3);
    const Half2 new_value = value.SetLane<1>(9);

    EXPECT_NEAR(float(new_value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(new_value.Extract<1>()), 9, 1e-6f);
  }
}

TEST(Half2, SetLaneXY) {
  {
    Half2 value = Half2(2, 3);
    value.x(9);

    EXPECT_NEAR(float(value.Extract<0>()), 9, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 3, 1e-6f);
  }

  {
    Half2 value = Half2(2, 3);
    value.y(9);

    EXPECT_NEAR(float(value.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(value.Extract<1>()), 9, 1e-6f);
  }
}

TEST(Half2, Negate) {
  {
    const Half2 result = -Half2(2, 3);
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
  }
}

TEST(Half2, MultiplyScalar) {
  // Half2 * scalar
  {
    const Half2 result = Half2(2, 3) * Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
  }

  // scalar * Half2
  {
    const Half2 result = Half(2) * Half2(2, 3);
    EXPECT_NEAR(float(result.Extract<0>()), 4, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
  }
}

TEST(Half2, DivideScalar) {
  // Half2 / scalar
  {
    const Half2 result = Half2(2, 3) / Half(2);
    EXPECT_NEAR(float(result.Extract<0>()), 1.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1.5f, 1e-6f);
  }
}

TEST(Half2, Add) {
  {
    const Half2 result = Half2(2, 3) + Half2(10, 20);
    EXPECT_NEAR(float(result.Extract<0>()), 12.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 23.0f, 1e-6f);
  }
}

TEST(Half2, Subtract) {
  {
    const Half2 result = Half2(10, 20) - Half2(2, 3);
    EXPECT_NEAR(float(result.Extract<0>()), 8.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 17.0f, 1e-6f);
  }
}

TEST(Half2, Multiply) {
  {
    const Half2 result = Half2(10, 20) * Half2(2, 3);
    EXPECT_NEAR(float(result.Extract<0>()), 20.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 60.0f, 1e-6f);
  }
}

TEST(Half2, Divide) {
  {
    const Half2 result = Half2(20, 60) / Half2(2, 3);
    EXPECT_NEAR(float(result.Extract<0>()), 10.0f, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 20.0f, 1e-6f);
  }
}

TEST(Half2, LessThan) {
  {
    const UShort2 result = Half2(2, 4) < Half2(1, 5);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffu);
  }
}

TEST(Half2, GreaterThan) {
  {
    const UShort2 result = Half2(2, 4) > Half2(1, 5);
    EXPECT_EQ(result.Extract<0>(), 0xffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
  }
}

TEST(Half2, FastLog10) {
  {
    const Half2 result = FastLog10(Half2(10, 100));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
  }

  {
    const Half2 result = FastLog10(Half2(1, 2));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 2e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.301025390625f, 1e-6f);
  }
}

TEST(Half2, Abs) {
  {
    const Half2 result = Abs(Half2(0, 0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-6f);
  }

  {
    const Half2 result = Abs(Half2(2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = Abs(Half2(-2, -3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = Abs(Half2(-2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = Abs(Half2(2, -3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = Abs(Half2(2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = Abs(Half2(2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }
}

TEST(Half2, Min) {
  {
    const Half2 result = Min(Half2(12, 13), Half2(21, 22));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
  }

  {
    const Half2 result = Min(Half2(12, 13), Half2(1, 22));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 13, 1e-6f);
  }

  {
    const Half2 result = Min(Half2(12, 13), Half2(21, 2));
    EXPECT_NEAR(float(result.Extract<0>()), 12, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 2, 1e-6f);
  }
}

TEST(Half2, Max) {
  {
    const Half2 result = Max(Half2(2, 1), Half2(-1, -2));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
  }

  {
    const Half2 result = Max(Half2(2, 1), Half2(5, -2));
    EXPECT_NEAR(float(result.Extract<0>()), 5, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
  }

  {
    const Half2 result = Max(Half2(2, 1), Half2(-1, 6));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 6, 1e-6f);
  }
}

TEST(Half2, HorizontalMax) {
  EXPECT_NEAR(float(HorizontalMax(Half2(12, 1))), 12, 1e-6f);
  EXPECT_NEAR(float(HorizontalMax(Half2(2, 11))), 11, 1e-6f);
}

TEST(Half2, HorizontalSum) {
  EXPECT_NEAR(float(HorizontalSum(Half2(2, 1))), 3, 1e-6f);
}

TEST(Half2, MultiplyAdd) {
  const Half2 a(2, 4);
  const Half2 b(3, 5);
  const Half2 c(7, 4);

  const Half2 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(float(result.Extract<0>()), 23, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 24, 1e-6f);
}

TEST(Half2, Select) {
  const UShort2 mask(0xffffu, 0);
  const Half2 source1(2, 3);
  const Half2 source2(6, 7);

  const Half2 result = Select(mask, source1, source2);
  EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
  EXPECT_NEAR(float(result.Extract<1>()), 7, 1e-6f);
}

TEST(Half2, Sign) {
  {
    const Half2 result = Sign(Half2(0.0f, -0.0f));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
  }

  {
    const Half2 result = Sign(Half2(2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
  }

  {
    const Half2 result = Sign(Half2(-2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), -1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
  }

  {
    const Half2 result = Sign(Half2(2, -3));
    EXPECT_NEAR(float(result.Extract<0>()), 1, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -1, 1e-6f);
  }
}

TEST(Half2, CopySign) {
  {
    const Half2 result = CopySign(Half2(2, 3), Half2(10, 20));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = CopySign(Half2(2, 3), Half2(-10, 20));
    EXPECT_NEAR(float(result.Extract<0>()), -2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 3, 1e-6f);
  }

  {
    const Half2 result = CopySign(Half2(2, 3), Half2(10, -20));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), -3, 1e-6f);
  }
}

TEST(Half2, Reverse) {
  {
    const Half2 result = Reverse(Half2(1, 2));
    EXPECT_NEAR(float(result.Extract<0>()), 2, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 1, 1e-6f);
  }
}

TEST(Half2, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3], [3, 4])
  EXPECT_NEAR(float(Dot(Half2(2, 3), Half2(3, 4))), 18.0f, 1e-6f);
}

TEST(Half2, Sin) {
  {
    // >>> import numpy
    // >>> numpy.sin([0.0, -0.1])
    // array([ 0.        , -0.09983342])
    const Half2 result = Sin(Half2(0.0f, -0.1f));
    EXPECT_NEAR(float(result.Extract<0>()), 0.0f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), -0.09983342f, 1e-3f);
  }

  // Test values in the range from -2*pi to 2*pi.
  {
    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 2 * constants::pi_v<float>;
      const Half2 result = Sin(Half2(arg));
      EXPECT_NEAR(float(result.Extract<0>()), Sin(arg), 2e-3f) << "arg=" << arg;
    }
  }

  // Test values in the range from -20*pi to 20*pi.
  {
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 20 * constants::pi_v<float>;
      const Half2 result = Sin(Half2(arg));
      EXPECT_NEAR(float(result.Extract<0>()), Sin(arg), 2e-2f) << "arg=" << arg;
    }
  }
}

TEST(Half2, Cos) {
  {
    // >>> import numpy
    // >>> numpy.cos([0.0, -0.1])
    // array([1.        , 0.99500417])
    const Half2 result = Cos(Half2(0.0f, -0.1f));
    EXPECT_NEAR(float(result.Extract<0>()), 1.0f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.99500417f, 1e-3f);
  }

  // Test values in the range from -2*pi to 2*pi.
  {
    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 2 * constants::pi_v<float>;
      const Half2 result = Cos(Half2(arg));
      ASSERT_NEAR(float(result.Extract<0>()), Cos(arg), 2e-3f) << "arg=" << arg;
    }
  }

  // Test values in the range from -20*pi to 20*pi.
  {
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 20 * constants::pi_v<float>;
      const Half2 result = Cos(Half2(arg));
      ASSERT_NEAR(float(result.Extract<0>()), Cos(arg), 2e-2f) << "arg=" << arg;
    }
  }
}

TEST(Half2, SinCos) {
  {
    Half2 sin, cos;
    SinCos(Half2(0.0f, -0.1f), sin, cos);

    // >>> import numpy
    // >>> numpy.sin([0.0, -0.1])
    // array([ 0.        , -0.09983342])
    EXPECT_NEAR(float(sin.Extract<0>()), 0.0f, 1e-3f);
    EXPECT_NEAR(float(sin.Extract<1>()), -0.09983342f, 1e-3f);

    // >>> import numpy
    // >>> numpy.cos([0.0, -0.1])
    // array([1.        , 0.99500417])
    EXPECT_NEAR(float(cos.Extract<0>()), 1.0f, 1e-3f);
    EXPECT_NEAR(float(cos.Extract<1>()), 0.99500417f, 1e-3f);
  }
}

TEST(Half2, Exp) {
  {
    // >>> import numpy
    // >>> numpy.exp([0.0, -0.1])
    // array([1.        , 0.90483742])
    const Half2 result = Exp(Half2(0.0f, -0.1f));
    EXPECT_NEAR(float(result.Extract<0>()), 1.0f, 1e-3f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.90483742f, 1e-3f);
  }

  // Test values in the range from -5 to 5.
  {
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 5.0f;
      const float actual = float(Exp(Half2(arg)).Extract<0>());
      const float expected = Exp(arg);
      ASSERT_NEAR(actual, expected, 4e-1f) << "arg=" << arg;
      ASSERT_LE(Abs((actual - expected) / expected), 1e-2f) << "arg=" << arg;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Half2, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3])
  EXPECT_NEAR(float(linalg::Norm(Half2(2, 3))), 3.605551275463989f, 1e-2f);
}

TEST(Half2, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3])
  EXPECT_NEAR(float(linalg::Length(Half2(2, 3))), 3.605551275463989f, 1e-2f);
}

TEST(Half2, Normalize) {
  {
    const Half2 result = linalg::Normalize(Half2(0));
    EXPECT_NEAR(float(result.Extract<0>()), 0, 1e-6f);
    EXPECT_NEAR(float(result.Extract<1>()), 0, 1e-6f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3])
    // >>> a / numpy.linalg.norm(a)
    const Half2 result = linalg::Normalize(Half2(2, 3));
    EXPECT_NEAR(float(result.Extract<0>()), 0.5547002f, 1e-4f);
    EXPECT_NEAR(float(result.Extract<1>()), 0.83205029f, 1e-4f);
  }
}

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
