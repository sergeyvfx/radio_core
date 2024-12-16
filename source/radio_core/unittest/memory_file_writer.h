// Copyright (c) 2022 radio code authors
//
// SPDX-License-Identifier: MIT

// Utilities that provide file-like operations for regression tests.
// Adopted from the Tiny Lib project.

#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

namespace radio_core::testing {

// Minimalistic implementation of a file writer interface which writes data to
// an in-memory container, namely given std::vector.
class FileWriterToMemory {
 public:
  explicit FileWriterToMemory(std::vector<uint8_t>& memory_buffer);

  // Rewind the current write position. Next write operation will happen at the
  // beginning of the file, overwriting its current content.
  auto Rewind() -> bool;

  auto Write(const void* ptr, const int num_bytes_to_write) -> int;

 private:
  std::vector<uint8_t>& memory_buffer_;

  int position_{0};
};

// Simple file writer which also owns the memory it has writen to.
class SimpleFileWriterToMemory : public FileWriterToMemory {
 public:
  std::vector<uint8_t> buffer;

  SimpleFileWriterToMemory() : FileWriterToMemory(buffer) {}
};

}  // namespace radio_core::testing
