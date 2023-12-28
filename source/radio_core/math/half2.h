// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Vectorized data type which holds 2 half precision floating point values.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/internal/vectorized_float_scalar.h"
#  include "radio_core/math/ushort2.h"
#  include "radio_core/math/vectorized_float_type.h"

namespace radio_core {

using Half2 = VectorizedFloatType<Half, 2>;

static_assert(alignof(Half2) == alignof(Half2::RegisterType));
static_assert(sizeof(Half2) == sizeof(Half2::RegisterType));

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
