// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 8 complex values in single floating point
// precision.

#pragma once

#include "radio_core/math/complex.h"
#include "radio_core/math/internal/vectorized_complex_scalar.h"
#include "radio_core/math/vectorized_complex_type.h"

#include "radio_core/math/internal/complex8_complex4x2.h"

// Some Complex8 operations return Float8, so ensure there are specializers of
// vectorized type available.
#include "radio_core/math/float8.h"

namespace radio_core {

using Complex8 = VectorizedComplexType<float, 8>;

static_assert(alignof(Complex8) == alignof(Complex8::RegisterType));
static_assert(sizeof(Complex8) == sizeof(Complex8::RegisterType));

}  // namespace radio_core
