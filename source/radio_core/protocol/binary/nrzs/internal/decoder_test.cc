// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/binary/nrzs/decoder.h"

#include "radio_core/unittest/test.h"

namespace radio_core::protocol::binary::nrzs {

TEST(Decoder, Basic) {
  Decoder decoder;

  EXPECT_TRUE(decoder(false));  // Constant.

  EXPECT_FALSE(decoder(true));  // Toggle.

  EXPECT_TRUE(decoder(true));  // Constant.
  EXPECT_TRUE(decoder(true));  // Constant.

  EXPECT_FALSE(decoder(false));  // Toggle.
  EXPECT_FALSE(decoder(true));   // Toggle.
  EXPECT_FALSE(decoder(false));  // Toggle.

  EXPECT_TRUE(decoder(false));  // Constant.
  EXPECT_TRUE(decoder(false));  // Constant.
}

}  // namespace radio_core::protocol::binary::nrzs
