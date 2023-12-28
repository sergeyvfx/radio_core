// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 3 single precision floating point values.

#pragma once

#include "radio_core/math/internal/vectorized_float_scalar.h"
#include "radio_core/math/uint3.h"
#include "radio_core/math/vectorized_float_type.h"

namespace radio_core {

using Float3 = VectorizedFloatType<float, 3>;

static_assert(alignof(Float3) == alignof(Float3::RegisterType));
static_assert(sizeof(Float3) == sizeof(Float3::RegisterType));

}  // namespace radio_core
