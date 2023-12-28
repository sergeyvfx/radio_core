// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 3 complex values.

#pragma once

#include "radio_core/math/complex.h"
#include "radio_core/math/internal/vectorized_complex_scalar.h"
#include "radio_core/math/vectorized_complex_type.h"

// Some Complex3 operations return Float3, so ensure there are specializers of
// vectorized type available.
#include "radio_core/math/float3.h"

namespace radio_core {

using Complex3 = VectorizedComplexType<float, 3>;

static_assert(alignof(Complex3) == alignof(Complex3::RegisterType));
static_assert(sizeof(Complex3) == sizeof(Complex3::RegisterType));

}  // namespace radio_core
