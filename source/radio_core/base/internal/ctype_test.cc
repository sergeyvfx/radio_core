// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/ctype.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(base, IsDigit) {
  for (const char ch : "0123456789") {
    if (ch == '\0') {
      break;
    }
    EXPECT_TRUE(IsDigit(ch)) << "Failure for " << ch;
  }

  for (const char ch : "abcdefABCDEF") {
    if (ch == '\0') {
      break;
    }
    EXPECT_FALSE(IsDigit(ch)) << "Failure for " << ch;
  }

  EXPECT_FALSE(IsDigit('\0'));
  EXPECT_FALSE(IsDigit('z'));
  EXPECT_FALSE(IsDigit('A'));
  EXPECT_FALSE(IsDigit('-'));

  EXPECT_FALSE(IsDigit(-1));
}

}  // namespace radio_core
