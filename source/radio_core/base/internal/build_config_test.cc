// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/build_config.h"

#include <bit>
#include <cstdint>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(BuildConfig, Endian) {
#if ARCH_CPU_BIG_ENDIAN
  EXPECT_EQ(std::endian::native, std::endian::big);
#elif ARCH_CPU_LITTLE_ENDIAN
  EXPECT_EQ(std::endian::native, std::endian::little);
#else
  ADD_FAILURE() << "Unhandled endian check.";
#endif
}

}  // namespace radio_core
