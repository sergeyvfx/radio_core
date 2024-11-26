// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Utilities on a strings.
//
// The inputs are typically views on the string. Whenever an owning string is
// needed (for example for result value) it is templated to allow using these
// utilities on all types of strings which implements std::string-like API.

#pragma once

#include <cstdarg>
#include <string_view>

#define TL_STRING_PORTABLE_NAMESPACE                                           \
  radio_core::bundled_tiny_lib::string_portable
#include "radio_core/base/internal/tiny_lib/tl_string_portable.h"

namespace radio_core {

// Trip leading and trailing whitespace.
// Assumes the typical ASCII space characters: space, tab, new line, and caret
// return.
//
// The return type is specified by the template argument.
template <class String>
auto TrimWhitespaceASCII(const std::string_view str) -> String {
  constexpr const char* kWhitespaceCharacters = " \t\r\n";

  String result(str);

  result.erase(0, result.find_first_not_of(kWhitespaceCharacters));
  result.erase(result.find_last_not_of(kWhitespaceCharacters) + 1);

  return result;
}

// Replace all occurrences of needle in haystack.
//
// The return type is specified by the template argument. This type needs to
// be able to hold the result of the replacement (in the terms of its maximum
// capacity). Otherwise the call will throw std::length_error.
template <class String>
auto StringReplaceAll(const std::string_view haystack,
                      const std::string_view needle,
                      const std::string_view replace) -> String {
  if (needle.empty()) {
    return String(haystack);
  }

  const size_t needle_length = needle.length();
  const size_t replace_length = replace.length();

  String result(haystack);

  size_t start_pos = 0;
  while ((start_pos = result.find(needle, start_pos)) !=
         std::string_view::npos) {
    result.replace(start_pos, needle_length, replace);
    start_pos += replace_length;
  }

  return result;
}

// Portable version of `::strncpy` which ensure the result is always
// null-terminated as long as the destination has a non-zero size.
//
// Copies characters from a null-terminated `src` to `dst`. At maximum of
// `dst_size` will be written to the `dst` (including the null-terminator).
inline auto StringCopy(char* dst, const char* src, const size_t dst_size)
    -> size_t {
  return bundled_tiny_lib::string_portable::strncpy(dst, src, dst_size);
}

// Portable version of `::vsnprintf()` which ensures the result is always
// null-terminated.
//
// Return the number of characters that would have been printed if the buffer
// size was unlimited, not including the null-terminator.
inline auto StringPrintFormatList(char* buffer,
                                  const size_t buffer_size,
                                  const char* format,
                                  va_list arg) -> int {
  return bundled_tiny_lib::string_portable::vsnprintf(
      buffer, buffer_size, format, arg);
}

// Portable version of `::snprintf()` which ensures the result is always
// null-terminated.
//
// Return the number of characters that would have been printed if the buffer
// size was unlimited, not including the null-terminator.
inline auto StringPrintFormat(char* buffer,
                              const size_t buffer_size,
                              const char* format,
                              ...) -> int {
  va_list arg;
  va_start(arg, format);
  const int num_printed =
      StringPrintFormatList(buffer, buffer_size, format, arg);
  va_end(arg);
  return num_printed;
}

}  // namespace radio_core
