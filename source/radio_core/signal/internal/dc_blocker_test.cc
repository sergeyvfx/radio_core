// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/dc_blocker.h"

#include "radio_core/math/complex.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(DCBlocker, Basic) {
  DCBlocker<float> dc_blocker(0.8f);

  int phi = 0;

  // Push a lot of samples to stabilize the blocker.
  for (int a = 0; a < 100; ++a, ++phi) {
    dc_blocker(1.21f);
  }

  // Ensure constant offset is removed.
  for (int a = 0; a < 10; ++a, ++phi) {
    EXPECT_NEAR(dc_blocker(1.21f), 0.0f, 1e-6f);
  }
}

TEST(DCBlocker, Complex) {
  // Simple test of complex input samples, currently only to ensure the code
  // compiles and does not have obvious mistakes.
  // TODO(sergey): Make it more comprehensive.

  DCBlocker<Complex, float> dc_blocker(0.8f);

  int phi = 0;

  // Push a lot of samples to stabilize the blocker.
  for (int a = 0; a < 100; ++a, ++phi) {
    dc_blocker(Complex(1.21f));
  }

  // Ensure constant offset is removed.
  for (int a = 0; a < 10; ++a, ++phi) {
    EXPECT_NEAR(Norm(dc_blocker(Complex(1.21f))), 0.0f, 1e-6f);
  }
}

}  // namespace radio_core::signal
