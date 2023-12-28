// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/quantize.h"

#include <cstdint>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(quantize, PositiveUnitFloatToUnsigned) {
  EXPECT_EQ(PositiveUnitFloatToUnsigned<uint16_t>(-10.0f), 0);
  EXPECT_EQ(PositiveUnitFloatToUnsigned<uint16_t>(0.0f), 0);

  EXPECT_EQ(PositiveUnitFloatToUnsigned<uint16_t>(0.2f), 13107);
  EXPECT_EQ(PositiveUnitFloatToUnsigned<uint16_t>(0.8f), 52428);

  EXPECT_EQ(PositiveUnitFloatToUnsigned<uint16_t>(10.0f), 65535);
}

}  // namespace radio_core
