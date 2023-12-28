// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 3 32 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

namespace radio_core {

using UInt3 = VectorizedIntType<uint32_t, 3>;

static_assert(alignof(UInt3) == alignof(UInt3::RegisterType));
static_assert(sizeof(UInt3) == sizeof(UInt3::RegisterType));

}  // namespace radio_core
