// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Simple (to use) FIR filter.
//
// This is a subclass of FIRFilter which stores kernel inside of object.
// This simplifies construction and usage of filters when kernel re-usability is
// not needed.

#pragma once

#include <memory>
#include <vector>

#include "radio_core/signal/fir_filter.h"

namespace radio_core::signal {

template <class SampleType,
          class KernelElementType,
          template <class> class Allocator = std::allocator>
class SimpleFIRFilter : public FIRFilter<SampleType, KernelElementType> {
 public:
  using BaseClass = FIRFilter<SampleType, KernelElementType>;

  using BaseClass::BaseClass;

  // Construct filter with given kernel values.
  //
  // The values are copied into the storage inside of this filter, so that the
  // caller can free kernel on its side.
  explicit SimpleFIRFilter(const std::span<const KernelElementType> kernel)
      : BaseClass(kernel_), kernel_(kernel) {}

  // Resize the filter to the new kernel size.
  //
  // NOTE: The current samples storage is reset to zeroes.
  inline void SetKernelSize(const size_t size) {
    if (BaseClass::GetKernelSize() == size) {
      return;
    }

    kernel_.resize(size);
    BaseClass::SetKernel(kernel_);
  }

  // NOTE: The filter is resized to the new kernel size and kernel is copied to
  // this filter so that caller can dispose kernel from its side.
  //
  // NOTE: The current samples storage is reset to zeroes.
  inline void SetKernel(std::span<const KernelElementType> kernel) {
    SetKernelSize(kernel.size());

    std::copy(kernel.begin(), kernel.end(), kernel_.begin());
  }

  using BaseClass::GetKernel;

  auto GetKernel() -> std::span<KernelElementType> {
    const SimpleFIRFilter* const_this = this;
    const std::span<const KernelElementType> const_kernel =
        const_this->GetKernel();
    return std::span(kernel_.data(), const_kernel.size());
  }

 private:
  std::vector<KernelElementType, Allocator<KernelElementType>> kernel_;
};

}  // namespace radio_core::signal
