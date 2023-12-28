// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 3 half precision floating point values.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/internal/vectorized_float_scalar.h"
#  include "radio_core/math/ushort3.h"
#  include "radio_core/math/vectorized_float_type.h"

namespace radio_core {

using Half3 = VectorizedFloatType<Half, 3>;

static_assert(alignof(Half3) == alignof(Half3::RegisterType));
static_assert(sizeof(Half3) == sizeof(Half3::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
