// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/picture/sstv/luma.h"

#include "radio_core/unittest/test.h"

namespace radio_core::picture::sstv {

TEST(sstv, FrequencyToLuma) {
  const ModeSpec<float> mode_spec = ModeSpec<float>::Get(Mode::kPD120);
  EXPECT_NEAR(FrequencyToLuma(mode_spec, 1500.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(FrequencyToLuma(mode_spec, 1900.0f), 0.5f, 1e-6f);
  EXPECT_NEAR(FrequencyToLuma(mode_spec, 2300.0f), 1.0f, 1e-6f);
}

TEST(sstv, LumaToFrequency) {
  const ModeSpec<float> mode_spec = ModeSpec<float>::Get(Mode::kPD120);
  EXPECT_NEAR(LumaToFrequency(mode_spec, 0.0f), 1500.0f, 1e-6f);
  EXPECT_NEAR(LumaToFrequency(mode_spec, 0.5f), 1900.0f, 1e-6f);
  EXPECT_NEAR(LumaToFrequency(mode_spec, 1.0f), 2300.0f, 1e-6f);
}

}  // namespace radio_core::picture::sstv
