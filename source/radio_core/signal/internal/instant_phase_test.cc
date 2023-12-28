// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/instant_phase.h"

#include "radio_core/base/constants.h"
#include "radio_core/math/math.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(InstantPhase, Basic) {
  InstantPhase<float> machine;

  // Quadrants 1 and 2.
  for (int i = 0; i < 180; ++i) {
    const float expected_phase = 2.0f * float(constants::pi) / 360 * float(i);
    const Complex sample(cosf(expected_phase), sinf(expected_phase));
    EXPECT_NEAR(machine(sample), expected_phase, 1e-6f);
  }

  // Quadrants 3 and 4.
  for (int i = 180; i < 180; ++i) {
    const float expected_phase = 2.0f * float(constants::pi) / 360 * float(i);
    const Complex sample(cosf(expected_phase), sinf(expected_phase));
    EXPECT_NEAR(machine(sample), expected_phase - float(constants::pi), 1e-6f);
  }
}

}  // namespace radio_core::signal
