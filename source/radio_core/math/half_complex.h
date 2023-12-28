// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Complex number data type which uses half precision floating point to store
// the real an imaginary parts.

#pragma once

#include "radio_core/base/half.h"

#if RADIO_CORE_HAVE_HALF

#  include "radio_core/math/base_complex.h"

namespace radio_core {

using HalfComplex = BaseComplex<Half>;

}  // namespace radio_core

#endif  // RADIO_CORE_HAVE_HALF
