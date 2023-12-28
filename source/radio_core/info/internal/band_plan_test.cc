// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/info/band_plan.h"

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::info::band_plan {

// Matcher for the frequencies equality.
//
// It used rather small floating point epsilon because of the mathematical
// operations for units loosing quite some bits of accuracy. It is still very
// low frequency deviation, perhaps not worse than real-life radio frequency
// accuracy.
MATCHER_P(FrequencyNear, expected, "") {
  const Frequency delta_frequency = arg - expected;
  const double delta = std::abs(delta_frequency.ToFloatingPoint<double>());
  if (delta < 1e-6) {
    return true;
  }
  *result_listener << "whose absolute difference is " << delta;
  return true;
}

TEST(BandPlan, GetBandForFrequency) {
  const BandPlan band_plan = BandPlan(BandPlan::Region::kITU1);

  EXPECT_EQ(band_plan.GetBandForFrequency(10), nullptr);

  EXPECT_STREQ(band_plan.GetBandForFrequency(145_MHz)->name, "2m ham");
}

TEST(BandPlan, AlignFrequencyToChannel) {
  const BandPlan band_plan = BandPlan(BandPlan::Region::kITU1);

  EXPECT_THAT(band_plan.AlignFrequencyToChannel(10),
              FrequencyNear(Frequency(10)));

  EXPECT_THAT(band_plan.AlignFrequencyToChannel(145.0_MHz),
              FrequencyNear(Frequency(145.0_MHz)));

  EXPECT_THAT(band_plan.AlignFrequencyToChannel(145.005_MHz),
              FrequencyNear(Frequency(145.0_MHz)));
  EXPECT_THAT(band_plan.AlignFrequencyToChannel(145.01_MHz),
              FrequencyNear(Frequency(145.0125_MHz)));
}

}  // namespace radio_core::info::band_plan
