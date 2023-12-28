// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 2 complex values in half precision floating
// point precision.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/half_complex.h"
#  include "radio_core/math/internal/vectorized_complex_scalar.h"
#  include "radio_core/math/vectorized_complex_type.h"

// Some HalfComplex2 operations return Half2, so ensure there are specializers
// of vectorized type available.
#  include "radio_core/math/half2.h"

namespace radio_core {

using HalfComplex2 = VectorizedComplexType<Half, 2>;

static_assert(alignof(HalfComplex2) == alignof(HalfComplex2::RegisterType));
static_assert(sizeof(HalfComplex2) == sizeof(HalfComplex2::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
