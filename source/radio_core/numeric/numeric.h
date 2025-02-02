// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Matrix and vector classes, and mathematical operations defined for them.

#pragma once

#include "radio_core/math/math.h"
#include "radio_core/numeric/internal/matrix.h"
#include "radio_core/numeric/internal/vector.h"

namespace radio_core {

////////////////////////////////////////////////////////////////////////////////
// Data types.

// Base vector class. Consists of N elements of type T.
template <class T, size_t N>
using Vec = numeric_internal::Vector<T, N>;

// Base matrix class.
template <class T, size_t kNumRows, size_t kNumColumns>
using Mat = numeric_internal::Matrix<T, kNumRows, kNumColumns>;

}  // namespace radio_core
