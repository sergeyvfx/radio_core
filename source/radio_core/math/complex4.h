// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 complex values.

#pragma once

#include "radio_core/math/complex.h"
#include "radio_core/math/internal/vectorized_complex_scalar.h"
#include "radio_core/math/vectorized_complex_type.h"

// Some Complex4 operations return Float4, so ensure there are specializers of
// vectorized type available.
#include "radio_core/math/float4.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#include "radio_core/math/complex2.h"

#include "radio_core/math/internal/complex4_neon.h"
#include "radio_core/math/internal/complex4_x86.h"

namespace radio_core {

using Complex4 = VectorizedComplexType<float, 4>;

static_assert(alignof(Complex4) == alignof(Complex4::RegisterType));
static_assert(sizeof(Complex4) == sizeof(Complex4::RegisterType));

}  // namespace radio_core
