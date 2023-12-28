// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/unreachable.h"

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

namespace {

void UnreachableCodeTrigger() { UNREACHABLE(); }

}  // namespace

TEST(base, unreachable) {
  // NOTE: Skip the function name check as it is compiler-dependent.
  EXPECT_DEATH_IF_SUPPORTED(UnreachableCodeTrigger(),
                            "file unreachable_test.cc, line 14");
}

}  // namespace radio_core
