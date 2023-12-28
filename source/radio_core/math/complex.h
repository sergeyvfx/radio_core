// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Complex number data type which uses single precision floating point to store
// the real an imaginary parts.

#pragma once

#include "radio_core/math/base_complex.h"

namespace radio_core {

using Complex = BaseComplex<float>;

}  // namespace radio_core
