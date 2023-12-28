// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 2 single precision floating point values.

#pragma once

#include "radio_core/math/internal/vectorized_float_scalar.h"
#include "radio_core/math/uint2.h"
#include "radio_core/math/vectorized_float_type.h"

namespace radio_core {

using Float2 = VectorizedFloatType<float, 2>;

static_assert(alignof(Float2) == alignof(Float2::RegisterType));
static_assert(sizeof(Float2) == sizeof(Float2::RegisterType));

}  // namespace radio_core
