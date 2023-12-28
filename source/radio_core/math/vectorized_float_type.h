// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for vectorized floating point types.
//
// Those types consist of several floating point-like values with arithmetic
// operations defined in a way that operate on all those elements, taking
// advantage of SIMD when possible.
//
// TODO(sergey): API overview?

#pragma once

#include <ostream>
#include <type_traits>

#include "radio_core/math/internal/vectorized_type.h"

namespace radio_core {

// Type information of a vectorized type of N elements of floating point type T.
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
struct VectorizedFloatTypeInfo;

// T is the type of a single element. This type should implement typical
// floating point operations. The exact element type implementation is not
// considered here, and it could be one of the built-in types like float or
// double, or extension type like _Float16, or a custom class like Jet or
// a double-double type.
//
// The N is the number of elements of type T in the vectorized type.
//
// The storage type and operation implementation is defined by
// VectorizedFloatTypeInfo<T, N, true>.
template <class T, int N>
class VectorizedFloatType {
 public:
  using ScalarType = T;
  using TypeInfo = VectorizedFloatTypeInfo<T, N, true>;
  using RegisterType = typename TypeInfo::RegisterType;
  using MaskType = typename TypeInfo::MaskType;

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

  VectorizedFloatType() = default;

  // Construct from given N floating point values stored in a buffer assigning
  // them to the corresponding elements.
  //
  // values[0] is the 0-th element of the vectorized type.
  inline explicit VectorizedFloatType(const T values[N])
      : register_(TypeInfo::Load(values)) {}

  // Construct from given N floating point values assigning them to the
  // corresponding elements.
  //
  // The first argument in the pack is the 0-th element of the vectorized type.
  template <class... Args>
  inline VectorizedFloatType(const Args&... args)
      : VectorizedFloatType(TypeInfo::Load(args...)) {}

  // Construct from given floating point value, assigning the same value to all
  // elements.
  inline explicit VectorizedFloatType(const T value)
      : VectorizedFloatType(TypeInfo::Load(value)) {}

  // Construct vectorized value from specified lower and upper halfs.
  //
  // For example, construct Float8 from lower and upper Float4. Is only
  // available for the vectorized types of at least 8 elements.
  //
  // Templated to conditionally compile for types which has 8 elements and more.
  // It is not supposed to be specialized during the method call.
  template <
      int HalfN = N / 2,
      typename = std::enable_if_t<HalfN >= 4 && (N && ((N & (N - 1)) == 0))>>
  inline VectorizedFloatType(const VectorizedFloatType<T, HalfN>& low,
                             const VectorizedFloatType<T, HalfN>& high)
      : VectorizedFloatType(
            TypeInfo::Load(low.GetRegister(), high.GetRegister())) {}

  //////////////////////////////////////////////////////////////////////////////
  // Unary operations.

