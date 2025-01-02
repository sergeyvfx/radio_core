// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/float8.h"

#include "radio_core/base/build_config.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Float8, Load) {
  {
    const float values[8] = {2, 3, 4, 5, 6, 7, 8, 9};
    const Float8 value = Float8(values);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<4>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<5>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<6>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<7>(), 2, 1e-6f);
  }

  {
    const Float8 value = Float8(Float4(2, 3, 4, 5), Float4(6, 7, 8, 9));

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(value.Extract<7>(), 9, 1e-6f);
  }
}

TEST(Float8, Store) {
  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    float data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    value.Store(data);
    EXPECT_NEAR(data[0], 2, 1e-6f);
    EXPECT_NEAR(data[1], 3, 1e-6f);
    EXPECT_NEAR(data[2], 4, 1e-6f);
    EXPECT_NEAR(data[3], 5, 1e-6f);
    EXPECT_NEAR(data[4], 6, 1e-6f);
    EXPECT_NEAR(data[5], 7, 1e-6f);
    EXPECT_NEAR(data[6], 8, 1e-6f);
    EXPECT_NEAR(data[7], 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    float data;

    value.Store<0>(&data);
    EXPECT_NEAR(data, 2, 1e-6f);

    value.Store<1>(&data);
    EXPECT_NEAR(data, 3, 1e-6f);

    value.Store<2>(&data);
    EXPECT_NEAR(data, 4, 1e-6f);

    value.Store<3>(&data);
    EXPECT_NEAR(data, 5, 1e-6f);

    value.Store<4>(&data);
    EXPECT_NEAR(data, 6, 1e-6f);

    value.Store<5>(&data);
    EXPECT_NEAR(data, 7, 1e-6f);

    value.Store<6>(&data);
    EXPECT_NEAR(data, 8, 1e-6f);

    value.Store<7>(&data);
    EXPECT_NEAR(data, 9, 1e-6f);
  }
}

TEST(Float8, Extract) {
  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);

    EXPECT_NEAR(value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(value.Extract<7>(), 9, 1e-6f);
  }
}

TEST(Float8, ExtractLow) {
  const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);

  const Float4 low = value.ExtractLow();

  EXPECT_NEAR(low.Extract<0>(), 2, 1e-6f);
  EXPECT_NEAR(low.Extract<1>(), 3, 1e-6f);
  EXPECT_NEAR(low.Extract<2>(), 4, 1e-6f);
  EXPECT_NEAR(low.Extract<3>(), 5, 1e-6f);
}

TEST(Float8, ExtractHigh) {
  const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);

  const Float4 high = value.ExtractHigh();

  EXPECT_NEAR(high.Extract<0>(), 6, 1e-6f);
  EXPECT_NEAR(high.Extract<1>(), 7, 1e-6f);
  EXPECT_NEAR(high.Extract<2>(), 8, 1e-6f);
  EXPECT_NEAR(high.Extract<3>(), 9, 1e-6f);
}

TEST(Float8, SetLane) {
  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<0>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<1>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<2>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<3>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<4>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<5>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<6>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 99, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 value = Float8(2, 3, 4, 5, 6, 7, 8, 9);
    const Float8 new_value = value.SetLane<7>(99);

    EXPECT_NEAR(new_value.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(new_value.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(new_value.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(new_value.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(new_value.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(new_value.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(new_value.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(new_value.Extract<7>(), 99, 1e-6f);
  }
}

TEST(Float8, Negate) {
  {
    const Float8 result = -Float8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), -6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), -7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), -8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), -9, 1e-6f);
  }
}

TEST(Float8, MultiplyScalar) {
  // float8 * scalar
  {
    const Float8 result = Float8(2, 3, 4, 5, 6, 7, 8, 9) * 2;
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 10, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 18, 1e-6f);
  }

  // scalar * float8
  {
    const Float8 result = 2 * Float8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(result.Extract<0>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 10, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 18, 1e-6f);
  }
}

