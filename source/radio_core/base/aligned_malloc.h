// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/base/build_config.h"

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>

#if !OS_MAC && !OS_BSD
#  include <malloc.h>
#else
// Apple does not have malloc.h, so include stdlib instead.
#  include <cstdlib>
#endif

namespace radio_core {

namespace aligned_malloc_internal {

// Check whether the given value is aligned to the given alignment.
// The alignment must be a power of 2.
inline auto IsAligned(const uintptr_t value, const size_t alignment) -> bool {
  assert(std::has_single_bit(alignment));
  return (value & (alignment - 1)) == 0;
}

// Check whether the given pointer satisfies the required alignment.
// The alignment must be a power of 2.
inline auto IsAligned(const void* ptr, const int alignment) -> bool {
  return IsAligned(reinterpret_cast<uintptr_t>(ptr), alignment);
}

}  // namespace aligned_malloc_internal

// Aligned memory allocation.
//
// Allocates `size` bytes of memory such that the base memory address is a
// multiple of the given alignment.
//
// Use `AlignedFree()` to free this memory.
//
// The requested alignment must be a power of 2 at least as large as
// sizeof(void*).
inline void* AlignedMalloc(const size_t size, const size_t alignment) {
  assert(std::has_single_bit(alignment));
  assert(alignment >= sizeof(void*));

  void* result = nullptr;

#if OS_WIN
  result = _aligned_malloc(size, alignment);
#elif OS_MAC || OS_BSD
  if (posix_memalign(&result, alignment, size)) {
    // Non-zero return value means allocation error: either no allocation or bad
    // alignment value.
    result = nullptr;
  }
#else  // This is for Linux.
  result = memalign(alignment, size);
#endif

  return result;
}

// Free memory allocated by `AlignedMalloc()`.
inline void AlignedFree(void* ptr) {
#if OS_WIN
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

}  // namespace radio_core
