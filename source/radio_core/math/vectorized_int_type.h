// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for vectorized integer types.
//
// Those types consist of several integer-like values with arithmetic
// operations defined in a way that operate on all those elements, taking
// advantage of SIMD when possible.
//
// TODO(sergey): API overview?

#pragma once

#include <ostream>
#include <type_traits>

#include "radio_core/math/internal/vectorized_type.h"

namespace radio_core {

// Type information of a vectorized type of N elements of integer type T.
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
struct VectorizedIntTypeInfo;

// T is the type of a single element. This type should implement typical
// integer operations. The exact element type implementation is not considered
// here, and it could be one of the built-in types like int or int64_t.
//
// The N is the number of elements of type T in the vectorized type.
//
// The storage type and operation implementation is defined by
// VectorizedIntTypeInfo<T, N, true>.
template <class T, int N>
class VectorizedIntType {
 public:
  using ScalarType = T;
  using TypeInfo = VectorizedIntTypeInfo<T, N, true>;
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

  VectorizedIntType() = default;

  // Construct from given N unsigned int values stored in a buffer assigning
  // them to the corresponding elements.
  //
  // values[0] is the 0-th element of the vectorized type.
  inline explicit VectorizedIntType(const T values[N])
      : register_(TypeInfo::Load(values)) {}

  // Construct from given 4 unsigned int values assigning them to the
  // corresponding elements.
  //
  // The first argument in the pack is the 0-th element of the vectorized type.
  template <class... Args>
  inline VectorizedIntType(const Args&... args)
      : VectorizedIntType(TypeInfo::Load(args...)) {}

  // Construct from given unsigned int value, assigning the same value to all
  // elements.
  inline explicit VectorizedIntType(const T value)
      : VectorizedIntType(TypeInfo::Load(value)) {}

  // Construct vectorized value from specified lower and upper halfs.
  //
  // For example, construct Int8 from lower and upper Int4. Is only available
  // for the vectorized types of at least 8 elements.
  //
  // Templated to conditionally compile for types which has 8 elements and more.
  // It is not supposed to be specialized during the method call.
  template <
      int HalfN = N / 2,
      typename = std::enable_if_t<HalfN >= 4 && (N && ((N & (N - 1)) == 0))>>
  inline VectorizedIntType(const VectorizedIntType<T, HalfN>& low,
                           const VectorizedIntType<T, HalfN>& high)
      : VectorizedIntType(
            TypeInfo::Load(low.GetRegister(), high.GetRegister())) {}

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
  // For example, returns the low (high) Int4 of the Int8 register. Is only
  // available for the vectorized types of at least 8 elements.
  //
  // Templated to conditionally compile for types which has 8 elements and more.
  // It is not supposed to be specialized during the method call.

  template <int HalfN = N / 2>
  inline auto ExtractLow() const
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedIntType<T, HalfN>> {
    return TypeInfo::ExtractLow(GetRegister());
  }

  template <int HalfN = N / 2>
  inline auto ExtractHigh() const
      -> std::enable_if_t<(HalfN == N / 2) &&
                              vectorized_internal::HasLowAndHighParts<N>(),
                          VectorizedIntType<T, HalfN>> {
    return TypeInfo::ExtractHigh(GetRegister());
  }

  //////////////////////////////////////////////////////////////////////////////
  // Lane.
  //
  // Construct a new vectorized value from the current one by replacing a scalar
  // value at the specific lane.

  template <int Index>
  inline auto SetLane(const T new_lane_value) const -> VectorizedIntType {
    static_assert(Index >= 0);
    static_assert(Index < kSize);

    return VectorizedIntType(
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
  inline explicit VectorizedIntType(const RegisterType reg) : register_(reg) {}
  auto GetRegister() const -> const RegisterType& { return register_; }

 private:
  RegisterType register_;
};

template <class T, int N>
inline auto operator<<(std::ostream& os, const VectorizedIntType<T, N>& value)
    -> std::ostream& {
  vectorized_internal::Print(os, value);
  return os;
}

// Per-element minimum value:
//   RESULT[i] = min(a[i], b[i]) for i = 0 to N
template <class T, int N>
inline auto Min(const VectorizedIntType<T, N>& a,
                const VectorizedIntType<T, N>& b) -> VectorizedIntType<T, N> {
  return VectorizedIntType<T, N>(
      VectorizedIntType<T, N>::TypeInfo::Min(a.GetRegister(), b.GetRegister()));
}

// Per-element maximum value:
//   RESULT[i] = max(a[i], b[i]) for i = 0 to N
template <class T, int N>
inline auto Max(const VectorizedIntType<T, N>& a,
                const VectorizedIntType<T, N>& b) -> VectorizedIntType<T, N> {
  return VectorizedIntType<T, N>(
      VectorizedIntType<T, N>::TypeInfo::Max(a.GetRegister(), b.GetRegister()));
}

// Get maximum element of the vector:
//   RESULT = max(a[0], a[1], a[2], a[3])
template <class T, int N>
inline auto HorizontalMax(const VectorizedIntType<T, N>& a) -> T {
  return VectorizedIntType<T, N>::TypeInfo::HorizontalMax(a.GetRegister());
}

// Bitwise select.
//
// Similar to the generic `Select()` functionality, but operates on a wider
// type. Does not carry any semantic: operation is performed on a purely bit
// level.
//
//   RESULT[i] = mask[i] == 1 ? source1[i] : source2[i] for i = 0 to <num bits>
template <class T, int N>
inline auto Select(const VectorizedIntType<T, N>& mask,
                   const VectorizedIntType<T, N>& source1,
                   const VectorizedIntType<T, N>& source2)
    -> VectorizedIntType<T, N> {
  return VectorizedIntType<T, N>(VectorizedIntType<T, N>::TypeInfo::Select(
      mask.GetRegister(), source1.GetRegister(), source2.GetRegister()));
}

// Reverse the order of elements in the vectorized value:
//   RESULT[i] = a[N - i - 1] for i = 0 to N
template <class T, int N>
inline auto Reverse(const VectorizedIntType<T, N>& a)
    -> VectorizedIntType<T, N> {
  return VectorizedIntType<T, N>(
      VectorizedIntType<T, N>::TypeInfo::Reverse(a.GetRegister()));
}

}  // namespace radio_core
