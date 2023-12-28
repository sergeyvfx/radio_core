// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 2 32 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

namespace radio_core {

using UInt2 = VectorizedIntType<uint32_t, 2>;

static_assert(alignof(UInt2) == alignof(UInt2::RegisterType));
static_assert(sizeof(UInt2) == sizeof(UInt2::RegisterType));

}  // namespace radio_core
