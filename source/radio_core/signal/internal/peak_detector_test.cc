// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/peak_detector.h"

#include <array>
#include <span>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(LerpPeakDetector, Single) {
  LerpPeakDetector<float> peak_detector(0.8f, 0.2f);

  EXPECT_NEAR(peak_detector(1), 0.8f, 1e-6f);
  EXPECT_NEAR(peak_detector(2), 1.76f, 1e-6f);
  EXPECT_NEAR(peak_detector(3), 2.752f, 1e-6f);
  EXPECT_NEAR(peak_detector(-4), 1.4016f, 1e-6f);
  EXPECT_NEAR(peak_detector(-5), 0.12128f, 1e-6f);
}

TEST(LerpPeakDetector, Buffer) {
  auto input_samples = std::to_array<float>({1, 2, 3, -4, -5});
  std::array<float, 5> output_samples;

  LerpPeakDetector<float> peak_detector(0.8f, 0.2f);
  peak_detector(input_samples, output_samples);

  EXPECT_THAT(output_samples,
              Pointwise(FloatNear(1e-6f),
                        std::to_array<float>(
                            {0.8f, 1.76f, 2.752f, 1.4016f, 0.12128f})));
}

}  // namespace radio_core::signal
