// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/filter.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(ConvertFilterSize, Basic) {
  EXPECT_EQ(ConvertFilterSize<float>(11025, 31, 11025), 31);

  EXPECT_EQ(ConvertFilterSize<float>(11025, 31, 44100), 121);

  EXPECT_EQ(ConvertFilterSize<float>(44100, 121, 11025), 31);
}

}  // namespace radio_core::signal
