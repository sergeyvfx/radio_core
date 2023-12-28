// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core {

// If the exceptions are enabled throws a new exception of the given type
// passing the given expression_str to its constructor as a reason. If the
// exceptions are disabled aborts the program execution.
template <class Exception, class... Args>
[[noreturn]] inline constexpr void ThrowOrAbort(
    [[maybe_unused]] Args&&... args) {
#if defined(__cpp_exceptions) && __cpp_exceptions >= 199711L
  throw Exception(std::forward<Args>(args)...);
#else
  __builtin_abort();
#endif
}

}  // namespace radio_core
