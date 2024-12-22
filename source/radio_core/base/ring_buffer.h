// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// A ring buffer implementation of variable size.
//
// This buffer is a FIFO queue with the oldest element at the `front()` and the
// newest element at the `back()`. Pushing new element to the back of the buffer
// "pops" the oldest element from the front.
//
// The buffer is optimized for sequential access of elements from oldest to the
// newest. This is done by traversing two spans:
//  - GetContinuousOldElements()
//  - GetContinuousNewElements()
//
// A random access is supported, but it is not cheap: the requested index needs
// to be mapped to an index within an internal storage array.
//
// The buffer uses the amount of memory that is needed to store the given number
// of elements.

#pragma once

#include <cassert>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "radio_core/base/exception.h"

namespace radio_core {

template <class T, class Allocator = std::allocator<T>>
class RingBuffer {
  template <bool kIsConst>
  class Iterator {
    using Container =
        std::conditional_t<kIsConst, const RingBuffer, RingBuffer>;

   public:
    using iterator_category = std::forward_iterator_tag;

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = std::conditional_t<kIsConst, const T, T>;
    using reference = value_type&;
    using pointer = value_type*;

    Iterator() = default;

    Iterator(Container* container, const size_type index)
        : container_(container), index_(index) {}

    Iterator(const Iterator& other) = default;
    Iterator(Iterator&& other) noexcept = default;

    auto operator=(const Iterator& other) -> Iterator& = default;
    auto operator=(Iterator&& other) noexcept -> Iterator& = default;

    inline auto operator<=>(const Iterator& other) const = default;

    // Postfix.
    inline auto operator++(int) -> Iterator {
      Iterator current_iterator = *this;
      ++(*this);
      return current_iterator;
    }

    // Prefix.
    inline auto operator++() -> Iterator& {
      ++index_;
      return *this;
    }

    inline auto operator*() const -> reference { return (*container_)[index_]; }
    inline auto operator->() const -> pointer { return &(*container_)[index_]; }

   private:
    Container* container_{nullptr};
    size_type index_{0};
  };

 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  RingBuffer() = default;

  // Initialize FIFO buffer with the given number of elements.
  // The elements are default-initialized.
  explicit RingBuffer(const size_type count) { resize(count); }

  //////////////////////////////////////////////////////////////////////////////
  // Element access.

  // Returns a reference to the element at specified location pos.
  //
  // Element with index 0 is the oldest one, the element with index size()-1 is
  // the newest.
  //
  // No bounds checking is performed.
  inline auto operator[](const size_type index) -> T& {
    const RingBuffer* const_this = this;
    return const_cast<T&>((*const_this)[index]);
  }
  inline auto operator[](const size_type index) const -> const T& {
    size_type data_index = head_ + index;
    if (data_index >= size()) {
      data_index -= size();
    }

    assert(data_index < size());
    return data_[data_index];
  }

  // Returns a reference to the element at specified location pos, with bounds
  // checking.
  //
  // If index is not within the range of the buffer, an exception of type
  // std::out_of_range is thrown.
  inline auto at(const size_type index) -> T& {
    const RingBuffer* const_this = this;
    return const_cast<T&>((const_this->at(index)));
  }
  inline auto at(const size_type index) const -> const T& {
    if (index >= size()) {
      ThrowOrAbort<std::out_of_range>("RingBuffer[] index out of bounds");
    }
    return (*this)[index];
  }

  // Access the front (the oldest) element of the buffer.
  //
  // Calling front on an empty buffer is undefined.
  inline auto front() -> T& {
    assert(!empty());
    return (*this)[0];
  }
  inline auto front() const -> const T& {
    assert(!empty());
    return (*this)[0];
  }

  // Access the back (the newest) element of the buffer.
  //
  // Calling back on an empty buffer is undefined.
  inline auto back() -> T& {
    assert(!empty());
    return (*this)[size() - 1];
  }
  inline auto back() const -> const T& {
    assert(!empty());
    return (*this)[size() - 1];
  }

  // Get continuous memory span which contains old elements (starting from
  // front() and up to the end of the internal storage).
  inline auto GetContinuousOldElements() const -> std::span<const T> {
    assert(!empty());
    return std::span(data_.data() + head_, size() - head_);
  }

  // Get continuous memory span which contains new elements (starting from
  // the beginning of the internal storage ending with back()).
  inline auto GetContinuousNewElements() const -> std::span<const T> {
    assert(!empty());
    return std::span(data_.data(), head_);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Modification.

  // Push the given element value to the end of the buffer.
  inline void push_back(const T& element) {
    AdvanceWarped(head_);
    AdvanceWarped(tail_);

    assert(tail_ < size());
    data_[tail_] = element;
  }

  // Push multiple elements at the back of the ring buffer.
  // This is a more optimal implementation of doing push_back() in a loop.
  inline void push_back_multiple(const std::span<const T> elements) {
    const size_type buffer_size = size();

    // Clamped span of the elements to be pushed.
    // Use the last buffer_size elements as the leading ones will overwritten.
    std::span<const T> elements_to_push =
        elements.size() > buffer_size ? elements.last(buffer_size) : elements;

    // The actual number of the new elements to be pushed.
    const size_type num_new_elements = elements_to_push.size();

    // Copy the head of the new elements to the end of the buffer storage.
    const size_type head_size =
        std::min(buffer_size - tail_ - 1, num_new_elements);
    if (head_size != 0) {
      std::copy(elements_to_push.begin(),
                elements_to_push.begin() + head_size,
                data_.begin() + tail_ + 1);
      elements_to_push = elements_to_push.subspan(head_size);
    }

    // Copy remaining elements to the beginning of the storage.
    if (!elements_to_push.empty()) {
      std::copy(
          elements_to_push.begin(), elements_to_push.end(), data_.begin());
    }

    AdvanceWarped(head_, num_new_elements);
    AdvanceWarped(tail_, num_new_elements);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Capacity.

  // resize the buffer to contain count elements.
  //
  // Resizing the buffer after any element has been added to it leads to an
  // undefined state of element content.
  //
  // New elements are default-initialized.
  inline void resize(const size_type count) {
    data_.resize(count, T());

    head_ = 0;

    if (!empty()) {
      tail_ = size() - 1;
    } else {
      tail_ = 0;
    }
  }

  // Returns the current number of elements.
  inline auto size() const -> size_type { return data_.size(); }

  // Check whether the container is empty.
  inline auto empty() const -> bool { return size() == 0; }

  //////////////////////////////////////////////////////////////////////////////
  // Iterators.

  inline auto begin() -> iterator { return iterator(this, 0); }
  inline auto begin() const -> const_iterator {
    return const_iterator(this, 0);
  }

  inline auto end() -> iterator { return iterator(this, size()); }
  inline auto end() const -> const_iterator {
    return const_iterator(this, size());
  }

 private:
  // Increment the given index by one, warping past the buffer size on overflow.
  inline auto AdvanceWarped(size_type& index,
                            const size_type advance = 1) const {
    assert(advance <= size());
    index += advance;
    if (index >= size()) {
      index -= size();
    }
  }

  std::vector<T, Allocator> data_;

  size_type head_{0};
  size_type tail_{0};
};

}  // namespace radio_core