TEST(Float8, DivideScalar) {
  // float8 / scalar
  {
    const Float8 result = Float8(2, 3, 4, 5, 6, 7, 8, 9) / 2;
    EXPECT_NEAR(result.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1.5f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 2.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 2.5f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 3.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 3.5f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 4.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 4.5f, 1e-6f);
  }
}

TEST(Float8, Add) {
  {
    const Float8 result =
        Float8(2, 3, 4, 5, 6, 7, 8, 9) + Float8(10, 20, 30, 40, 50, 60, 70, 80);
    EXPECT_NEAR(result.Extract<0>(), 12.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 23.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 34.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 45.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 56.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 67.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 78.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 89.0f, 1e-6f);
  }
}

TEST(Float8, Subtract) {
  {
    const Float8 result =
        Float8(10, 20, 30, 40, 50, 60, 70, 80) - Float8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(result.Extract<0>(), 8.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 17.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 26.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 35.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 44.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 53.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 62.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 71.0f, 1e-6f);
  }
}

TEST(Float8, Multiply) {
  {
    const Float8 result =
        Float8(10, 20, 30, 40, 50, 60, 70, 80) * Float8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(result.Extract<0>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 60.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 120.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 200.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 300.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 420.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 560.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 720.0f, 1e-6f);
  }
}

TEST(Float8, Divide) {
  {
    const Float8 result = Float8(20, 60, 120, 200, 300, 420, 560, 720) /
                          Float8(2, 3, 4, 5, 6, 7, 8, 9);
    EXPECT_NEAR(result.Extract<0>(), 10.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 20.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 30.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 40.0f, 1e-6f);

#if ARCH_CPU_32_BITS && ARCH_CPU_ARM_FAMILY
    // 32bit ARM uses an approximation for vdivq_f32 which has lower accuracy
    // with larger difference between numerator and denominator. The
    // implementation is verified against _mm_div_ps() from sse2nwon which uses
    // the same number of Netwon-Raphson steps.
    //
    // Tested on 32bit Raspbian Lite 12 running on Raspberry Pi 5.
    EXPECT_NEAR(result.Extract<4>(), 50.0f, 4e-6f);
    EXPECT_NEAR(result.Extract<5>(), 60.0f, 4e-6f);
    EXPECT_NEAR(result.Extract<6>(), 70.0f, 4e-6f);
    EXPECT_NEAR(result.Extract<7>(), 80.0f, 4e-6f);
#else
    EXPECT_NEAR(result.Extract<4>(), 50.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 60.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 70.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 80.0f, 1e-6f);
#endif
  }
}

TEST(Float8, LessThan) {
  {
    const UInt8 result =
        Float8(2, 4, 6, 8, 10, 12, 14, 16) < Float8(1, 5, 6, 9, 8, 13, 12, 18);
    EXPECT_EQ(result.Extract<0>(), 0);
    EXPECT_EQ(result.Extract<1>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<2>(), 0);
    EXPECT_EQ(result.Extract<3>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<4>(), 0);
    EXPECT_EQ(result.Extract<5>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<6>(), 0);
    EXPECT_EQ(result.Extract<7>(), 0xffffffffu);
  }
}

TEST(Float8, GreaterThan) {
  {
    const UInt8 result =
        Float8(2, 4, 6, 9, 10, 12, 14, 16) > Float8(1, 5, 3, 9, 8, 13, 12, 18);
    EXPECT_EQ(result.Extract<0>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<1>(), 0);
    EXPECT_EQ(result.Extract<2>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<3>(), 0);
    EXPECT_EQ(result.Extract<4>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<5>(), 0);
    EXPECT_EQ(result.Extract<6>(), 0xffffffffu);
    EXPECT_EQ(result.Extract<7>(), 0);
  }
}

