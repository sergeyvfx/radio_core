// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 8 single precision floating point values.

#pragma once

#include "radio_core/math/internal/vectorized_float_scalar.h"
#include "radio_core/math/uint8.h"
#include "radio_core/math/vectorized_float_type.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#include "radio_core/math/float4.h"

#include "radio_core/math/internal/float8_float4x2.h"

namespace radio_core {

using Float8 = VectorizedFloatType<float, 8>;

static_assert(alignof(Float8) == alignof(Float8::RegisterType));
static_assert(sizeof(Float8) == sizeof(Float8::RegisterType));

}  // namespace radio_core
