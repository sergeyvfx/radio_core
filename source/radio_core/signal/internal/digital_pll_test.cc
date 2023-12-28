// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/digital_pll.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(signal, DigitalPLL) {
  DigitalPLL<float>::Options options;
  options.data_baud = 1200;
  options.sample_rate = 11025;
  options.inertia = 0.01f;

  DigitalPLL<float> pll(options);

  // First bit.
  {
    EXPECT_FALSE(pll(0));
    EXPECT_FALSE(pll(0));
    EXPECT_FALSE(pll(0));
    EXPECT_FALSE(pll(0));

    EXPECT_TRUE(pll(0));

    EXPECT_FALSE(pll(0));
    EXPECT_FALSE(pll(0));
    EXPECT_FALSE(pll(0));
    EXPECT_FALSE(pll(0));
  }

  // Inject some extra samples to test the clock recovery capabilities.
  EXPECT_FALSE(pll(0));
  EXPECT_FALSE(pll(0));

  // Second bit.
  {
    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));

    EXPECT_TRUE(pll(1));

    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));
    EXPECT_FALSE(pll(1));
  }
}

}  // namespace radio_core::signal
