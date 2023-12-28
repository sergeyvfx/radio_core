// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Various container utilities.

#pragma once

#include <cstddef>

namespace radio_core {

// Make sure the size of container is at least as big as `required_size`.
// If the container is larger than the requested size then nothing happens,
// otherwise the container is resized to the requested size.
template <class ContainerType>
inline void EnsureSizeAtLeast(ContainerType& container,
                              const size_t required_size) {
  if (container.size() >= required_size) {
    return;
  }

  container.resize(required_size);
}

}  // namespace radio_core