TEST(Float8, FastLog10) {
  {
    const Float8 result =
        FastLog10(Float8(10, 100, 1000, 10000, 1, 10, 100, 1000));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 3, 1e-6f);
  }

  {
    const Float8 result = FastLog10(Float8(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_NEAR(result.Extract<0>(), 0, 2e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.301029995664f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.477121254720f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.602059991328f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 0.6989700043360f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 0.77815125038364f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 0.84509804001425f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 0.90308998699194f, 1e-6f);
  }
}

TEST(Float8, Abs) {
  {
    const Float8 result = Abs(Float8(0, 0, 0, 0, 0, 0, 0, 0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 0, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(-2, -3, -4, -5, -6, -7, -8, -9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(-2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, -3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, -4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, 4, -5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, 4, 5, -6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, 4, 5, 6, -7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, 4, 5, 6, 7, -8, 9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = Abs(Float8(2, 3, 4, 5, 6, 7, 8, -9));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }
}

TEST(Float8, Min) {
  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 23, 24, 25, 26, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(1, 22, 23, 24, 25, 26, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 2, 23, 24, 25, 26, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 3, 24, 25, 26, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 23, 4, 25, 26, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 23, 24, 5, 26, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 23, 24, 25, 6, 27, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 23, 24, 25, 26, 7, 28));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 19, 1e-6f);
  }

  {
    const Float8 result = Min(Float8(12, 13, 14, 15, 16, 17, 18, 19),
                              Float8(21, 22, 23, 24, 25, 26, 27, 8));
    EXPECT_NEAR(result.Extract<0>(), 12, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 13, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 14, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 15, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 16, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 17, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 18, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 8, 1e-6f);
  }
}

TEST(Float8, Max) {
  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, -3, -4, -5, -6, -7, -8));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(5, -2, -3, -4, -5, -6, -7, -8));

    EXPECT_NEAR(result.Extract<0>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, 6, -3, -4, -5, -6, -7, -8));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, 7, -4, -5, -6, -7, -8));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, -3, 8, -5, -6, -7, -8));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, -3, -4, 10, -6, -7, -8));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 10, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, -3, -4, -5, 10, -7, -8));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 10, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, -3, -4, -5, -6, 10, -8));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 10, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 7, 1e-6f);
  }

  {
    const Float8 result = Max(Float8(2, 1, 4, 3, 6, 5, 8, 7),
                              Float8(-1, -2, -3, -4, -5, -6, -7, 10));

    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 10, 1e-6f);
  }
}

TEST(Float8, HorizontalMax) {
  EXPECT_NEAR(HorizontalMax(Float8(12, 3, 4, 5, 6, 7, 8, 9)), 12.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 13, 4, 5, 6, 7, 8, 9)), 13.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 3, 14, 5, 6, 7, 8, 9)), 14.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 3, 4, 15, 6, 7, 8, 9)), 15.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 3, 4, 5, 16, 7, 8, 9)), 16.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 3, 4, 5, 6, 17, 8, 9)), 17.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 3, 4, 5, 6, 7, 18, 9)), 18.0f, 1e-6f);
  EXPECT_NEAR(HorizontalMax(Float8(2, 3, 4, 5, 6, 7, 8, 19)), 19.0f, 1e-6f);
}

TEST(Float8, HorizontalSum) {
  EXPECT_NEAR(HorizontalSum(Float8(2, 1, 4, 3, 6, 5, 8, 7)), 36, 1e-6f);
}

