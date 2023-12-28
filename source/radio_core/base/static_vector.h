// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// A fixed capacity dynamically sized vector.
//
// Static vector implements C++ vector API and used in-object storage of a
// static size. No allocations will happen be performed by the static vector.
//
//
// Exceptions
// ==========
//
// General notes on exceptions:
//
//  - If an exception is thrown for any reason, functions have effect (strong
//    exception guarantee).
//
//  - If an operation would result in size() > max_size(), an std::length_error
//    exception is throw.

#pragma once

#define TL_STATIC_VECTOR_NAMESPACE radio_core::bundled_tiny_lib::static_vector
#include "radio_core/base/internal/tiny_lib/tl_static_vector.h"

namespace radio_core {

using bundled_tiny_lib::static_vector::StaticVector;

}  // namespace radio_core
