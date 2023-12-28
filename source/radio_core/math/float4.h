// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 single precision floating point values.

#pragma once

#include "radio_core/math/internal/vectorized_float_scalar.h"
#include "radio_core/math/uint4.h"  // Bit-wise mask type.
#include "radio_core/math/vectorized_float_type.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#include "radio_core/math/float2.h"

#include "radio_core/math/internal/float4_neon.h"
#include "radio_core/math/internal/float4_x86.h"

namespace radio_core {

using Float4 = VectorizedFloatType<float, 4>;

static_assert(alignof(Float4) == alignof(Float4::RegisterType));
static_assert(sizeof(Float4) == sizeof(Float4::RegisterType));

}  // namespace radio_core
