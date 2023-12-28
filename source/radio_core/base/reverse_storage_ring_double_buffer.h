// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Ring buffer implementation which stores elements starting from the end
// of the underlying memory buffer. This allows to get access to a continuous
// memory block with elements stored in the newer-to-older order.
//
// The buffer has a specified length and elements are circularly overwritten.
//
// Internally the samples are stored twice with an offset of the buffer size
// between them. This allows to have access to a single continuous chunk of
// memory for the current state of the buffer.
//
// Typical use-case of such storage is FIR filter implementation where samples
// needs to be multiplied by kernel in a reverse order (from kernel and samples
// array indices point of view).

#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <span>
#include <vector>

namespace radio_core {

template <class T, class Allocator = std::allocator<T>>
class ReverseStorageRingDoubleBuffer {
 public:
  // Default constructor.
  //
  // An explicit call to `resize()` is expected, otherwise pushing elements
  // will have undefined behavior and potentially memory corruption.
  ReverseStorageRingDoubleBuffer() = default;

  explicit ReverseStorageRingDoubleBuffer(const size_t size) { resize(size); }

  // Resize the buffer to the new size (new number of elements).
  // This will reset the content to zeros.
  inline void resize(const size_t size) {
    size_ = size;
    tail_index_ = size - 1;

    data_.resize(size * 2);

    std::fill(data_.begin(), data_.end(), T(0));
  }

  // Get current size of the buffer.
  // NOTE: It is the buffer size, not the number of elements.
  inline auto size() const -> size_t { return size_; }

  inline auto empty() const -> bool { return size() == 0; }

  // Push new element to the buffer.
  //
  // Calling in on an empty buffer leads to an undefined behavior.
  inline void push(const T& element) {
    assert(!empty());

    data_[tail_index_] = element;
    data_[tail_index_ + size_] = element;

    if (tail_index_ > 0) {
      --tail_index_;
    } else {
      tail_index_ = size() - 1;
    }
  }

  // Push a sequence of multiple elements to the buffer.
  // Similar to sequentially pushing elements individually, but this way leads
  // to less computations.
  //
  // Calling in on an empty buffer leads to an undefined behavior.
  //
  // Follow the naming convention of push().
  inline void push_multiple(const std::span<const T> elements) {
    assert(!empty());

    // Clamp number of elements to at max size().
    // This mimics the behavior when extra elements are pushed out of the
    // buffer.
    const std::span<const T> elements_to_push =
        elements.size() > size_ ? elements.last(size_) : elements;

    for (const T& element : elements_to_push) {
      push(element);
    }
  }

  // Access element with a given index.
  // The index of 0 points to the most recent element, the index of size()-1
  // points to the lest recent element.
  //
  // Calling in on an empty buffer leads to an undefined behavior.
  inline auto operator[](const size_t index) -> T& {
    const ReverseStorageRingDoubleBuffer* const_this = this;
    return const_cast<T&>((*const_this)[index]);
  }
  inline auto operator[](const size_t index) const -> const T& {
    assert(!empty());
    return data_[tail_index_ + 1 + index];
  }

  // Get span which contains the size() elements of the buffer.
  inline auto GetElements() const -> std::span<const T> {
    assert(!empty());
    return std::span(data_.data() + tail_index_ + 1, size());
  }

 private:
  std::vector<T, Allocator> data_;

  // Number of elements in this buffer.
  size_t size_ = 0;

  // Index at which next sample will be written.
  size_t tail_index_ = 0;
};

}  // namespace radio_core
