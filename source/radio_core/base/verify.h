// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Function Verify() checks the input condition and it it is false it aborts
// program execution. This is done regardless of the build type (debug or
// release).
//
// The implementation avoid using macro to provide a short name without risking
// running into symbol conflicts. This requires passing message explicitly, as
// it is not possible to convert expression used as a function argument to
// string.
//
// Conceptually this is similar to Google Log's CHECK() and Python's assert.
//
// Example:
//   Verify(foo == bar, "Expected foo == bar")

#pragma once

#include <cstdio>
#include <cstdlib>

#include "radio_core/base/source_location.h"

namespace radio_core {

namespace verify_internal {

inline void VerifyImpl(const bool expression_eval,
                       const char* expression_str,
                       const char* function,
                       const char* file,
                       const int line) {
  if (expression_eval) {
    return;
  }

  // Extract a base file name from the full file name.
  // Done for possible safety reasons to not expose build environment setup.

  const char* last_char = nullptr;
  for (size_t i = 0; file[i] != '\0'; ++i) {
    if (file[i] == '/' || file[i] == '\\') {
      last_char = file + i;
    }
  }

  const char* base_file_name = last_char ? last_char + 1 : file;

  ::fprintf(stderr,
            "Verification failed: (%s), function %s, file %s, line %d.\n",
            expression_str,
            function,
            base_file_name,
            line);
  ::fflush(stderr);
  ::abort();
}

}  // namespace verify_internal

inline void Verify(
    const bool condition,
    const char* message,
    const source_location location = source_location::current()) {
  verify_internal::VerifyImpl(condition,
                              message,
                              location.function_name(),
                              location.file_name(),
                              location.line());
}

}  // namespace radio_core
