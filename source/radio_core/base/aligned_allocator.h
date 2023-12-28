// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// STL compatible allocator implementation which allocates memory with address
// being a multiple of the given alignment.

#pragma once

#include <cstddef>

#include "radio_core/base/aligned_malloc.h"

namespace radio_core {

template <class T, size_t kAlignment>
struct AlignedAllocator {
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using propagate_on_container_move_assignment = std::true_type;

  AlignedAllocator() = default;

  template <class U>
  constexpr AlignedAllocator(const AlignedAllocator<U, kAlignment>&) noexcept {}

  template <class U>
  struct rebind {
    using other = AlignedAllocator<U, kAlignment>;
  };

  [[nodiscard]] auto allocate(const std::size_t n) -> T* {
    return static_cast<T*>(AlignedMalloc(sizeof(T) * n, kAlignment));
  }

  void deallocate(T* ptr, const std::size_t /*n*/) noexcept {
    AlignedFree(ptr);
  }

  template <class U>
  bool operator==(const AlignedAllocator<U, kAlignment>& /*other*/) {
    return true;
  }
  template <class U>
  bool operator!=(const AlignedAllocator<U, kAlignment>& /*other*/) {
    return false;
  }
};

}  // namespace radio_core
