// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 8 complex values in half precision floating
// point precision.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/half_complex.h"
#  include "radio_core/math/internal/vectorized_complex_scalar.h"
#  include "radio_core/math/vectorized_complex_type.h"

// Some HalfComplex8 operations return Half8, so ensure there are specializers
// of vectorized type available.
#  include "radio_core/math/half8.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#  include "radio_core/math/half_complex4.h"

#  include "radio_core/math/internal/half_complex8_half_complex4x2.h"
#  include "radio_core/math/internal/half_complex8_neon.h"

namespace radio_core {

using HalfComplex8 = VectorizedComplexType<Half, 8>;

static_assert(alignof(HalfComplex8) == alignof(HalfComplex8::RegisterType));
static_assert(sizeof(HalfComplex8) == sizeof(HalfComplex8::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
