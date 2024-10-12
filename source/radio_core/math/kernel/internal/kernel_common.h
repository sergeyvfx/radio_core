// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/vectorized_complex_type.h"
#include "radio_core/math/vectorized_float_type.h"

#include "radio_core/math/complex4.h"
#include "radio_core/math/complex8.h"
#include "radio_core/math/float4.h"
#include "radio_core/math/float8.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half4.h"
#  include "radio_core/math/half8.h"
#  include "radio_core/math/half_complex4.h"
#  include "radio_core/math/half_complex8.h"
#endif

namespace radio_core::kernel::kernel_internal {

// Accessor to a vectorized type for the type T.
//
// Specialized access VectorizedBase<T>::VectorizedFloatType<Size> which
// defines the vectorized type.
//
// For example, VectorizedBase<float>::VectorizedFloatType<4> resolves to the
// same type as Float4.
//
// Note that to be proper C++ the actual access looks like:
//   using Type4 = typename VectorizedBase<Type>::template VectorizedType<4>;
template <class T>
struct VectorizedBase;

template <>
struct VectorizedBase<float> {
  template <int Size>
  using VectorizedType = VectorizedFloatType<float, Size>;
};

template <>
struct VectorizedBase<BaseComplex<float>> {
  template <int Size>
  using VectorizedType = VectorizedComplexType<float, Size>;
};

#if RADIO_CORE_HAVE_HALF
template <>
struct VectorizedBase<Half> {
  template <int Size>
  using VectorizedType = VectorizedFloatType<Half, Size>;
};

template <>
struct VectorizedBase<BaseComplex<Half>> {
  template <int Size>
  using VectorizedType = VectorizedComplexType<Half, Size>;
};
#endif

}  // namespace radio_core::kernel::kernel_internal
