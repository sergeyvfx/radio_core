// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/simple_fir_filter.h"

#include <array>

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(SimpleFIRFilter, ImpulseResponse) {
  SimpleFIRFilter<float, float> filter;

  filter.SetKernel({{0.1f, 0.2f, 0.3f, 0.25f, 0.15f}});

  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(filter(1.0f), 0.1f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.2f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.3f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.25f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.15f, 1e-6f);
  EXPECT_NEAR(filter(0.0f), 0.0f, 1e-6f);
}

}  // namespace radio_core::signal
