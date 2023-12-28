// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 3 complex values in half precision floating
// point precision.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/half_complex.h"
#  include "radio_core/math/internal/vectorized_complex_scalar.h"
#  include "radio_core/math/vectorized_complex_type.h"

// Some HalfComplex3 operations return Half3, so ensure there are specializers
// of vectorized type available.
#  include "radio_core/math/half3.h"

namespace radio_core {

using HalfComplex3 = VectorizedComplexType<Half, 3>;

static_assert(alignof(HalfComplex3) == alignof(HalfComplex3::RegisterType));
static_assert(sizeof(HalfComplex3) == sizeof(HalfComplex3::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
