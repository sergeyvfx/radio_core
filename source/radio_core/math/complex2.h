// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 2 complex values.

#pragma once

#include "radio_core/math/complex.h"
#include "radio_core/math/internal/vectorized_complex_scalar.h"
#include "radio_core/math/vectorized_complex_type.h"

// Some Complex2 operations return Float2, so ensure there are specializers of
// vectorized type available.
#include "radio_core/math/float2.h"

namespace radio_core {

using Complex2 = VectorizedComplexType<float, 2>;

static_assert(alignof(Complex2) == alignof(Complex2::RegisterType));
static_assert(sizeof(Complex2) == sizeof(Complex2::RegisterType));

}  // namespace radio_core
