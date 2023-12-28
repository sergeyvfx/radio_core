// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/aligned_malloc.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(aligned_malloc, IsAligned) {
  using aligned_malloc_internal::IsAligned;

  EXPECT_FALSE(IsAligned(1, 16));
  EXPECT_FALSE(IsAligned(7, 16));
  EXPECT_FALSE(IsAligned(15, 16));

  EXPECT_TRUE(IsAligned(16, 16));
  EXPECT_TRUE(IsAligned(32, 16));
  EXPECT_TRUE(IsAligned(64, 16));

  EXPECT_FALSE(IsAligned(65, 16));
}

TEST(aligned_malloc, AlignedMalloc) {
  using aligned_malloc_internal::IsAligned;

  {
    int* ptr = static_cast<int*>(AlignedMalloc(sizeof(int) * 128, 32));

    // Check the alignment is the expected one.
    EXPECT_TRUE(IsAligned(ptr, 32));

    // Make sure all writes are possible.
    // While it might not crash in release, building with ASAN will show the
    // issue.
    for (int i = 0; i < 128; ++i) {
      ptr[i] = 123;
    }

    AlignedFree(ptr);
  }
}

}  // namespace radio_core
