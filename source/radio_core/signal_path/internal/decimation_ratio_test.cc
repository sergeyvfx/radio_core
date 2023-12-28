// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal_path/internal/decimation_ratio.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal_path::internal {

TEST(CalculateStagesDecimationRadio, AirSpyTo48kHz) {
  // Typical AM configuration.
  EXPECT_EQ(StagesDecimation<float>::Calculate(6000000, 48000, 5000),
            StagesDecimation<float>::Ratio(125, 1));

  // Typical NFM configuration.
  EXPECT_EQ(StagesDecimation<float>::Calculate(6000000, 48000, 12500),
            StagesDecimation<float>::Ratio(125, 1));

  // Typical WFM configuration.
  EXPECT_EQ(StagesDecimation<float>::Calculate(6000000, 48000, 150000),
            StagesDecimation<float>::Ratio(25, 5));

  // Extreme WFM configuration.
  //
  // NOTE: Clamp to 240 kHz instead of some other radio's 250 kHz which
  // better corresponds the desired decimation ratio for the best performance.
  EXPECT_EQ(StagesDecimation<float>::Calculate(6000000, 48000, 240000),
            StagesDecimation<float>::Ratio(25, 5));
}

}  // namespace radio_core::signal_path::internal
