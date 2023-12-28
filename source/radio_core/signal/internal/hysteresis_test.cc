// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/hysteresis.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(Hysteresis, AroundZero) {
  Hysteresis<float> hysteresis(0.0f, 0.2f);

  EXPECT_EQ(hysteresis(0.0f), 0.0f);
  EXPECT_EQ(hysteresis(-0.02f), 0.0f);
  EXPECT_EQ(hysteresis(0.02f), 0.0f);

  EXPECT_EQ(hysteresis(0.2f), 0.2f);
  EXPECT_EQ(hysteresis(0.8f), 0.8f);
  EXPECT_EQ(hysteresis(0.01f), 0.8f);
  EXPECT_EQ(hysteresis(-0.01f), 0.8f);

  EXPECT_EQ(hysteresis(-0.2f), -0.2f);
  EXPECT_EQ(hysteresis(-0.8f), -0.8f);
  EXPECT_EQ(hysteresis(-0.01f), -0.8f);
  EXPECT_EQ(hysteresis(0.01f), -0.8f);
}

}  // namespace radio_core::signal
