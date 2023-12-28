// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/aligned_allocator.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

class MyClass {
 public:
  int foo;
  float bar;
};

TEST(AlignedAllocator, Basic) {
  using aligned_malloc_internal::IsAligned;

  {
    AlignedAllocator<MyClass, 32> allocator;
    MyClass* ptr = allocator.allocate(128);

    // Check the alignment is the expected one.
    EXPECT_TRUE(IsAligned(ptr, 32));

    // Make sure all writes are possible.
    // While it might not crash in release, building with ASAN will show the
    // issue.
    for (int i = 0; i < 128; ++i) {
      ptr[i] = {1, 2};
    }

    allocator.deallocate(ptr, 128);
  }
}

TEST(AlignedAllocator, STLContainer) {
  std::vector<MyClass, AlignedAllocator<MyClass, 16>> vec;
  vec.push_back({1, 2});
}

TEST(AlignedAllocator, Rebind) {
  using AllocatorForInt = AlignedAllocator<int, 16>;
  using AllocatorForClass =
      std::allocator_traits<AllocatorForInt>::template rebind_alloc<MyClass>;

  AllocatorForClass allocator;

  MyClass* ptr = allocator.allocate(1);
  *ptr = {1, 2};

  allocator.deallocate(ptr, 1);
}

}  // namespace radio_core
