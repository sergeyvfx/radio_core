// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/binary/nrzs/encoder.h"

#include "radio_core/unittest/test.h"

namespace radio_core::protocol::binary::nrzs {

TEST(Encoder, Basic) {
  Encoder encoder;

  EXPECT_FALSE(encoder(true));
  EXPECT_FALSE(encoder(true));

  EXPECT_TRUE(encoder(false));
  EXPECT_FALSE(encoder(false));
  EXPECT_TRUE(encoder(false));

  EXPECT_TRUE(encoder(true));
}

}  // namespace radio_core::protocol::binary::nrzs
