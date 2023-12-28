// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// FIR filter which applies filter kernel in an input stream of samples.
// The filter stores its internal state needed do deal with a continuous stream
// of new samples.

#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <span>
#include <vector>

#include "radio_core/base/reverse_storage_ring_buffer.h"
#include "radio_core/base/verify.h"
#include "radio_core/math/kernel/dot.h"
#include "radio_core/math/kernel/dot_flip.h"

namespace radio_core::signal {

template <class SampleType,
          class KernelElementType,
          template <class> class Allocator = std::allocator>
class FIRFilter {
 public:
  // Default constructor which leaves the filter uninitialized.
  // A kernel is to be provided using `setKernel()` before samples can be
  // processed.
  FIRFilter() = default;

  explicit FIRFilter(std::span<const KernelElementType> kernel) {
    SetKernel(kernel);
  }

  // NOTE: The kernel is referenced by this filter.
  // NOTE: The current samples storage is reset to zeroes.
  inline void SetKernel(std::span<const KernelElementType> kernel) {
    kernel_ = kernel;

    stored_samples_.resize(GetKernelSize());
  }

  inline auto GetKernel() const -> std::span<const KernelElementType> {
    return kernel_;
  }

  inline auto GetKernelSize() const -> size_t { return kernel_.size(); }

  // NOTE: If the kernel was not yet provided and initialized this function will
  // have an undefined behavior.
  auto operator()(const SampleType sample) -> SampleType {
    stored_samples_.push(sample);

    SampleType output(0);

    // TODO(sergey): Use double-buffer technique (where sample is pushed twice
    // to a cyclic buffer with an offset of size of the kernel). This will allow
    // to have a single Dot() kernel invocation.
    //
    // However, from quick experiments seems that it does not yeld noticeable
    // performance improvement on Apple M2 Max and Raspberry Pi 4.

    const std::span<const SampleType> new_samples =
        stored_samples_.GetContinuousNewElements();
    const size_t num_new_samples = new_samples.size();
    if (num_new_samples) {
      output += Dot(new_samples, kernel_.subspan(0, num_new_samples));
    }

    const std::span<const SampleType> old_samples =
        stored_samples_.GetContinuousOldElements();
    if (!old_samples.empty()) {
      output += Dot(old_samples, kernel_.subspan(num_new_samples));
    }

    return output;
  }

  // Filter multiple input samples.
  //
  // The input and output buffers are allowed to be the same.
  // Aliasing of the input and output buffers might lead to an undefined
  // behavior.
  //
  // Returns subspan of output where samples were actually written.
  //
  // The caller must ensure the output samples buffer is big enough should have
  // at least the size of the input samples).
  //
  // NOTE: If the kernel was not provided and initialized this function will
  // have an undefined behavior.
  auto operator()(const std::span<const SampleType> input_samples,
                  const std::span<SampleType> output_samples)
      -> std::span<SampleType> {
    assert(input_samples.size() <= output_samples.size());

    const size_t kernel_size = kernel_.size();
    const size_t num_input_samples = input_samples.size();

    // If the number of input samples is not large enough use the naive
    // implementation, as it overall provides less memory transfers.
    if (num_input_samples < kernel_size * 4) {
      for (size_t i = 0; i < num_input_samples; ++i) {
        output_samples[i] = (*this)(input_samples[i]);
      }

      return output_samples.subspan(0, num_input_samples);
    }

    const bool is_aliased = input_samples.data() == output_samples.data();

    size_t i = 0;

    // Process the first samples into potentially temporary buffer.
    // This is needed because the input and output buffers can be the same, and
    // processing of samples after this loop need the current samples intact.
    SampleType* prefix_buffer = nullptr;
    if (is_aliased) {
      temp_buffer_.resize(kernel_size + 1);
      prefix_buffer = temp_buffer_.data();
    } else {
      prefix_buffer = output_samples.data();
    }
    while (i <= kernel_size) {
      prefix_buffer[i] = (*this)(input_samples[i]);
      ++i;
    }

    if (i < num_input_samples) {
      // Push the last samples to the state machine.
      stored_samples_.push_multiple(input_samples.last(num_input_samples - i));

      size_t k = num_input_samples - 1;
      while (k >= i) {
        assert(k >= kernel_size);

        output_samples[k] = experimental::DotFlipG(
            input_samples.subspan(k - kernel_size + 1, kernel_size), kernel_);

        --k;
      }

      if (is_aliased) {
        std::copy(
            temp_buffer_.begin(), temp_buffer_.end(), output_samples.begin());
      }
    }

    return output_samples.subspan(0, num_input_samples);
  }

  // In-place filter samples.
  inline void operator()(const std::span<SampleType> samples) {
    (*this)(samples, samples);
  }

 private:
  std::span<const KernelElementType> kernel_;

  // Buffer to store temporary result to avoid conflict caused by the input
  // and output aliasing.
  // This buffer is used when processing a large number of input samples when
  // it is possible to filter samples without copying all of them to the ring
  // buffer.
  std::vector<SampleType> temp_buffer_;

  // THe last kernel_.size() number of samples.
  ReverseStorageRingBuffer<SampleType, Allocator<SampleType>> stored_samples_;
};

}  // namespace radio_core::signal
