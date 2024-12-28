// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for vectorized complex types.
//
// Those types consist of several complex values with arithmetic operations
// defined in a way that operate on all those elements, taking advantage of SIMD
// when possible.
//
// TODO(sergey): API overview?

#pragma once

#include <cassert>
#include <ostream>

#include "radio_core/math/complex.h"
#include "radio_core/math/internal/vectorized_type.h"
#include "radio_core/math/vectorized_float_type.h"

namespace radio_core {

// Type information of a vectorized type of N elements of floating point type
// BaseComplex<T>.
//
// Defines the storage and operations on the vectorized type.
//
// The SpecializationMarker allows to have an indirection level of optimization
// by allowing to partially specialize wider vectorized type which uses more
// narrow vectorized type under the hood, and full specialization for the
// implementation which is fully utilizing the current CPU instruction set.
//
// TODO(sergey): Brief explanation of what is expected to be implemented?
template <class T, int N, bool SpecializationMarker>
struct VectorizedComplexTypeInfo;

// T is the type of real/imaginary part of a single element. The vectorized
// element itself is BaseComplex<T>.
//
// The N is the number of elements of type T in the vectorized type.
//
// The storage type and operation implementation is defined by
// VectorizedComplexTypeInfo<T, N, true>.
template <class T, int N>
class VectorizedComplexType {
 public:
  using ScalarType = BaseComplex<T>;
  using TypeInfo = VectorizedComplexTypeInfo<T, N, true>;
  using RegisterType = typename TypeInfo::RegisterType;

  // The number of elements in this vectorized type.
  static constexpr int kSize = N;

  // True when the platform has SIMD-optimized implementation.
  //
  // This static variable allows algorithms to opt-out from optimization code
  // paths if the vectorization is not available. This is because non-vectorized
  // implementation might have worse memory access pattern compared to a purely
  // scalar code path.
  static constexpr bool kIsVectorized = TypeInfo::kIsVectorized;

  static_assert(TypeInfo::kSize == N);

  VectorizedComplexType() = default;

  // Construct from given N complex values stored in a buffer assigning them to
  // the corresponding elements.
  //
  // values[0] is the 0-th element of the vectorized type.
  inline explicit VectorizedComplexType(const BaseComplex<T> values[N])
      : register_(TypeInfo::Load(values)) {}

  // Construct from given N complex values assigning them to the corresponding
  // elements.
  //
  // The first argument in the pack is the 0-th element of the vectorized type.
  template <class... Args>
  inline VectorizedComplexType(const Args&... args)
      : VectorizedComplexType(TypeInfo::Load(args...)) {}

  // Construct from given complex value, assigning the same value to all
  // elements.
  inline explicit VectorizedComplexType(const BaseComplex<T>& value)
      : VectorizedComplexType(TypeInfo::Load(value)) {}

  // Construct from given real and imaginary parts.
  inline VectorizedComplexType(const VectorizedFloatType<T, N>& real,
                               const VectorizedFloatType<T, N>& imag)
      : VectorizedComplexType(
            TypeInfo::Load(real.GetRegister(), imag.GetRegister())) {}

  // Store all values from this vector into the given memory.
  inline void Store(BaseComplex<T> dst[N]) const {
    TypeInfo::Store(GetRegister(), dst);
  }

