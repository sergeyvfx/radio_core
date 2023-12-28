// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 half precision floating point values.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/internal/vectorized_float_scalar.h"
#  include "radio_core/math/ushort8.h"
#  include "radio_core/math/vectorized_float_type.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#  include "radio_core/math/half4.h"

#  include "radio_core/math/internal/half8_half4x2.h"
#  include "radio_core/math/internal/half8_neon.h"

// TODO(sergey): Implementation which operates on float16x8_t on Neon.

namespace radio_core {

using Half8 = VectorizedFloatType<Half, 8>;

static_assert(alignof(Half8) == alignof(Half8::RegisterType));
static_assert(sizeof(Half8) == sizeof(Half8::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
