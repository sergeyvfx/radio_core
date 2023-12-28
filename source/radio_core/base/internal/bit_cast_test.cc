// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/bit_cast.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(bitwise, BitCast) {
  // float to uint32_t.
  EXPECT_EQ(BitCast<uint32_t>(float(0)), 0);
  EXPECT_EQ(BitCast<uint32_t>(float(FLT_MAX)), 2139095039);
  EXPECT_EQ(BitCast<uint32_t>(float(-FLT_MAX)), 4286578687);

  // uint32_t to float.
  EXPECT_EQ(BitCast<float>(uint32_t(0)), 0);
  EXPECT_EQ(BitCast<float>(uint32_t(2139095039)), FLT_MAX);
  EXPECT_EQ(BitCast<float>(uint32_t(4286578687)), -FLT_MAX);
}

}  // namespace radio_core
