// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/math.h"

#include <array>
#include <cstdint>
#include <limits>

#include "radio_core/base/algorithm.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

// Functionally this is constant zero.
//
// Defined as volatile to avoid compilers being too smart and generating errors
// about division by constant zero.
volatile int kZero = 0;

TEST(math, Modulo) {
  // Test case os based on https://stackoverflow.com/a/67098028
  const auto kArguments = std::to_array<double>({-10.9,
                                                 -10.5,
                                                 -10.4,
                                                 -0.9,
                                                 -0.5,
                                                 -0.1,
                                                 0.0,
                                                 0.1,
                                                 0.5,
                                                 0.9,
                                                 10.4,
                                                 10.5,
                                                 10.9});

  for (const double a : kArguments) {
    EXPECT_NEAR(Modulo<double>(a, 3.2), std::fmod(a, 3.2), 1e-12);
    EXPECT_NEAR(Modulo<double>(a, -3.2), std::fmod(a, -3.2), 1e-12);
  }
}

TEST(math, IsFinite) {
  // Single precision.
  {
    EXPECT_TRUE(IsFinite(0.0f));
    EXPECT_TRUE(IsFinite(1.0f));
    EXPECT_TRUE(IsFinite(-1.0f));

    EXPECT_TRUE(IsFinite(FLT_MIN));
    EXPECT_TRUE(IsFinite(-FLT_MIN));
    EXPECT_TRUE(IsFinite(FLT_MAX));
    EXPECT_TRUE(IsFinite(-FLT_MAX));

    EXPECT_FALSE(IsFinite(NAN));
    EXPECT_FALSE(IsFinite(INFINITY));
  }
}

TEST(math, IsNaN) {
  // Single precision.
  {
    using limits = std::numeric_limits<float>;

    EXPECT_TRUE(IsNaN(limits::quiet_NaN()));
    EXPECT_FALSE(IsNaN(limits::infinity()));
    EXPECT_FALSE(IsNaN(0.0f));
    EXPECT_FALSE(IsNaN(limits::min() / 2.0f));
    EXPECT_TRUE(IsNaN(0.0f / float(kZero)));
    EXPECT_TRUE(IsNaN(limits::infinity() - limits::infinity()));
  }

  // Double precision.
  {
    using limits = std::numeric_limits<double>;

    EXPECT_TRUE(IsNaN(limits::quiet_NaN()));
    EXPECT_FALSE(IsNaN(limits::infinity()));
    EXPECT_FALSE(IsNaN(0.0));
    EXPECT_FALSE(IsNaN(limits::min() / 2.0));
    EXPECT_TRUE(IsNaN(0.0 / double(kZero)));
    EXPECT_TRUE(IsNaN(limits::infinity() - limits::infinity()));
  }
}

TEST(math, Clamp) {
  EXPECT_EQ(Clamp(1.1f, 1.0f, 2.0f), 1.1f);
  EXPECT_EQ(Clamp(0.1f, 1.0f, 2.0f), 1.0f);
  EXPECT_EQ(Clamp(2.1f, 1.0f, 2.0f), 2.0f);
}

TEST(math, Saturate) {
  EXPECT_EQ(Saturate(0.1f), 0.1f);
  EXPECT_EQ(Saturate(-0.1f), 0.0f);
  EXPECT_EQ(Saturate(1.1f), 1.0f);
}

TEST(math, Sign) {
  EXPECT_EQ(Sign(1), 1);
  EXPECT_EQ(Sign(12), 1);

  EXPECT_EQ(Sign(-1), -1);
  EXPECT_EQ(Sign(-12), -1);

  EXPECT_EQ(Sign(0), 1);
  EXPECT_EQ(Sign(-0.0f), -1);
}

TEST(math, DegreesToRadians) {
  EXPECT_NEAR(DegreesToRadians(60.0), 1.0471975511965976, 1e-12);
}

TEST(math, RadiansToDegrees) {
  EXPECT_NEAR(RadiansToDegrees(1.0471975511965976), 60.0, 1e-12);
}

