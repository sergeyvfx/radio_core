// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/digital/fsk/tones.h"

#include "radio_core/unittest/test.h"

namespace radio_core::modulation::digital::fsk {

TEST(Tones, Compare) {
  EXPECT_EQ(Tones(1200, 2200), Tones(1200, 2200));
  EXPECT_NE(Tones(1200, 2200), Tones(2200, 1200));
}

}  // namespace radio_core::modulation::digital::fsk
