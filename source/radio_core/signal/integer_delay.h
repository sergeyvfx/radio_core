// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Delay processor which returns processed samples with a given delay in
// integer number of samples.
//
// The initial content of the buffer is all zeroes. This means that processing
// samples until the delay duration is reached will return 0.

#pragma once

#include <cassert>

#include "radio_core/base/ring_buffer.h"

namespace radio_core::signal {

template <class T, class Allocator = std::allocator<T>>
class IntegerDelay {
 public:
  using value_type = T;
  using size_type = size_t;

  IntegerDelay() = default;

  explicit IntegerDelay(const size_type delay) { SetDelay(delay); }

  // Push new sample to the processor, return the sample which was processed
  // configured delay ago.
  //
  // Processing samples when the delay is 0 is undefined,
  inline auto operator()(T sample) -> T {
    assert(GetDelay() != 0);

    const T oldest_sample = ring_buffer_.front();

    ring_buffer_.push_back(sample);

    return oldest_sample;
  }

  // Set new delay in number of samples.
  //
  // Setting delay after processing has begin is undefined.
  inline void SetDelay(const size_type delay) { ring_buffer_.resize(delay); }

  // Get current effective delay in number of samples.
  inline auto GetDelay() const -> size_t { return ring_buffer_.size() - 1; }

 private:
  RingBuffer<T, Allocator> ring_buffer_;
};

}  // namespace radio_core::signal