TEST(math, Sinc) {
  EXPECT_EQ(Sinc(0.0f), 1.0f);
  EXPECT_NEAR(Sinc(0.5f), 0.63662f, 1e-6f);
  EXPECT_NEAR(Sinc(-0.5f), 0.63662f, 1e-6f);
}

TEST(math, MultiplyAdd) {
  EXPECT_NEAR(MultiplyAdd(2, 7, 9), 65, 1e-6f);
  EXPECT_NEAR(MultiplyAdd(2, 9, 7), 65, 1e-6f);
}

TEST(math, Lerp) {
  EXPECT_NEAR(Lerp(3.0f, 5.0f, 0.0f), 3.0f, 1e-6f);
  EXPECT_NEAR(Lerp(3.0f, 5.0f, 0.1f), 3.2f, 1e-6f);
  EXPECT_NEAR(Lerp(3.0f, 5.0f, 0.9f), 4.8f, 1e-6f);
  EXPECT_NEAR(Lerp(3.0f, 5.0f, 1.0f), 5.0f, 1e-6f);
}

TEST(math, FastLog10) {
  EXPECT_NEAR(FastLog10(1.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(FastLog10(5.0f), 0.69897f, 1e-6f);
  EXPECT_NEAR(FastLog10(10.0f), 1.0f, 1e-6f);
}

TEST(math, FastArcTan2) {
  EXPECT_NEAR(FastArcTan2(0.0f, 1.0f), 0.0f, 0.005f);
  EXPECT_NEAR(FastArcTan2(1.0f, 0.0f), 1.57079637f, 0.005f);
  EXPECT_NEAR(FastArcTan2(2.0f, -3.0f), 2.55359006f, 0.005f);
  EXPECT_NEAR(FastArcTan2(-2.0f, 3.0f), -0.58800262f, 0.005f);
}

TEST(math, FastIntPow) {
  // Integer argument.
  {
    EXPECT_EQ(FastIntPow(0, 0), 1);
    EXPECT_EQ(FastIntPow(2, 0), 1);

    EXPECT_EQ(FastIntPow(2, 4), 16);
    EXPECT_EQ(FastIntPow(8, 10), 1073741824);
  }
}

TEST(math, RoundUpToMultiple) {
  {
    EXPECT_EQ(RoundUpToMultiple(0, 10), 0);
    EXPECT_EQ(RoundUpToMultiple(1, 10), 10);
    EXPECT_EQ(RoundUpToMultiple(8, 10), 10);
    EXPECT_EQ(RoundUpToMultiple(10, 10), 10);
    EXPECT_EQ(RoundUpToMultiple(21, 10), 30);
  }

  {
    EXPECT_EQ(RoundUpToMultiple<uint8_t>(21, 100), 100);
    EXPECT_EQ(RoundUpToMultiple<uint8_t>(121, 100), 200);
    EXPECT_EQ(RoundUpToMultiple<uint8_t>(243, 10), 250);
  }
}

TEST(math, RoundDownToMultiple) {
  EXPECT_EQ(RoundDownToMultiple(0, 10), 0);
  EXPECT_EQ(RoundDownToMultiple(9, 10), 0);
  EXPECT_EQ(RoundDownToMultiple(11, 10), 10);
  EXPECT_EQ(RoundDownToMultiple(19, 10), 10);
  EXPECT_EQ(RoundDownToMultiple(20, 10), 20);
  EXPECT_EQ(RoundDownToMultiple(21, 10), 20);
}

TEST(math, SinCos) {
  constexpr int kNumSteps = 3600;

  float max_error = 0;

  for (int i = 0; i < kNumSteps; ++i) {
    const float arg = 2 * constants::pi_v<float> / kNumSteps * float(i);

    float sine;
    float cosine;
    SinCos(arg, sine, cosine);

    max_error = Max(max_error, Abs(sine - Sin(arg)));
    max_error = Max(max_error, Abs(cosine - Cos(arg)));
  }

  EXPECT_LT(max_error, 1e-6);
}

}  // namespace radio_core
