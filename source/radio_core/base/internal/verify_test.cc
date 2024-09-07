// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/verify.h"

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

namespace {

void FunctionWithVerifyFail() { Verify(0 == 1, "0 == 1"); }

}  // namespace

TEST(base, verify) {
  Verify(1 == 1, "1 == 1");

  EXPECT_DEATH_IF_SUPPORTED(
      FunctionWithVerifyFail(),
      "function .*FunctionWithVerifyFail.*, file verify_test.cc");
}

}  // namespace radio_core
