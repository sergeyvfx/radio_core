// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Filter and downsample signal.
//
// The decimator downsamples the input signal by an integer ratio M.
// It consists of an anti-alias filter and a downsampler.
//
// The anti-alias filter is a low-pass filter which filters out frequencies
// above of the half of the output sample rate.
//
// The implementation follows the naive implementation with a distinct blocks
// for the filter and downsampler. The optimization is such that the filter is
// only applied at the every Mth input sample.
//
// TODO(sergey):
//
//   - Compare how the current implementation different from a more classical
//     polyphase subfilter approach.
//
//     The algorithmical complexity seems to be the same, but is the quality the
//     same?
//
//   - Verify the calculation of the kernel size for the anti-alias filter.

#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <vector>

#include "radio_core/base/ring_buffer.h"
#include "radio_core/math/kernel/dot.h"
#include "radio_core/signal/filter.h"
#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/frequency.h"
#include "radio_core/signal/window.h"

namespace radio_core::signal {

template <class SampleType,
          class KernelElementType = SampleType,
          template <class> class Allocator = std::allocator>
class Decimator {
 public:
  // Default constructor.
  //
  // Leaves object uninitialized. When this path is used an explicit call to
  // `setRatio()` is expected before performing downsapling, otherwise the
  // object will have an undefined behavior.
  inline Decimator() = default;

  // Construct decimator with pre-defined ratio.
  inline explicit Decimator(const int ratio) { SetRatio(ratio); }

  // Set decimation ratio.
  // If the current ratio is the same as the new one then nothing happens.
  void SetRatio(const int ratio) {
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

    kernel_.resize(kernel_size);
    stored_samples_.resize(kernel_.size());

    // Low-pass filter, rejecting frequencies above of half of the destination
    // sample rate. Additionally subtract the transition bandwidth to ensure a
    // good cut-off at the half of the destination sampling rate. Without this
    // some aliasing is still possible.
    //
    // TODO(sergey): Find a good synthetic test for the aliasing. For now it is
    // only being tested by offsetting radio from a strong local WFM station.
    // Namely with bad settings tuning to 91.33 MHz will show a phantom mirror
    // of 91.1 station.
    DesignLowPassFilter<KernelElementType>(
        kernel_,
        WindowEquation<RealType, Window::kBlackman>(),
        RealType(0.5) / ratio,
        RealType(1));

    // Reverse the kernel as the samples are stored in the reverse order.
    std::reverse(kernel_.begin(), kernel_.end());

    // Reset the downsampling accumulation.
    //
    // There might be more graceful reset to avoid possible spike in the output,
    // but with the current downsampling algorithm without reset lowering the
    // decimation ratio without such reset will lead to empty output for all
    // subsequent samples.
    num_unprocessed_samples_ = 0;
  }

  // Get currently configured decimation ratio.
  inline auto GetRatio() const -> int { return ratio_; }

  // Push and process new sample.
  //
  // This function will return a downsampled sample for every ratio-th input
  // sample. In all other cases nullopt is returned.
  auto operator()(const SampleType sample) -> std::optional<SampleType> {
    assert(ratio_ != 0);

    if (ratio_ == 1) {
      return sample;
    }

    stored_samples_.push_back(sample);
    ++num_unprocessed_samples_;

    if (num_unprocessed_samples_ != ratio_) {
      return std::nullopt;
    }

    num_unprocessed_samples_ = 0;

    return DotProductSamplesAndKernel();
  }

  // Downsample multiple input samples.
  //
  // The output buffer must have enough elements to hold result of the
  // downsampled samples. Use the `CalcNeededOutputBufferSize()` to calculate
  // the needed buffer size.
  //
  // It is possible to have the output buffer bigger than it is actually needed
  // in which case the output buffer will only be partially written (only number
  // of input samples will be written to the output).
  //
  // The input and output buffers might be the same. The algorithm always only
  // modifies the beginning of the output_samples buffer equal in size to the
  // number of written samples.
  //
  // Returns a subspan of the output samples buffer which was written by this
  // call.
  auto operator()(const std::span<const SampleType> input_samples,
                  const std::span<SampleType> output_samples)
      -> std::span<SampleType> {
    assert(ratio_ != 0);

    if (ratio_ == 1) {
      return HandleUnitRatio(input_samples, output_samples);
    }

    // The index of the first unprocessed input sample.
    size_t input_sample_index = 0;
    // The index of the next element in the output samples buffer to write the
    // result to.
    size_t output_sample_index = 0;

    // Process currently un-processed samples from the storage (the ones which
    // remained from the previous decimation processing) up to the point when
    // the decimation filter can fully operate on the input samples buffer.
    ProcessCombinedSamples(
        input_samples, output_samples, input_sample_index, output_sample_index);

    if (input_samples.size() - input_sample_index >= ratio_) {
      assert(num_unprocessed_samples_ == 0);

      // Push the latest processed samples to the ring buffer, preparing for the
      // next decimation.
      stored_samples_.push_back_multiple(
          input_samples.last(std::min(input_samples.size(), kernel_.size())));

      // Process input samples without copying them to a ring buffer to save on
      // memory transfers.
      ProcessContinuousSamples(input_samples,
                               output_samples,
                               input_sample_index,
                               output_sample_index);

    } else {
      stored_samples_.push_back_multiple(
          input_samples.last(input_samples.size() - input_sample_index));
    }

    num_unprocessed_samples_ = input_samples.size() - input_sample_index;

    return output_samples.subspan(0, output_sample_index);
  }

