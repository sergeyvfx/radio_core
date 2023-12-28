// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Upsample and filter the signal.
//
// The interpolator upsamples the input signal by an integer ratio M.
// It consists of an upsampler and anti-alias filter.
//
// The anti-alias filter is a low-pass filter which filters out frequencies
// above of the half of the input sample rate.
//
// The implementation follows polyphase form of the interpolator to save up on
// the number of multiplications [1].
//
// References:
//
//   [1] Orfanidis, Sophocles J. Introduction to Signal Processing.
//       Upper Saddle River, NJ: Prentice-Hall, 1996.

#pragma once

#include <algorithm>
#include <cassert>

#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/polyphase_filter.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::signal {

template <class SampleType,
          class KernelElementType = SampleType,
          template <class> class Allocator = std::allocator>
class Interpolator {
  template <class T>
  using Vector = std::vector<T, Allocator<T>>;

  using Kernel = Vector<KernelElementType>;
  using Filter = SimpleFIRFilter<SampleType, KernelElementType, Allocator>;

 public:
  // Default constructor.
  //
  // Leaves object uninitialized. When this path is used an explicit call to
  // `setRatio()` is expected before performing interpolation, otherwise the
  // object will have an undefined behavior.
  inline Interpolator() = default;

  // Construct interpolator with pre-defined ratio.
  inline explicit Interpolator(const int ratio) { SetRatio(ratio); }

  // Set interpolation ratio.
  // If the current ratio is the same as the new one then nothing happens.
  inline void SetRatio(const int ratio) {
    // Pseudonym for real-typed scalar values. Depending on the kernel this is
    // typically either float or double.
    //
    // TODO(sergey): Not really correct: the either or both of the sample type
    // and the kernel elements can be complex, and here it is required to have
    // a real type.
    using RealType = KernelElementType;

    assert(ratio > 0);

    if (ratio_ == ratio) {
      // Avoid re-initialization if the ratio did not change.
      return;
    }

    ratio_ = ratio;

    // This follows calculation of the FIR kernel size used in scipy.decimate()
    // which is the 20 times the ratio (rounded to an odd number). The same
    // factor is also used in scipy.resample_poly().
    //
    // TODO(sergey): Consider making it configurable to help applications on a
    // low performance hardware. Seems that for the radio applications a quarter
    // of this gives good results.
    const int kernel_size = 20 * ratio + 1;

    // Allocate memory for the components.
    //
    // NOTE: Do it before the filter kernel calculation as this way the
    // temporary allocation has the least amount of side effect on possible
    // fragmentation.
    AllocatePolyphaseFilters(kernel_size);

    Kernel kernel(kernel_size);

    DesignLowPassFilter<KernelElementType>(
        kernel,
        WindowEquation<RealType, Window::kBlackman>(),
        RealType(0.5) / ratio,
        RealType(1));

    // Decompose the filter into components.
    for (int l = 0; l < ratio; ++l) {
      PolyphaseComponentDecomposition<KernelElementType>(
          kernel, ratio, l, polyphase_filters_[l].GetKernel());
    }
  }

  // Get currently configured interpolation ratio.
  inline auto GetRatio() const -> int { return ratio_; }

  // Interpolate multiple input samples.
  //
  // The output buffer must have enough elements to hold result of the
  // interpolated samples. Use the `CalcNeededOutputBufferSize()` to calculate
  // the needed buffer size.
  //
  // The input and the output buffers must not alias or overlap.
  // This is not verified and if such situation happens it leads to an
  // undefined behavior.
  //
  // Returns a subspan of the output samples buffer which was written by this
  // call.
  inline auto operator()(const std::span<const SampleType> input_samples,
                         const std::span<SampleType> output_samples)
      -> std::span<SampleType> {
    assert(GetRatio() != 0);

    const int ratio = GetRatio();

    if (ratio == 1) {
      assert(output_samples.size() >= input_samples.size());
      std::copy(
          input_samples.begin(), input_samples.end(), output_samples.begin());
      return output_samples.subspan(0, input_samples.size());
    }

    size_t num_output_samples = 0;

    for (const SampleType& input_sample : input_samples) {
      for (int m = 0; m < ratio; ++m) {
        assert(num_output_samples < output_samples.size());

        output_samples[num_output_samples++] =
            polyphase_filters_[m](input_sample) * ratio;
      }
    }

    return output_samples.subspan(0, num_output_samples);
  }

  // Calculate required output buffer size for the given number of input
  // samples.
  inline auto CalcNeededOutputBufferSize(const size_t num_input_samples) const
      -> size_t {
    assert(GetRatio() != 0);

    return num_input_samples * GetRatio() + 1;
  }

 private:
  // Allocate memory for the polyphase component filters.
  //
  // This includes memory to store the components, as well as the per-component
  // memory for its coefficients.
  inline void AllocatePolyphaseFilters(const size_t filter_kernel_size) {
    const int ratio = GetRatio();

    // Allocate polyphase components.
    polyphase_filters_.resize(ratio);

    // Allocate elements of the polyphase components.
    const size_t polyphase_component_size =
        PolyphaseDecompositionSize(filter_kernel_size, ratio);
    for (Filter& polyphase_filter : polyphase_filters_) {
      polyphase_filter.SetKernelSize(polyphase_component_size);
    }
  }

  // Interpolation ratio.
  int ratio_ = 0;

  Vector<Filter> polyphase_filters_;
};

}  // namespace radio_core::signal
