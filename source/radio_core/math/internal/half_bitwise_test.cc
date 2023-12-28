// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/internal/half_bitwise.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

#if RADIO_CORE_HAVE_HALF

TEST(Half, Select) {
  EXPECT_EQ(Select(0, Half(0.0f), Half(0.0f)), Half(0.0f));
  EXPECT_EQ(Select(0xffff, Half(12.3f), Half(0.0f)), Half(12.3f));
  EXPECT_EQ(Select(0x7fff, Half(12.3f), Half(-1.0f)), Half(-12.3f));
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
