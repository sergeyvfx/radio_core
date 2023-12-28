// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdio>
#include <cstdlib>

// Similar to `assert()` but performs checks in release builds as well.
#define VERIFY(expression)                                                     \
  ::radio_core::verify_internal::VerifyImpl(                                   \
      (expression), #expression, __func__, __FILE__, __LINE__)

namespace radio_core::verify_internal {

inline void VerifyImpl(bool expression_eval,
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

}  // namespace radio_core::verify_internal
