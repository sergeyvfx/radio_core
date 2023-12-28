// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/bandwidth.h"

#include "radio_core/unittest/test.h"

namespace radio_core::modulation::analog {

TEST(modulation, GetBandwidthIntervalAroundCenterFrequency) {
  EXPECT_EQ(GetBandwidthIntervalAroundCenterFrequency<float>(Type::kAM, 1000),
            Interval<float>(-500, 500));

  EXPECT_EQ(GetBandwidthIntervalAroundCenterFrequency<float>(Type::kUSB, 1000),
            Interval<float>(0, 1000));

  EXPECT_EQ(GetBandwidthIntervalAroundCenterFrequency<float>(Type::kLSB, 1000),
            Interval<float>(-1000, 0));
}

}  // namespace radio_core::modulation::analog
