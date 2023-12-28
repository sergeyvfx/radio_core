// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/frequency_duration.h"

#include <cstdint>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(FrequencyDuration, Construct) {
  using FrequencyDuration = FrequencyDuration<float>;

  {
    const FrequencyDuration frequency_duration(1200, 100);
    EXPECT_EQ(frequency_duration.frequency, 1200);
    EXPECT_EQ(frequency_duration.duration_ms, 100);
  }
}

TEST(FrequencyDuration, Compare) {
  using FrequencyDuration = FrequencyDuration<float>;

  EXPECT_TRUE(FrequencyDuration(10, 20) == FrequencyDuration(10, 20));
  EXPECT_FALSE(FrequencyDuration(10, 20) != FrequencyDuration(10, 20));

  EXPECT_FALSE(FrequencyDuration(10, 20) == FrequencyDuration(30, 40));
  EXPECT_TRUE(FrequencyDuration(10, 20) != FrequencyDuration(30, 40));
}

}  // namespace radio_core