TEST(Float8, MultiplyAdd) {
  const Float8 a(2, 4, 6, 8, 3, 5, 9, 2);
  const Float8 b(3, 5, 9, 2, 7, 4, 3, 2);
  const Float8 c(7, 4, 3, 2, 2, 4, 6, 8);

  const Float8 result = MultiplyAdd(a, b, c);
  EXPECT_NEAR(result.Extract<0>(), 23, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 24, 1e-6f);
  EXPECT_NEAR(result.Extract<2>(), 33, 1e-6f);
  EXPECT_NEAR(result.Extract<3>(), 12, 1e-6f);
  EXPECT_NEAR(result.Extract<4>(), 17, 1e-6f);
  EXPECT_NEAR(result.Extract<5>(), 21, 1e-6f);
  EXPECT_NEAR(result.Extract<6>(), 27, 1e-6f);
  EXPECT_NEAR(result.Extract<7>(), 18, 1e-6f);
}

TEST(Float8, Select) {
  const UInt8 mask(
      0xffffffffu, 0, 0xffffffffu, 0, 0, 0xffffffffu, 0, 0xffffffffu);
  const Float8 source1(2, 3, 4, 5, 10, 11, 12, 13);
  const Float8 source2(6, 7, 8, 9, 14, 15, 16, 17);

  const Float8 result = Select(mask, source1, source2);
  EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
  EXPECT_NEAR(result.Extract<1>(), 7, 1e-6f);
  EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
  EXPECT_NEAR(result.Extract<3>(), 9, 1e-6f);
  EXPECT_NEAR(result.Extract<4>(), 14, 1e-6f);
  EXPECT_NEAR(result.Extract<5>(), 11, 1e-6f);
  EXPECT_NEAR(result.Extract<6>(), 16, 1e-6f);
  EXPECT_NEAR(result.Extract<7>(), 13, 1e-6f);
}

TEST(Float8, Sign) {
  {
    const Float8 result =
        Sign(Float8(0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), -1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(-2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, -3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, -4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, 4, -5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, 4, 5, -6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, 4, 5, 6, -7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, 4, 5, 6, 7, -8, 9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), -1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }

  {
    const Float8 result = Sign(Float8(2, 3, 4, 5, 6, 7, 8, -9));
    EXPECT_NEAR(result.Extract<0>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), -1, 1e-6f);
  }
}

TEST(Float8, CopySign) {
  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, 30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(-10, 20, 30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), -2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, -20, 30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), -3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, -30, 40, 50, 60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), -4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, 30, -40, 50, 60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), -5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, 30, 40, -50, 60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), -6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, 30, 40, 50, -60, 70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), -7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, 30, 40, 50, 60, -70, 80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), -8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 9, 1e-6f);
  }

  {
    const Float8 result = CopySign(Float8(2, 3, 4, 5, 6, 7, 8, 9),
                                   Float8(10, 20, 30, 40, 50, 60, 70, -80));
    EXPECT_NEAR(result.Extract<0>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), -9, 1e-6f);
  }
}

TEST(Float8, Reverse) {
  {
    const Float8 result = Reverse(Float8(1, 2, 3, 4, 5, 6, 7, 8));
    EXPECT_NEAR(result.Extract<0>(), 8, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 7, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 6, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 5, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 4, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 3, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 2, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 1, 1e-6f);
  }
}

TEST(Float8, Dot) {
  // >>> import numpy
  // >>> numpy.dot([2, 3, 4, 5, 6, 7, 8, 9], [3, 4, 5, 6, 7, 8, 9, 10])
  EXPECT_NEAR(
      Dot(Float8(2, 3, 4, 5, 6, 7, 8, 9), Float8(3, 4, 5, 6, 7, 8, 9, 10)),
      328.0f,
      1e-6f);
}

TEST(Float8, Sin) {
  {
    // >>> import numpy
    // >>> numpy.sin([0.0, 0.1, 0.2, 0.3, -0.1, -0.2, -0.3, -0.4])
    // array([ 0.        ,  0.09983342,  0.19866933,  0.29552021, -0.09983342,
    //        -0.19866933, -0.29552021, -0.38941834])
    const Float8 result =
        Sin(Float8(0.0f, 0.1f, 0.2f, 0.3f, -0.1f, -0.2f, -0.3f, -0.4f));
    EXPECT_NEAR(result.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.09983342f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.19866933f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.29552021f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), -0.09983342f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), -0.19866933f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), -0.29552021f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), -0.38941834f, 1e-6f);
  }

  // Test values in the range from -20*pi to 20*pi.
  {
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 20 * constants::pi_v<float>;
      const Float8 result = Sin(Float8(arg));
      ASSERT_NEAR(result.Extract<0>(), Sin(arg), 1e-6f) << "arg=" << arg;
    }
  }
}

