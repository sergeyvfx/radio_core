// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/peak_detector.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(PerPointLerpPeakDetector, Float) {
  std::array<float, 5> samples = {{1, 2, 3, -4, -5}};
  std::array<float, 5> peak = {{0, 0, 0, 0, 0}};

  kernel::PerPointLerpPeakDetector<float>(samples, peak, 0.8f, 0.2f);

  EXPECT_NEAR(peak[0], 0.8f, 1e-6f);
  EXPECT_NEAR(peak[1], 1.6f, 1e-6f);
  EXPECT_NEAR(peak[2], 2.4f, 1e-6f);
  EXPECT_NEAR(peak[3], -0.8f, 1e-6f);
  EXPECT_NEAR(peak[4], -1.0f, 1e-6f);
}

TEST(PerPointLerpPeakDetector, Double) {
  std::array<double, 5> samples = {{1, 2, 3, -4, -5}};
  std::array<double, 5> peak = {{0, 0, 0, 0, 0}};

  kernel::PerPointLerpPeakDetector<double>(samples, peak, 0.8, 0.2);

  EXPECT_NEAR(peak[0], 0.8, 1e-6);
  EXPECT_NEAR(peak[1], 1.6, 1e-6);
  EXPECT_NEAR(peak[2], 2.4, 1e-6);
  EXPECT_NEAR(peak[3], -0.8, 1e-6);
  EXPECT_NEAR(peak[4], -1.0, 1e-6);
}

#if RADIO_CORE_HAVE_HALF

TEST(PerPointLerpPeakDetector, Half) {
  const auto samples = std::to_array<Half>({1, 2, 3, -4, -5, 1, 2, 3, -4, -5});
  std::array<Half, 10> peak = {{0, 0, 0, 0, 0}};

  kernel::PerPointLerpPeakDetector<Half>(samples, peak, Half(0.8f), Half(0.2f));

  EXPECT_NEAR(float(peak[0]), 0.8f, 2e-3f);
  EXPECT_NEAR(float(peak[1]), 1.6f, 2e-3f);
  EXPECT_NEAR(float(peak[2]), 2.4f, 2e-3f);
  EXPECT_NEAR(float(peak[3]), -0.8f, 2e-3f);
  EXPECT_NEAR(float(peak[4]), -1.0f, 2e-3f);
  EXPECT_NEAR(float(peak[5]), 0.8f, 2e-3f);
  EXPECT_NEAR(float(peak[6]), 1.6f, 2e-3f);
  EXPECT_NEAR(float(peak[7]), 2.4f, 2e-3f);
  EXPECT_NEAR(float(peak[8]), -0.8f, 2e-3f);
  EXPECT_NEAR(float(peak[9]), -1.0f, 2e-3f);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
