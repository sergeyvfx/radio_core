// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 4 half precision floating point values.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/internal/vectorized_float_scalar.h"
#  include "radio_core/math/ushort4.h"
#  include "radio_core/math/vectorized_float_type.h"

// Types for extracting lower and upper parts, and constructing from 2 parts.
#  include "radio_core/math/half2.h"

#  include "radio_core/math/internal/half4_neon.h"

namespace radio_core {

using Half4 = VectorizedFloatType<Half, 4>;

static_assert(alignof(Half4) == alignof(Half4::RegisterType));
static_assert(sizeof(Half4) == sizeof(Half4::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