TEST(Float8, Cos) {
  {
    // >>> import numpy
    // >>> numpy.cos([0.0, 0.1, 0.2, 0.3, -0.1, -0.2, -0.3, -0.4])
    // array([1.        , 0.99500417, 0.98006658, 0.95533649, 0.99500417,
    // 0.98006658, 0.95533649, 0.92106099])
    const Float8 result =
        Cos(Float8(0.0f, 0.1f, 0.2f, 0.3f, -0.1f, -0.2f, -0.3f, -0.4f));
    EXPECT_NEAR(result.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.99500417f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.98006658f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.95533649f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 0.99500417f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 0.98006658f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 0.95533649f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 0.92106099f, 1e-6f);
  }

  // Test values in the range from -20*pi to 20*pi.
  {
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 20 * constants::pi_v<float>;
      const Float8 result = Cos(Float8(arg));
      ASSERT_NEAR(result.Extract<0>(), Cos(arg), 1e-6f) << "arg=" << arg;
    }
  }
}

TEST(Float8, SinCos) {
  {
    Float8 sin, cos;
    SinCos(
        Float8(0.0f, 0.1f, 0.2f, 0.3f, -0.1f, -0.2f, -0.3f, -0.4f), sin, cos);

    // >>> import numpy
    // >>> numpy.sin([0.0, 0.1, 0.2, 0.3, -0.1, -0.2, -0.3, -0.4])
    // array([ 0.        ,  0.09983342,  0.19866933,  0.29552021, -0.09983342,
    //        -0.19866933, -0.29552021, -0.38941834])
    EXPECT_NEAR(sin.Extract<0>(), 0.0f, 1e-6f);
    EXPECT_NEAR(sin.Extract<1>(), 0.09983342f, 1e-6f);
    EXPECT_NEAR(sin.Extract<2>(), 0.19866933f, 1e-6f);
    EXPECT_NEAR(sin.Extract<3>(), 0.29552021f, 1e-6f);
    EXPECT_NEAR(sin.Extract<4>(), -0.09983342f, 1e-6f);
    EXPECT_NEAR(sin.Extract<5>(), -0.19866933f, 1e-6f);
    EXPECT_NEAR(sin.Extract<6>(), -0.29552021f, 1e-6f);
    EXPECT_NEAR(sin.Extract<7>(), -0.38941834f, 1e-6f);

    // >>> import numpy
    // >>> numpy.cos([0.0, 0.1, 0.2, 0.3, -0.1, -0.2, -0.3, -0.4])
    // array([1.        , 0.99500417, 0.98006658, 0.95533649, 0.99500417,
    // 0.98006658, 0.95533649, 0.92106099])
    EXPECT_NEAR(cos.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(cos.Extract<1>(), 0.99500417f, 1e-6f);
    EXPECT_NEAR(cos.Extract<2>(), 0.98006658f, 1e-6f);
    EXPECT_NEAR(cos.Extract<3>(), 0.95533649f, 1e-6f);
    EXPECT_NEAR(cos.Extract<4>(), 0.99500417f, 1e-6f);
    EXPECT_NEAR(cos.Extract<5>(), 0.98006658f, 1e-6f);
    EXPECT_NEAR(cos.Extract<6>(), 0.95533649f, 1e-6f);
    EXPECT_NEAR(cos.Extract<7>(), 0.92106099f, 1e-6f);
  }
}

TEST(Float8, Exp) {
  {
    // >>> import numpy
    // >>> numpy.exp([0.0, 0.1, -0.1, -0.2, 0.3, -0.3, 0.4, -0.4])
    // array([1.        , 1.10517092, 0.90483742, 0.81873075, 1.34985881,
    // 0.74081822, 1.4918247 , 0.67032005])
    const Float8 result =
        Exp(Float8(0.0f, 0.1f, -0.1f, -0.2f, 0.3f, -0.3f, 0.4f, -0.4f));
    EXPECT_NEAR(result.Extract<0>(), 1.0f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 1.10517092f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.90483742f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.81873075f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 1.34985881f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 0.74081822f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 1.4918247f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 0.67032005f, 1e-6f);
  }

  // Test values in the range from -5 to 5.
  {
    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 5.0f;
      const float actual = Exp(Float8(arg)).Extract<0>();
      const float expected = Exp(arg);
      ASSERT_NEAR(actual, expected, 2e-5f) << "arg=" << arg;
      ASSERT_LE(Abs((actual - expected) / expected), 1e-6f) << "arg=" << arg;
    }
  }

  // Test values in the range from -10 to 10.
  {
    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) {
      const float fac = (float(i) / (N - 1) - 0.5f) * 2;
      const float arg = fac * 10.0f;
      const float actual = Exp(Float8(arg)).Extract<0>();
      const float expected = Exp(arg);
      // GCC and CLang have different prevision of their built-in exp function,
      // which makes it hard to agree on a single eps. Hence rather a large
      // value.
      ASSERT_NEAR(actual, expected, 1e-2f) << "arg=" << arg;
      ASSERT_LE(Abs((actual - expected) / expected), 1e-6f) << "arg=" << arg;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

TEST(Float8, Norm) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5, 6, 7, 8, 9])
  EXPECT_NEAR(
      linalg::Norm(Float8(2, 3, 4, 5, 6, 7, 8, 9)), 16.852299546352718f, 1e-6f);
}

