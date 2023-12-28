// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/filter_window_heuristic.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(filter_window_heuristic, EstimateNormalizedTransitionBandwidth) {
  // Kernel for 24kHz transition bandwidth at 6Msps.
  EXPECT_NEAR(
      EstimateNormalizedTransitionBandwidth<float>(159), 0.0251572f, 1e-6f);
}

TEST(filter_window_heuristic,
     EstimateFilterSizeForNormalizedTransitionBandwidth) {
  // Kernel for 24kHz transition bandwidth at 6Msps.
  EXPECT_NEAR(EstimateFilterSizeForNormalizedTransitionBandwidth(0.0251572f),
              159,
              1e-6f);
}

TEST(filter_window_heuristic, CalculateKaiserBeta) {
  EXPECT_NEAR(CalculateKaiserBeta(75.0f), 7.3062606f, 1e-6f);
  EXPECT_NEAR(CalculateKaiserBeta(32.0f), 2.3919291f, 1e-6f);
  EXPECT_NEAR(CalculateKaiserBeta(16.0f), 0.0f, 1e-6f);
}

TEST(filter_window_heuristic, CalculateKaiserSize) {
  EXPECT_EQ(CalculateKaiserSize(32.6f, 2.1f), 6);
}

}  // namespace radio_core::signal
