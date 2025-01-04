// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/dc_blocker.h"

#include <vector>

#include "radio_core/math/complex.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::Each;
using testing::FloatNear;

TEST(DCBlocker, Basic) {
  DCBlocker<float> dc_blocker(0.8f);

  // Push a lot of samples to stabilize the blocker.
  for (int a = 0; a < 100; ++a) {
    dc_blocker(1.21f);
  }

  // Ensure constant offset is removed.
  for (int a = 0; a < 10; ++a) {
    EXPECT_NEAR(dc_blocker(1.21f), 0.0f, 1e-6f);
  }
}

TEST(DCBlocker, BasicMultipleSampels) {
  const std::vector<float> input_samples(128, 1.21f);
  std::vector<float> output_samples(128);

  DCBlocker<float> dc_blocker(0.8f);
  dc_blocker(input_samples, output_samples);
  dc_blocker(input_samples, output_samples);

  EXPECT_THAT(output_samples, Each(FloatNear(0.0f, 1e-6f)));
}

TEST(DCBlocker, Complex) {
  // Simple test of complex input samples, currently only to ensure the code
  // compiles and does not have obvious mistakes.
  // TODO(sergey): Make it more comprehensive.

  DCBlocker<Complex, float> dc_blocker(0.8f);

  // Push a lot of samples to stabilize the blocker.
  for (int a = 0; a < 100; ++a) {
    dc_blocker(Complex(1.21f));
  }

  // Ensure constant offset is removed.
  for (int a = 0; a < 10; ++a) {
    EXPECT_NEAR(Norm(dc_blocker(Complex(1.21f))), 0.0f, 1e-6f);
  }
}

}  // namespace radio_core::signal
