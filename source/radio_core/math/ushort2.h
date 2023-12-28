// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 2 16 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

namespace radio_core {

using UShort2 = VectorizedIntType<uint16_t, 2>;

static_assert(alignof(UShort2) == alignof(UShort2::RegisterType));
static_assert(sizeof(UShort2) == sizeof(UShort2::RegisterType));

}  // namespace radio_core
