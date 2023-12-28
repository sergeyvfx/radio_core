// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>

#include "radio_core/base/compiler_specific.h"

#if !HAS_BUILTIN(__builtin_bit_cast)
#  include <cstring>
#endif

namespace radio_core {

// Obtain a value of type To by reinterpreting the object representation of
// From. Every bit in the value representation of the returned To object is
// equal to the corresponding bit in the object representation of from. The
// values of padding bits in the returned To object are unspecified.
//
// This is an implementation of C++'s std::bit_cast.
// Is needed since not all compilers support this C++20 part yet.
//
// More information and the implementation is available at:
//   https://en.cppreference.com/w/cpp/numeric/bit_cast
#if HAS_BUILTIN(__builtin_bit_cast)
template <class To, class From>
constexpr std::enable_if_t<sizeof(To) == sizeof(From) &&
                               std::is_trivially_copyable_v<From> &&
                               std::is_trivially_copyable_v<To>,
                           To>
BitCast(const From& src) noexcept {  // NOLINT(*-use-trailing-return-type)
  return __builtin_bit_cast(To, src);
}
#else
template <class To, class From>
inline std::enable_if_t<sizeof(To) == sizeof(From) &&
                            std::is_trivially_copyable_v<From> &&
                            std::is_trivially_copyable_v<To>,
                        To>
BitCast(const From& src) noexcept {  // NOLINT(*-use-trailing-return-type)
  static_assert(std::is_trivially_constructible_v<To>,
                "This implementation additionally requires "
                "destination type to be trivially constructible");

  To dst;
  std::memcpy(&dst, &src, sizeof(To));
  return dst;
}
#endif

}  // namespace radio_core
