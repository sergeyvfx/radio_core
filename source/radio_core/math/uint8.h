// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 8 32 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#include "radio_core/math/uint4.h"

#include "radio_core/math/internal/uint8_uint4x2.h"

namespace radio_core {

using UInt8 = VectorizedIntType<uint32_t, 8>;

static_assert(alignof(UInt8) == alignof(UInt8::RegisterType));
static_assert(sizeof(UInt8) == sizeof(UInt8::RegisterType));

}  // namespace radio_core