  inline auto operator()(const std::span<SampleType> samples)
      -> std::span<SampleType> {
    if (ratio_ == 1) {
      return samples;
    }

    return (*this)(samples, samples);
  }

  // Calculate required output buffer size for the given number of input
  // samples.
  //
  // The calculation takes care of the rounding, giving the smallest size of the
  // output buffer needed for downsampling input buffer size of the given size.
  // The calculation gives the worst case scenario, which means that the output
  // buffer size can only be calculated once if the downsampling happens for a
  // fixed input buffer size.
  inline auto CalcNeededOutputBufferSize(const size_t num_input_samples) const
      -> size_t {
    assert(ratio_ != 0);

    return (num_input_samples + ratio_ - 1) / ratio_;
  }

 private:
  // Special handler of the decimation ratio of 1, which copies input samples
  // to the output buffer and returns span od the output buffer of a proper
  // size.
  inline auto HandleUnitRatio(const std::span<const SampleType> input_samples,
                              const std::span<SampleType> output_samples)
      -> std::span<SampleType> {
    assert(ratio_ == 1);
    assert(output_samples.size() >= input_samples.size());

    std::copy(
        input_samples.begin(), input_samples.end(), output_samples.begin());

    return output_samples.subspan(0, input_samples.size());
  }

  // Process samples from both current ring buffer and the samples buffer.
  // Only the number of the new input samples is processed needed to give enough
  // head-room for in-place filtering done in the ProcessContinuousSamples().
  void ProcessCombinedSamples(const std::span<const SampleType> input_samples,
                              const std::span<SampleType> output_samples,
                              size_t& input_sample_index,
                              size_t& output_sample_index) {
    // The number of outputs processing of which will advance far enough in the
    // input samples buffer to use the continuous dot-product strategy.
    //
    // Note that the input and output buffer might be the same, so process
    // enough of the input samples to make enough head room past the written
    // sample in the buffer.
    const int max_num_output_samples = kernel_.size() / (ratio_ - 1) + 1;

    for (int i = 0; i < max_num_output_samples; ++i) {
      // The number of samples which needs to be pushed to the ring buffer
      // for the decimator filter.
      assert(num_unprocessed_samples_ < ratio_);

      const size_t num_remaining_input_samples =
          input_samples.size() - input_sample_index;
      const size_t num_samples_to_push = ratio_ - num_unprocessed_samples_;
      if (num_remaining_input_samples < num_samples_to_push) {
        break;
      }

      // Push samples to the ring buffer.
      stored_samples_.push_back_multiple(
          input_samples.subspan(input_sample_index, num_samples_to_push));

      output_samples[output_sample_index++] = DotProductSamplesAndKernel();

      // Update the state: there are now less input samples to process and
      // there are no unprocessed samples in the ring buffer.
      input_sample_index += num_samples_to_push;
      num_unprocessed_samples_ = 0;
    }
  }

  // Process input samples without copying them into a ring buffer for the
  // filtering.
  // The caller needs to ensure that there is enough headroom in the memory
  // prior to where the remaining_input_samples is currently pointing at.
  auto ProcessContinuousSamples(const std::span<const SampleType> input_samples,
                                const std::span<SampleType> output_samples,
                                size_t& input_sample_index,
                                size_t& output_sample_index) {
    const size_t kernel_size = kernel_.size();

    while (input_samples.size() - input_sample_index >= ratio_) {
      assert(input_sample_index + ratio_ >= kernel_size);

      const std::span<const SampleType> samples = input_samples.subspan(
          input_sample_index + ratio_ - kernel_size, kernel_size);

      // Assert the sub-span is within the input samples.
      assert(samples.begin() >= input_samples.begin());
      assert(samples.end() <= input_samples.end());

      output_samples[output_sample_index++] =
          Dot<SampleType, KernelElementType>(samples, kernel_);

      input_sample_index += ratio_;
    }
  }

  // Apply dot-product of the kernel and the current samples buffer.
  auto DotProductSamplesAndKernel() const -> SampleType {
    // Create explicit span of the kernel, to make creation of sub-spans easier.
    const std::span<const KernelElementType> kernel_span(kernel_);

    SampleType filtered_sample(0);

    // TODO(sergey): Use double-buffer technique (where sample is pushed twice
    // to a cyclic buffer with an offset of size of the kernel). This will allow
    // to have a single Dot() kernel invocation.

    const std::span<const SampleType> old_samples =
        stored_samples_.GetContinuousOldElements();
    const size_t num_old_samples = old_samples.size();
    filtered_sample +=
        Dot(old_samples, kernel_span.subspan(0, num_old_samples));

    const std::span<const SampleType> new_samples =
        stored_samples_.GetContinuousNewElements();
    filtered_sample += Dot(new_samples, kernel_span.subspan(num_old_samples));

    return filtered_sample;
  }

  // Decimation ratio.
  int ratio_ = 0;

  // Kernel of the low-pass filter.
  std::vector<KernelElementType, Allocator<KernelElementType>> kernel_;

  // Ring buffer with latest input samples of a size which matches the kernel
  // size.
  RingBuffer<SampleType, Allocator<SampleType>> stored_samples_;

  // The number of unprocessed in the stored_samples_.
  size_t num_unprocessed_samples_ = 0;
};

}  // namespace radio_core::signal