  // Per-element negation:
  //   RESULT[i] = -THIS[i] for i = 0 to N
  inline auto operator-() const -> VectorizedFloatType {
    return VectorizedFloatType(TypeInfo::Negate(GetRegister()));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Math between vectorized and scalar types.

  // Per-element multiplication by a scalar:
  //   RESULT[i] = rhs[i] * lhs for i = 0 to N
  friend inline auto operator*(const T lhs, const VectorizedFloatType& rhs)
      -> VectorizedFloatType {
    return VectorizedFloatType(TypeInfo::Multiply(rhs.GetRegister(), lhs));
  }

  // Per-element multiplication by a scalar:
  //   RESULT[i] = lhs[i] * rhs for i = 0 to N
  friend inline auto operator*(const VectorizedFloatType& lhs, const T rhs)
      -> VectorizedFloatType {
    return VectorizedFloatType(TypeInfo::Multiply(lhs.GetRegister(), rhs));
  }

  // Per-element division by a scalar:
  //   RESULT[i] = lhs[i] / rhs for i = 0 to N
  friend inline auto operator/(const VectorizedFloatType& lhs, const T rhs)
      -> VectorizedFloatType {
    return VectorizedFloatType(TypeInfo::Divide(lhs.GetRegister(), rhs));
  }

  // Math between 2 vectorized registers.

  // Per-element sum:
  //   RESULT[i] = this[i] + rhs[i] for i = 0 to N
  inline auto operator+=(const VectorizedFloatType& rhs)
      -> VectorizedFloatType& {
    *this = *this + rhs;
    return *this;
  }

  // Per-element difference:
  //   RESULT[i] = this[i] - rhs[i] for i = 0 to N
  inline auto operator-=(const VectorizedFloatType& rhs)
      -> VectorizedFloatType& {
    *this = *this - rhs;
    return *this;
  }

  // Per-element multiplication:
  //   RESULT[i] = this[i] * rhs[i] for i = 0 to N
  inline auto operator*=(const VectorizedFloatType& rhs)
      -> VectorizedFloatType& {
    *this = *this * rhs;
    return *this;
  }

  // Per-element division:
  //   RESULT[i] = lhs[i] / rhs[i] for i = 0 to N
  inline auto operator/=(const VectorizedFloatType& rhs)
      -> VectorizedFloatType& {
    *this = *this / rhs;
    return *this;
  }

  // Per-element sum:
  //   RESULT[i] = lhs[i] + rhs[i] for i = 0 to N
  friend inline auto operator+(const VectorizedFloatType& lhs,
                               const VectorizedFloatType& rhs) {
    return VectorizedFloatType(
        TypeInfo::Add(lhs.GetRegister(), rhs.GetRegister()));
  }

  // Per-element difference:
  //   RESULT[i] = lhs[i] - rhs[i] for i = 0 to N
  friend inline auto operator-(const VectorizedFloatType& lhs,
                               const VectorizedFloatType& rhs) {
    return VectorizedFloatType(
        TypeInfo::Subtract(lhs.GetRegister(), rhs.GetRegister()));
  }

  // Per-element multiplication:
  //   RESULT[i] = lhs[i] * rhs[i] for i = 0 to N
  friend inline auto operator*(const VectorizedFloatType& lhs,
                               const VectorizedFloatType& rhs) {
    return VectorizedFloatType(
        TypeInfo::Multiply(lhs.GetRegister(), rhs.GetRegister()));
  }

  // Per-element division:
  //   RESULT[i] = lhs[i] / rhs[i] for i = 0 to N
  friend inline auto operator/(const VectorizedFloatType& lhs,
                               const VectorizedFloatType& rhs) {
    return VectorizedFloatType(
        TypeInfo::Divide(lhs.GetRegister(), rhs.GetRegister()));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparators.

  // Per-element compare less than:
  //   RESULT[i] = lhs[i] < rhs[i] ? 0xffffffff : 0 for i = 0 to N
  friend inline auto operator<(const VectorizedFloatType& lhs,
                               const VectorizedFloatType& rhs) -> MaskType {
    return TypeInfo::LessThan(lhs.GetRegister(), rhs.GetRegister());
  }

  // Per-element compare greater than:
  //   RESULT[i] = lhs[i] < rhs[i] ? 0xffffffff : 0 for i = 0 to N
  friend inline auto operator>(const VectorizedFloatType& lhs,
                               const VectorizedFloatType& rhs) -> MaskType {
    return TypeInfo::GreaterThan(lhs.GetRegister(), rhs.GetRegister());
  }

  //////////////////////////////////////////////////////////////////////////////
  // Store.

  // Store all values from this vector into the given memory.
  inline void Store(T dst[N]) const { TypeInfo::Store(GetRegister(), dst); }

  // Store a single element with a given index in the given destination.
  template <int Index>
  inline void Store(T* dst) const {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    TypeInfo::template Store<Index>(GetRegister(), dst);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Extract.

  // Access single element with the given index.
  template <int Index>
  inline auto Extract() const -> T {
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
                          VectorizedFloatType<T, HalfN>> {
    return TypeInfo::ExtractLow(GetRegister());
  }

  template <int HalfN = N / 2>
  inline auto ExtractHigh() const
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedFloatType<T, HalfN>> {
    return TypeInfo::ExtractHigh(GetRegister());
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.
  //
  // Construct a new vectorized value from the current one by replacing a scalar
  // value at the specific lane.

  template <int Index>
  inline auto SetLane(const T new_lane_value) const -> VectorizedFloatType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return VectorizedFloatType(
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
  inline auto x() const -> std::enable_if_t<N >= 1 && std::is_same_v<T, U>, U> {
    return Extract<0>();
  }
  template <class U = T>
  inline auto x(const U& arg)
      -> std::enable_if_t<N >= 1 && std::is_convertible_v<U, T>, void> {
    *this = SetLane<0>(arg);
  }

  template <class U = T>
  inline auto y() const -> std::enable_if_t<N >= 2 && std::is_same_v<T, U>, U> {
    return Extract<1>();
  }
  template <class U = T>
  inline auto y(const U& arg)
      -> std::enable_if_t<N >= 2 && std::is_convertible_v<U, T>, void> {
    *this = SetLane<1>(arg);
  }

  template <class U = T>
  inline auto z() const -> std::enable_if_t<N >= 3 && std::is_same_v<T, U>, U> {
    return Extract<2>();
  }
  template <class U = T>
  inline auto z(const U& arg)
      -> std::enable_if_t<N >= 3 && std::is_convertible_v<U, T>, void> {
    *this = SetLane<2>(arg);
  }

  template <class U = T>
  inline auto w() const -> std::enable_if_t<N >= 4 && std::is_same_v<T, U>, U> {
    return Extract<3>();
  }
  template <class U = T>
  inline auto w(const U& arg)
      -> std::enable_if_t<N >= 4 && std::is_convertible_v<U, T>, void> {
    *this = SetLane<3>(arg);
  }

  // TODO(sergey): Cover wider registers.

  //////////////////////////////////////////////////////////////////////////////
  // Construction from vector register and access to the register.
  //
  // Used for constructing values from other types (i.e. from complex32fx4_t)
  // and for out-of-class functions implementation.
  inline explicit constexpr VectorizedFloatType(const RegisterType reg)
      : register_(reg) {}
  auto GetRegister() const -> const RegisterType& { return register_; }

 private:
  RegisterType register_;
};

template <class T, int N>
inline auto operator<<(std::ostream& os, const VectorizedFloatType<T, N>& value)
    -> std::ostream& {
  vectorized_internal::Print(os, value);
  return os;
}

// Calculate per-element logarithm with base 10:
//   RESULT[i] = FastLog10(a[i]) for i = 0 to N
//
// Tries to be fastest for the native platform without sacrificing much of
// precision.
template <class T, int N>
inline auto FastLog10(const VectorizedFloatType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(
      VectorizedFloatType<T, N>::TypeInfo::FastLog10(a.GetRegister()));
}

// Calculate per-element absolute value:
//   RESULT[i] = Abs(a[i]) for i = 0 to N
template <class T, int N>
inline auto Abs(const VectorizedFloatType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(
      VectorizedFloatType<T, N>::TypeInfo::Abs(a.GetRegister()));
}

// Per-element minimum value:
//   RESULT[i] = min(a[i], b[i]) for i = 0 to N
template <class T, int N>
inline auto Min(const VectorizedFloatType<T, N>& a,
                const VectorizedFloatType<T, N>& b)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(VectorizedFloatType<T, N>::TypeInfo::Min(
      a.GetRegister(), b.GetRegister()));
}

// Per-element maximum value:
//   RESULT[i] = max(a[i], b[i]) for i = 0 to N
template <class T, int N>
inline auto Max(const VectorizedFloatType<T, N>& a,
                const VectorizedFloatType<T, N>& b)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(VectorizedFloatType<T, N>::TypeInfo::Max(
      a.GetRegister(), b.GetRegister()));
}

// Get maximum element of the vector:
//   RESULT = max(a[0], a[1], a[2], a[3])
template <class T, int N>
inline auto HorizontalMax(const VectorizedFloatType<T, N>& a) -> T {
  return VectorizedFloatType<T, N>::TypeInfo::HorizontalMax(a.GetRegister());
}

// Sum of all elements:
//  RESULT = a[0] + a[1] + a[2] + a[3]
template <class T, int N>
inline auto HorizontalSum(const VectorizedFloatType<T, N>& a) -> T {
  return VectorizedFloatType<T, N>::TypeInfo::HorizontalSum(a.GetRegister());
}

// Multiply-add to accumulator:
//   RESULT[i] = a[i] + (b[i] * c[i]) for i = 0 to N
template <class T, int N>
inline auto MultiplyAdd(const VectorizedFloatType<T, N>& a,
                        const VectorizedFloatType<T, N>& b,
                        const VectorizedFloatType<T, N>& c)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(
      VectorizedFloatType<T, N>::TypeInfo::MultiplyAdd(
          a.GetRegister(), b.GetRegister(), c.GetRegister()));
}

// Bitwise select.
//
// Similar to the generic `Select()` functionality, but operates on a wider
// type. Does not carry any semantic: operation is performed on a purely bit
// level.
//
//   RESULT[i] = mask[i] == 1 ? source1[i] : source2[i] for i = 0 to <num bits>
template <class T, int N>
inline auto Select(const typename VectorizedFloatType<T, N>::MaskType& mask,
                   const VectorizedFloatType<T, N>& source1,
                   const VectorizedFloatType<T, N>& source2)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(VectorizedFloatType<T, N>::TypeInfo::Select(
      mask, source1.GetRegister(), source2.GetRegister()));
}

// Per-element sign calculation.
// sign of sgn:
//   RESULT[i] = Sign(arg[i]) for i = 0 to N
template <class T, int N>
inline auto Sign(const VectorizedFloatType<T, N>& arg)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(
      VectorizedFloatType<T, N>::TypeInfo::Sign(arg.GetRegister()));
}

// Composes a per-element floating point value with the magnitude of mag and the
// sign of sgn:
//   RESULT[i] = CopySign(mag[i], sgn[i]) for i = 0 to N
template <class T, int N>
inline auto CopySign(const VectorizedFloatType<T, N>& mag,
                     const VectorizedFloatType<T, N>& sgn)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(
      VectorizedFloatType<T, N>::TypeInfo::CopySign(mag.GetRegister(),
                                                    sgn.GetRegister()));
}

// Reverse the order of elements in the vectorized value:
//   RESULT[i] = a[N - i - 1] for i = 0 to N
template <class T, int N>
inline auto Reverse(const VectorizedFloatType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  return VectorizedFloatType<T, N>(
      VectorizedFloatType<T, N>::TypeInfo::Reverse(a.GetRegister()));
}

// Dot product of two vectorrized values.
//   RESULT = Sum from i to N (a[i] * b[i])
template <class T, int N>
inline auto Dot(const VectorizedFloatType<T, N>& a,
                const VectorizedFloatType<T, N>& b) -> T {
  return HorizontalSum(a * b);
}

////////////////////////////////////////////////////////////////////////////////
// Linear algebra.

namespace linalg {

// Calculate a Frobenius norm of the vector norm:
//   RESULT = Sqrt( Sum from i to N (a[i]) )
template <class T, int N>
inline auto Norm(const VectorizedFloatType<T, N>& a) -> T {
  return VectorizedFloatType<T, N>::TypeInfo::Norm(a.GetRegister());
}

// Calculate length of the vector.
// Same as Frobenius norm but with a more clear naming for certain use-cases.
template <class T, int N>
inline auto Length(const VectorizedFloatType<T, N>& a) -> T {
  return Norm(a);
}

// Calculate squared length of the vector.
template <class T, int N>
inline auto SquaredLength(const VectorizedFloatType<T, N>& a) -> T {
  return VectorizedFloatType<T, N>::TypeInfo::SquaredNorm(a.GetRegister());
}

// Normalize a to a unit vector:
//   RESULT = a / Norm(a)
//
// If the norm of the vector is 0 it is returned as-is.
template <class T, int N>
inline auto Normalize(const VectorizedFloatType<T, N>& a)
    -> VectorizedFloatType<T, N> {
  const T norm = Norm(a);
  if (norm == T(0)) {
    return a;
  }

  return a / norm;
}

}  // namespace linalg

}  // namespace radio_core
