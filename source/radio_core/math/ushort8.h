// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 16 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#include "radio_core/math/ushort4.h"

#include "radio_core/math/internal/ushort8_neon.h"
#include "radio_core/math/internal/ushort8_ushort4x2.h"

namespace radio_core {

using UShort8 = VectorizedIntType<uint16_t, 8>;

static_assert(alignof(UShort8) == alignof(UShort8::RegisterType));
static_assert(sizeof(UShort8) == sizeof(UShort8::RegisterType));

}  // namespace radio_core