TEST(Float8, Length) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5, 6, 7, 8, 9])
  EXPECT_NEAR(linalg::Length(Float8(2, 3, 4, 5, 6, 7, 8, 9)),
              16.852299546352718f,
              1e-6f);
}

TEST(Float8, SquaredLength) {
  // >>> import numpy
  // >>> numpy.linalg.norm([2, 3, 4, 5, 6, 7, 8, 9]) ** 2
  EXPECT_NEAR(
      linalg::SquaredLength(Float8(2, 3, 4, 5, 6, 7, 8, 9)), 284.0f, 1e-6f);
}

TEST(Float8, Normalize) {
  {
    const Float8 result = linalg::Normalize(Float8(0));
    EXPECT_NEAR(result.Extract<0>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 0, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 0, 1e-6f);
  }

  {
    // >>> import numpy
    // >>> a = numpy.array([2, 3, 4, 5, 6, 7, 8, 9])
    // >>> a / numpy.linalg.norm(a)
    const Float8 result = linalg::Normalize(Float8(2, 3, 4, 5, 6, 7, 8, 9));
    EXPECT_NEAR(result.Extract<0>(), 0.11867817f, 1e-6f);
    EXPECT_NEAR(result.Extract<1>(), 0.17801725f, 1e-6f);
    EXPECT_NEAR(result.Extract<2>(), 0.23735633f, 1e-6f);
    EXPECT_NEAR(result.Extract<3>(), 0.29669541f, 1e-6f);
    EXPECT_NEAR(result.Extract<4>(), 0.3560345f, 1e-6f);
    EXPECT_NEAR(result.Extract<5>(), 0.41537358f, 1e-6f);
    EXPECT_NEAR(result.Extract<6>(), 0.47471266f, 1e-6f);
    EXPECT_NEAR(result.Extract<7>(), 0.53405175f, 1e-6f);
  }
}

}  // namespace radio_core
