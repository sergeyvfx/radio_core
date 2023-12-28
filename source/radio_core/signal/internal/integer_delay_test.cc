// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/integer_delay.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(IntegerDelay, Basic) {
  IntegerDelay<float> delay(4);

  EXPECT_EQ(delay(1.0f), 0.0f);
  EXPECT_EQ(delay(2.0f), 0.0f);
  EXPECT_EQ(delay(3.0f), 0.0f);
  EXPECT_EQ(delay(4.0f), 0.0f);

  EXPECT_EQ(delay(5.0f), 1.0f);
  EXPECT_EQ(delay(6.0f), 2.0f);
  EXPECT_EQ(delay(7.0f), 3.0f);
  EXPECT_EQ(delay(8.0f), 4.0f);
}

}  // namespace radio_core::signal
