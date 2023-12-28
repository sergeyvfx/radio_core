// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/digital_hysteresis.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(DigitalHysteresis, AroundZero) {
  DigitalHysteresis<float> hysteresis(0.0f, 0.2f);

  EXPECT_FALSE(hysteresis(0.0f));
  EXPECT_FALSE(hysteresis(-0.02f));
  EXPECT_FALSE(hysteresis(0.02f));

  EXPECT_TRUE(hysteresis(0.2f));
  EXPECT_TRUE(hysteresis(0.8f));
  EXPECT_TRUE(hysteresis(0.01f));
  EXPECT_TRUE(hysteresis(-0.01f));

  EXPECT_FALSE(hysteresis(-0.2f));
  EXPECT_FALSE(hysteresis(-0.8f));
  EXPECT_FALSE(hysteresis(-0.01f));
  EXPECT_FALSE(hysteresis(0.01f));
}

}  // namespace radio_core::signal
