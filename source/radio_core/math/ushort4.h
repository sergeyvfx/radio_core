// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 16 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

#include "radio_core/math/internal/ushort4_neon.h"

namespace radio_core {

using UShort4 = VectorizedIntType<uint16_t, 4>;

static_assert(alignof(UShort4) == alignof(UShort4::RegisterType));
static_assert(sizeof(UShort4) == sizeof(UShort4::RegisterType));

}  // namespace radio_core
