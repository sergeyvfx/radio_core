// Copyright (c) 2022 radio code authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/memory_file_writer.h"

#include <cassert>
#include <span>

namespace radio_core::testing {

FileWriterToMemory::FileWriterToMemory(std::vector<uint8_t>& memory_buffer)
    : memory_buffer_(memory_buffer) {}

auto FileWriterToMemory::Rewind() -> bool {
  position_ = 0;
  return true;
}

auto FileWriterToMemory::Write(const void* ptr, const int num_bytes_to_write)
    -> int {
  // Make sure the storage is big enough.
  const int required_size = position_ + num_bytes_to_write;
  if (required_size > memory_buffer_.size()) {
    memory_buffer_.resize(required_size);
  }

  // Helper class to act as a generator for `std::generate()` which returns
  // bytes from the buffer to be written.
  class DataGenerator {
   public:
    DataGenerator(const void* ptr, const int size)
        : buffer_(static_cast<const uint8_t*>(ptr), size) {}

    auto operator()() -> uint8_t {
      assert(current_index_ < buffer_.size());
      return buffer_[current_index_++];
    }

   private:
    const std::span<const uint8_t> buffer_;
    int current_index_ = 0;
  };

  // Assign values in the container to the ones which are to be written.
  std::generate(memory_buffer_.begin() + position_,
                memory_buffer_.begin() + position_ + num_bytes_to_write,
                DataGenerator(ptr, num_bytes_to_write));

  // Advance the write head position.
  position_ += num_bytes_to_write;

  return num_bytes_to_write;
}

}  // namespace radio_core::testing
