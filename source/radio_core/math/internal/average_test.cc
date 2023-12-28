// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/average.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(math_average, MovingAverage) {
  {
    MovingAverage<float, 4> average;
    EXPECT_EQ(average.Get(), 0.0f);
  }

  {
    MovingAverage<float, 4> average;

    EXPECT_NEAR(average(1.0f), 1.0f, 1e-6f);
    EXPECT_NEAR(average(2.0f), 1.5f, 1e-6f);
    EXPECT_NEAR(average(3.0f), 2.0f, 1e-6f);
    EXPECT_NEAR(average(4.0f), 2.5f, 1e-6f);
    EXPECT_NEAR(average(5.0f), 3.5f, 1e-6f);
    EXPECT_NEAR(average(6.0f), 4.5f, 1e-6f);
  }

  {
    constexpr float kValue = 0.13f;
    MovingAverage<float, 4> average;

    for (int i = 0; i < 10000; ++i) {
      average.PushSample(kValue);
    }

    EXPECT_NEAR(average.Get(), kValue, 1e-6f);
  }

  {
    MovingAverage<float, 4> average;
    average.PushSample(1.0f);

    const MovingAverage<float, 4> const_average = average;
    EXPECT_NEAR(const_average.Get(), 1.0f, 1e-6f);
  }
}

TEST(math_average, ExponentialMovingAverage) {
  float average = 0.0f;

  average = ExponentialMovingAverage(1.0f, 0.1f, average);
  EXPECT_NEAR(average, 0.1f, 1e-6f);

  average = ExponentialMovingAverage(2.0f, 0.1f, average);
  EXPECT_NEAR(average, 0.29f, 1e-6f);

  average = ExponentialMovingAverage(3.0f, 0.2f, average);
  EXPECT_NEAR(average, 0.832f, 1e-6f);
}

}  // namespace radio_core
