// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/info.h"

#include "radio_core/unittest/test.h"

namespace radio_core::modulation::analog {

TEST(modulation, TypeFromName) {
  Type type;

  EXPECT_TRUE(TypeFromName("AM", type));
  EXPECT_EQ(type, Type::kAM);

  EXPECT_TRUE(TypeFromName("NFM", type));
  EXPECT_EQ(type, Type::kNFM);

  EXPECT_TRUE(TypeFromName("WFM", type));
  EXPECT_EQ(type, Type::kWFM);

  EXPECT_TRUE(TypeFromName("LSB", type));
  EXPECT_EQ(type, Type::kLSB);
  EXPECT_TRUE(TypeFromName("USB", type));
  EXPECT_EQ(type, Type::kUSB);
}

}  // namespace radio_core::modulation::analog
