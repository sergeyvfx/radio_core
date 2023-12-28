// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/picture/sstv/mode_spec.h"

#include "radio_core/unittest/test.h"

namespace radio_core::picture::sstv {

// Basic test to help working on the ModeSpec during development.
// Need some simple implementation file to compile the headers.

TEST(sstv, mode_spec) {
  const ModeSpec<float> mode_spec = ModeSpec<float>::Get(Mode::kPD120);
  EXPECT_EQ(mode_spec.mode, Mode::kPD120);
  EXPECT_FLOAT_EQ(mode_spec.black_frequency, 1500.0f);
}

}  // namespace radio_core::picture::sstv
