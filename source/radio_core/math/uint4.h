// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 32 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

#include "radio_core/math/internal/uint4_neon.h"
#include "radio_core/math/internal/uint4_x86.h"

namespace radio_core {

using UInt4 = VectorizedIntType<uint32_t, 4>;

static_assert(alignof(UInt4) == alignof(UInt4::RegisterType));
static_assert(sizeof(UInt4) == sizeof(UInt4::RegisterType));

}  // namespace radio_core
