// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core {

// Checks if the given character is one of the 10 decimal digits: 0123456789.
//
// It is similar to standard isdigit() but does not raise any exceptions and
// does not access any locale information.
//
// For any character outside of unsigned char range will return false.
inline auto IsDigit(const int ch) -> bool { return ch >= '0' && ch <= '9'; }

}  // namespace radio_core
