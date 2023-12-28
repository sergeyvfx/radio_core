// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/byte_util.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::Eq;
using testing::Pointwise;

TEST(base_byte_utils, ToBytesArray) {
  constexpr auto kConstExprBytes = ToBytesArray({0x12, 0x36, 0x7e, 0xff});

  EXPECT_THAT(kConstExprBytes,
              Pointwise(Eq(),
                        std::to_array({std::byte{0x12},
                                       std::byte{0x36},
                                       std::byte{0x7e},
                                       std::byte{0xff}})));
}

}  // namespace radio_core
