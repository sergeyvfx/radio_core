// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 complex values in half precision floating
// point precision.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/half_complex.h"
#  include "radio_core/math/internal/vectorized_complex_scalar.h"
#  include "radio_core/math/vectorized_complex_type.h"

// Some HalfComplex4 operations return Half4, so ensure there are specializers
// of vectorized type available.
#  include "radio_core/math/half4.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#  include "radio_core/math/half_complex2.h"

#  include "radio_core/math/internal/half_complex4_neon.h"

namespace radio_core {

using HalfComplex4 = VectorizedComplexType<Half, 4>;

static_assert(alignof(HalfComplex4) == alignof(HalfComplex4::RegisterType));
static_assert(sizeof(HalfComplex4) == sizeof(HalfComplex4::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
