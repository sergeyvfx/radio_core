// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 3 16 bit unsigned integer values.

#pragma once

#include <cstdint>

#include "radio_core/math/internal/vectorized_int_scalar.h"
#include "radio_core/math/vectorized_int_type.h"

namespace radio_core {

using UShort3 = VectorizedIntType<uint16_t, 3>;

static_assert(alignof(UShort3) == alignof(UShort3::RegisterType));
static_assert(sizeof(UShort3) == sizeof(UShort3::RegisterType));

}  // namespace radio_core
