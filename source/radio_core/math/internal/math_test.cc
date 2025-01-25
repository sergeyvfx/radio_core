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
  // Test data is based on https://stackoverflow.com/a/67098028
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

  // Single precision floating point.
  // Tests float overload.
  {
    for (const double a_double : kArguments) {
      const float a = a_double;
      EXPECT_NEAR(Modulo(a, 3.2f), std::fmod(a, 3.2f), 1e-6f);
      EXPECT_NEAR(Modulo(a, -3.2f), std::fmod(a, -3.2f), 1e-6f);
    }
  }

  // Double precision floating point.
  // Tests double overload.
  {
    for (const double a : kArguments) {
      EXPECT_NEAR(Modulo(a, 3.2), std::fmod(a, 3.2), 1e-12);
      EXPECT_NEAR(Modulo(a, -3.2), std::fmod(a, -3.2), 1e-12);
    }
  }

  // Double precision floating point.
  // Tests generic templated implementation.
  {
    for (const double a : kArguments) {
      EXPECT_NEAR(Modulo<double>(a, 3.2), std::fmod(a, 3.2), 1e-12);
      EXPECT_NEAR(Modulo<double>(a, -3.2), std::fmod(a, -3.2), 1e-12);
    }
  }

  // Manual test to ensure behavior w.r.t argument signs is what it is expected
  // to be.
  //
  // >>> import numpy as np
  // >>> np.fmod([4.2, -4.2, 4.2, -4.2], [3.1, 3.1, -3.1, -3.1])
  // array([ 1.1, -1.1,  1.1, -1.1])
  //
  // It is also verified against Matlab's rem() function:
  // >> [rem(4.2, 3.1), rem(-4.2, 3.1), rem(4.2, -3.1), rem(-4.2,-3.1)]
  // ans =
  //   1.1000   -1.1000    1.1000   -1.1000
  {
    EXPECT_NEAR(Modulo(4.2f, 3.1f), 1.1f, 1e-6f);
    EXPECT_NEAR(Modulo(-4.2f, 3.1f), -1.1f, 1e-6f);
    EXPECT_NEAR(Modulo(4.2f, -3.1f), 1.1f, 1e-6f);
    EXPECT_NEAR(Modulo(-4.2f, -3.1f), -1.1f, 1e-6f);

    EXPECT_NEAR(Modulo<float>(4.2f, 3.1f), 1.1f, 1e-6f);
    EXPECT_NEAR(Modulo<float>(-4.2f, 3.1f), -1.1f, 1e-6f);
    EXPECT_NEAR(Modulo<float>(4.2f, -3.1f), 1.1f, 1e-6f);
    EXPECT_NEAR(Modulo<float>(-4.2f, -3.1f), -1.1f, 1e-6f);
  }
}

TEST(math, FloorModulo) {
  // Similar code implemented in Python for the cross-reference and ease of
  // getting results for multiple inputs.
  //
  // >>> import numpy as np
  // >>> def floored_mod(a, b):
  // ...     a = np.array(a)
  // ...     b = np.array(b)
  // ...     return a - np.floor(a / b) * b
  // >>> floored_mod([4.2, -4.2, 4.2, -4.2], [3.1, 3.1, -3.1, -3.1])
  // array([ 1.1,  2. , -2. , -1.1])
  //
  // It is also verified against Matlab's mod() function:
  // >> [mod(4.2, 3.1), mod(-4.2, 3.1), mod(4.2, -3.1), mod(-4.2,-3.1)]
  // ans =
  //   1.1000    2.0000   -2.0000   -1.1000
  EXPECT_NEAR(FloorModulo(4.2f, 3.1f), 1.1f, 1e-6f);
  EXPECT_NEAR(FloorModulo(-4.2f, 3.1f), 2.0f, 1e-6f);
  EXPECT_NEAR(FloorModulo(4.2f, -3.1f), -2.0f, 1e-6f);
  EXPECT_NEAR(FloorModulo(-4.2f, -3.1f), -1.1f, 1e-6f);
}

TEST(math, Fract) {
  // Single precision floating point.
  // Lower epsilon value due ot Fract(34.12f) not giving result within typical
  // 1e-6f epsilon.
  {
    EXPECT_NEAR(Fract(0.0f), 0.0f, 1e-5f);

    EXPECT_NEAR(Fract(1.0f), 0.0f, 1e-5f);
    EXPECT_NEAR(Fract(0.12f), 0.12f, 1e-5f);
    EXPECT_NEAR(Fract(34.12f), 0.12f, 1e-5f);

    EXPECT_NEAR(Fract(-1.0f), 0.0f, 1e-5f);
    EXPECT_NEAR(Fract(-0.12f), -0.12f, 1e-5f);
    EXPECT_NEAR(Fract(-34.12f), -0.12f, 1e-5f);
  }

  // Double precision floating point.
  {
    EXPECT_NEAR(Fract(0.0), 0.0, 1e-12);

    EXPECT_NEAR(Fract(1.0), 0.0, 1e-12);
    EXPECT_NEAR(Fract(0.12), 0.12, 1e-12);
    EXPECT_NEAR(Fract(34.12), 0.12, 1e-12);

    EXPECT_NEAR(Fract(-1.0), 0.0, 1e-12);
    EXPECT_NEAR(Fract(-0.12), -0.12, 1e-12);
    EXPECT_NEAR(Fract(-34.12), -0.12, 1e-12);
  }
}

TEST(math, FloorFract) {
  // Single precision floating point.
  // Lower epsilon value due ot Fract(34.12f) not giving result within typical
  // 1e-6f epsilon.
  {
    EXPECT_NEAR(FloorFract(0.0f), 0.0f, 1e-5f);

    EXPECT_NEAR(FloorFract(1.0f), 0.0f, 1e-5f);
    EXPECT_NEAR(FloorFract(0.12f), 0.12f, 1e-5f);
    EXPECT_NEAR(FloorFract(34.12f), 0.12f, 1e-5f);

    EXPECT_NEAR(FloorFract(-1.0f), 0.0f, 1e-5f);
    EXPECT_NEAR(FloorFract(-0.12f), 0.88f, 1e-5f);
    EXPECT_NEAR(FloorFract(-34.12f), 0.88f, 1e-5f);
  }

  // Double precision floating point.
  {
    EXPECT_NEAR(FloorFract(0.0), 0.0, 1e-12);

    EXPECT_NEAR(FloorFract(1.0), 0.0, 1e-12);
    EXPECT_NEAR(FloorFract(0.12), 0.12, 1e-12);
    EXPECT_NEAR(FloorFract(34.12), 0.12, 1e-12);

    EXPECT_NEAR(FloorFract(-1.0), 0.0, 1e-12);
    EXPECT_NEAR(FloorFract(-0.12), 0.88, 1e-12);
    EXPECT_NEAR(FloorFract(-34.12), 0.88, 1e-12);
  }
}

// A simple snippet to benchmark possible different implementations of the
// Fract() function.
TEST(math, FractBenchmark) {
  GTEST_SKIP() << "Benchmark is disabled";

  float value = 12.345f;
  for (int j = 0; j < 16 * 1024; ++j) {
    for (int i = 0; i < 1024; ++i) {
      const float f = Fract(value);
      // Modify the variable to prevent constant unrolling.
      value += f * 0.01f;
    }
  }
  // Perform comparison on the result value to avoid the whole loop from above
  // from being optimized out.
  EXPECT_TRUE(IsFinite(value));
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