  // Store a single element with a given index in the given destination.
  template <int Index>
  inline void Store(BaseComplex<T>* dst) const {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    TypeInfo::template Store<Index>(GetRegister(), dst);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  // Per-element negation:
  //   RESULT[i] = -THIS[i] for i = 0 to N
  inline auto operator-() const -> VectorizedComplexType {
    return VectorizedComplexType(TypeInfo::Negate(GetRegister()));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operation between two vectorized registers.

  // Per-element sum:
  //   RESULT[i] = this[i] + rhs[i] for i = 0 to N
  inline auto operator+=(const VectorizedComplexType& rhs)
      -> VectorizedComplexType& {
    *this = *this + rhs;
    return *this;
  }

  // Per-element difference:
  //   RESULT[i] = this[i] - rhs[i] for i = 0 to N
  inline auto operator-=(const VectorizedComplexType& rhs)
      -> VectorizedComplexType& {
    *this = *this - rhs;
    return *this;
  }

  // Per-element multiplication:
  //   RESULT[i] = this[i] * rhs[i] for i = 0 to N
  inline auto operator*=(const VectorizedComplexType& rhs)
      -> VectorizedComplexType& {
    *this = *this * rhs;
    return *this;
  }
  inline auto operator*=(const VectorizedFloatType<T, N>& rhs)
      -> VectorizedComplexType& {
    *this = *this * rhs;
    return *this;
  }

  // Per-element division:
  //   RESULT[i] = lhs[i] / rhs[i] for i = 0 to N
  inline auto operator/=(const VectorizedComplexType& rhs)
      -> VectorizedComplexType& {
    *this = *this / rhs;
    return *this;
  }

  // Per-element sum:
  //   RESULT[i] = lhs[i] + rhs[i] for i = 0 to N
  friend inline auto operator+(const VectorizedComplexType& lhs,
                               const VectorizedComplexType& rhs)
      -> VectorizedComplexType {
    return VectorizedComplexType(
        TypeInfo::Add(lhs.GetRegister(), rhs.GetRegister()));
  }

  // Per-element difference:
  //   RESULT[i] = lhs[i] - rhs[i] for i = 0 to N
  friend inline auto operator-(const VectorizedComplexType& lhs,
                               const VectorizedComplexType& rhs)
      -> VectorizedComplexType {
    return VectorizedComplexType(
        TypeInfo::Subtract(lhs.GetRegister(), rhs.GetRegister()));
  }

  // Per-element multiplication:
  //   RESULT[i] = this[i] * rhs[i] for i = 0 to N
  friend inline auto operator*(const VectorizedComplexType& lhs,
                               const VectorizedComplexType& rhs)
      -> VectorizedComplexType {
    return VectorizedComplexType(
        TypeInfo::Multiply(lhs.GetRegister(), rhs.GetRegister()));
  }
  friend inline auto operator*(const VectorizedComplexType& lhs,
                               const VectorizedFloatType<T, N>& rhs)
      -> VectorizedComplexType {
    return VectorizedComplexType(
        TypeInfo::Multiply(lhs.GetRegister(), rhs.GetRegister()));
  }
  friend inline auto operator*(const VectorizedFloatType<T, N>& lhs,
                               const VectorizedComplexType& rhs)
      -> VectorizedComplexType {
    return VectorizedComplexType(
        TypeInfo::Multiply(rhs.GetRegister(), lhs.GetRegister()));
  }

  // Per-element division:
  //   RESULT[i] = this[i] * rhs[i] for i = 0 to N
  friend inline auto operator/(const VectorizedComplexType& lhs,
                               const VectorizedComplexType& rhs)
      -> VectorizedComplexType {
    return VectorizedComplexType(
        TypeInfo::Divide(lhs.GetRegister(), rhs.GetRegister()));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  // Access single element with the given index.
  template <int Index>
  inline auto Extract() const -> BaseComplex<T> {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return TypeInfo::template Extract<Index>(GetRegister());
  }

  // Extract the low or high half of the register.
  //
  // For example, returns the low (high) Float4 of the Float8 register. Is only
  // available for the vectorized types of at least 8 elements.
  //
  // Templated to conditionally compile for types which has 8 elements and more.
  // It is not supposed to be specialized during the method call.

  template <int HalfN = N / 2>
  inline auto ExtractLow() const
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedComplexType<T, HalfN>> {
    return TypeInfo::ExtractLow(GetRegister());
  }

  template <int HalfN = N / 2>
  inline auto ExtractHigh() const
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedComplexType<T, HalfN>> {
    return TypeInfo::ExtractHigh(GetRegister());
  }

  // Extract all real parts as a vectorized floating point vector:
  //   RESULT[i] = a[i].real for i = 0 to N
  inline auto ExtractReal() const -> VectorizedFloatType<T, N> {
    return TypeInfo::ExtractReal(GetRegister());
  }

  // Extract all imaginary parts as a vectorized floating point vector:
  //   RESULT[i] = a[i].imag for i = 0 to N
  inline auto ExtractImag() const -> VectorizedFloatType<T, N> {
    return TypeInfo::ExtractImag(GetRegister());
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.
  //
  // Construct a new vectorized value from the current one by replacing a scalar
  // value at the specific lane.

  template <int Index>
  inline auto SetLane(const BaseComplex<T> new_lane_value) const
      -> VectorizedComplexType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return VectorizedComplexType(
        TypeInfo::template SetLane<Index>(GetRegister(), new_lane_value));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Access shortcuts.
  //
  // Provides more readable API to access elements of the vectorized type by a
  // common mnemonics such as x(), y(), z(). This is merely a convenience
  // wrapper around the Extract<>() and SetLane() functions to make API look
  // closer to as if the vectorized type was a POD struct with several scalar
  // fields.
  //
  // The methods calls without argument reads the corresponding element of the
  // vector.

  template <class U = T>
  inline auto x() const
      -> std::enable_if_t<N >= 1 && std::is_same_v<T, U>, BaseComplex<U>> {
    return Extract<0>();
  }
  template <class U = T>
  inline auto x(const BaseComplex<U>& arg)
      -> std::enable_if_t<N >= 1 && std::is_same_v<T, U>, void> {
    *this = SetLane<0>(arg);
  }

  template <class U = T>
  inline auto y() const
      -> std::enable_if_t<N >= 2 && std::is_same_v<T, U>, BaseComplex<U>> {
    return Extract<1>();
  }
  template <class U = T>
  inline auto y(const BaseComplex<U>& arg)
      -> std::enable_if_t<N >= 2 && std::is_same_v<T, U>, void> {
    *this = SetLane<1>(arg);
  }

  template <class U = T>
  inline auto z() const
      -> std::enable_if_t<N >= 3 && std::is_same_v<T, U>, BaseComplex<U>> {
    return Extract<2>();
  }
  template <class U = T>
  inline auto z(const BaseComplex<U>& arg)
      -> std::enable_if_t<N >= 3 && std::is_same_v<T, U>, void> {
    *this = SetLane<2>(arg);
  }

  template <class U = T>
  inline auto w() const
      -> std::enable_if_t<N >= 4 && std::is_same_v<T, U>, BaseComplex<U>> {
    return Extract<3>();
  }
  template <class U = T>
  inline auto w(const BaseComplex<U>& arg)
      -> std::enable_if_t<N >= 4 && std::is_same_v<T, U>, void> {
    *this = SetLane<3>(arg);
  }

  // TODO(sergey): Cover wider registers.

  //////////////////////////////////////////////////////////////////////////////
  // Construction from vector register and access to the register.
  //
  // Used for constructing values from other types (i.e. from complex32fx4_t)
  // and for out-of-class functions implementation.
  inline explicit VectorizedComplexType(const RegisterType reg)
      : register_(reg) {}
  auto GetRegister() const -> const RegisterType& { return register_; }

 private:
  RegisterType register_;
};

template <class T, int N>
inline auto operator<<(std::ostream& os,
                       const VectorizedComplexType<T, N>& value)
    -> std::ostream& {
  vectorized_internal::Print(os, value);
  return os;
}

// Calculate the per-element magnitude of a complex number:
//   RESULT[i] = Abs(a[i]) for i = 0 to N
template <class T, int N>
inline auto Abs(const VectorizedComplexType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedComplexType<T, N>::TypeInfo::Abs(a.GetRegister());
}

// Calculate the per-element magnitude of a complex number using faster but less
// accurate arithmetic.
//
// Roughly:
//   RESULT[i] = FastAbs(a[i]) for i = 0 to N
template <class T, int N>
inline auto FastAbs(const VectorizedComplexType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedComplexType<T, N>::TypeInfo::FastAbs(a.GetRegister());
}

// Calculate the per-element squared magnitude of a complex number:
//   RESULT[i] = Norm(a[i]) for i = 0 to N
template <class T, int N>
inline auto Norm(const VectorizedComplexType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedComplexType<T, N>::TypeInfo::Norm(a.GetRegister());
}

// Sum of all elements:
//  RESULT = a[0] + a[1] + a[2] + a[3]
template <class T, int N>
inline auto HorizontalSum(const VectorizedComplexType<T, N>& a)
    -> BaseComplex<T> {
  return BaseComplex<T>(
      VectorizedComplexType<T, N>::TypeInfo::HorizontalSum(a.GetRegister()));
}

// Per-element multiply-add to accumulator:
//   RESULT[i] = a[i] + (b[i] * c[i]) for i = 0 to N
template <class T, int N>
inline auto MultiplyAdd(const VectorizedComplexType<T, N>& a,
                        const VectorizedComplexType<T, N>& b,
                        const VectorizedFloatType<T, N>& c)
    -> VectorizedComplexType<T, N> {
  return VectorizedComplexType<T, N>(
      VectorizedComplexType<T, N>::TypeInfo::MultiplyAdd(
          a.GetRegister(), b.GetRegister(), c.GetRegister()));
}

// Calculates per-element phase angle (in radians) of the complex values:
//   RESULT[i] = FastArg(a[i]) for i = 0 to N
template <class T, int N>
inline auto FastArg(const VectorizedComplexType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedComplexType<T, N>::TypeInfo::FastArg(a.GetRegister());
}

// Calculates per-element complex conjugate:
//   RESULT[i] = Conj(a[i]) for i = 0 to N
template <class T, int N>
inline auto Conj(const VectorizedComplexType<T, N>& a)
    -> VectorizedComplexType<T, N> {
  return VectorizedComplexType<T, N>(
      VectorizedComplexType<T, N>::TypeInfo::Conj(a.GetRegister()));
}

// Compute per-element complex exponential: base-e exponential to the power of
// i*x where i is the imaginary unit.
//   RESULT[i] = ComplexExp(x[i])
template <class T, int N>
inline auto ComplexExp(const VectorizedFloatType<T, N>& x)
    -> VectorizedComplexType<T, N> {
  return VectorizedComplexType<T, N>(
      VectorizedComplexType<T, N>::TypeInfo::ComplexExp(x.GetRegister()));
}

// Compute per-element base-e exponential of the complex number z: the Euler's
// number raised to the power of z.
//   RESULT[i] = Exp(x[i])
template <class T, int N>
inline auto Exp(const VectorizedComplexType<T, N>& x)
    -> VectorizedComplexType<T, N> {
  return VectorizedComplexType<T, N>(
      VectorizedComplexType<T, N>::TypeInfo::Exp(x.GetRegister()));
}

// Reverse the order of elements in the vectorized value:
//   RESULT[i] = a[N - i - 1] for i = 0 to N
template <class T, int N>
inline auto Reverse(const VectorizedComplexType<T, N>& a)
    -> VectorizedComplexType<T, N> {
  return VectorizedComplexType<T, N>(
      VectorizedComplexType<T, N>::TypeInfo::Reverse(a.GetRegister()));
}

}  // namespace radio_core
