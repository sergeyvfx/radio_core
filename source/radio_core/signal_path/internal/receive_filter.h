// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Radio receive filter.
//
// The intended application is to use it to filter out signal at the
// intermediate frequency stage. It operates on a complex samples, and applies
// a low-pass filter which cut-off frequency equal to half of the receive
// filter bandwidth.
//
// The filter implements a down-fir-up algorithm for cases when the signal
// sampling rate is much higher than the filter bandwidth.

#pragma once

#include <memory>
#include <span>
#include <vector>

#include "radio_core/base/algorithm.h"
#include "radio_core/base/container.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/decimator.h"
#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/filter_window_heuristic.h"
#include "radio_core/signal/interpolator.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::signal_path::internal {

template <class T, template <class> class Allocator = std::allocator>
class ReceiveFilter {
 public:
  struct Options {
    // Sample rate of signal this filter operates on.
    T sample_rate{0};

    // Bandwidth of the filter, Hertz.
    // The signal around DC of this bandwidth is passed through.
    T bandwidth{0};

    // Transition band, Hertz.
    // Defines transition with measured in hertz between a passband and a
    // stopband.
    T transition_band{0};

    constexpr auto operator<=>(const Options& other) const = default;
  };

  // Configure the filter.
  // If the requested configuration matches the requested one nothing happens.
  void Configure(const Options& options) {
    if (configured_options_ == options) {
      return;
    }

    decimation_ratio_ = GetDecimationRatio(options);

    decimator_.SetRatio(decimation_ratio_);
    interpolator_.SetRatio(decimation_ratio_);

    const T filter_sample_rate = options.sample_rate / decimation_ratio_;

    const size_t kernel_size =
        signal::EstimateFilterSizeForTransitionBandwidth<T>(
            options.transition_band, filter_sample_rate);

    filter_.SetKernelSize(kernel_size);

    // The cutoff frequency is the half of the receive filter bandwidth because
    // the band is centered around the DC.
    //
    // Clamp the frequency to the IF sample rate, so that there are no
    // mathematical issues and the filter gives usable results under the extreme
    // configuration.
    const T clamped_cutoff_frequency =
        Min<T>(options.bandwidth / 2, filter_sample_rate / 2);

    DesignLowPassFilter<T>(
        filter_.GetKernel(),
        signal::WindowEquation<T, signal::Window::kHamming>(),
        clamped_cutoff_frequency,
        filter_sample_rate);

    // Store the actual filter configuration.
    filter_bandwidth_ = clamped_cutoff_frequency * 2;
    filter_transition_band_ = options.transition_band;
  }

  // Filter the given input samples.
  //
  // The output samples must have at least the same number of elements as the
  // input samples. It is possible to pass a buffer of a bigger size. It is also
  // possible to do in-place processing.
  //
  // Returns the span of actually written samples in the output.
  auto operator()(const std::span<const BaseComplex<T>> input_samples,
                  const std::span<BaseComplex<T>> output_samples)
      -> std::span<BaseComplex<T>> {
    if (decimation_ratio_ == 1) {
      return filter_(input_samples, output_samples);
    }

    // Make sure the buffer is large enough.
    EnsureSizeAtLeast(
        downsample_buffer_,
        decimator_.CalcNeededOutputBufferSize(input_samples.size()));

    const std::span<BaseComplex<T>> downsampled_signal =
        decimator_(input_samples, downsample_buffer_);

    filter_(downsampled_signal);

    return interpolator_(downsampled_signal, output_samples);
  }

  // Filter the signal in-place.
  //
  // NOTE: It is possible that the buffer is not fully filled. This happens when
  // the filter uses decimation to speed up processing.
  //
  // Returns the span of actually written samples in the output.
  auto operator()(const std::span<BaseComplex<T>> signal)
      -> std::span<BaseComplex<T>> {
    return (*this)(signal, signal);
  }

  // Calculate required output buffer size for the given number of input
  // samples.
  inline auto CalcNeededOutputBufferSize(const size_t num_input_samples) const {
    if (decimation_ratio_ == 1) {
      return num_input_samples;
    }

    const size_t decimated_size =
        decimator_.CalcNeededOutputBufferSize(num_input_samples);
    return interpolator_.CalcNeededOutputBufferSize(decimated_size);
  }

  // Get actual filter configuration.
  auto GetDecimationRatio() -> int { return decimation_ratio_; }
  auto GetBandwidth() -> T { return filter_bandwidth_; }
  auto GetTransitionBand() -> T { return filter_transition_band_; }
  auto GetKernelSize() -> size_t { return filter_.GetKernelSize(); }

 private:
  // Get the decimation ratio which is applied prior to the filter.
  // The same ratio is used for interpolation after the filter.
  auto GetDecimationRatio(const Options& options) const -> int {
    const T filter_cutoff = options.bandwidth / 2;

    // Minimum sample rate for the good performance of the filter and the
    // radio. Give some extra margin above the Nyquist frequency.
    const T min_sample_rate = filter_cutoff * 4;

    if (options.sample_rate <= min_sample_rate) {
      return 1;
    }

    const int ratio = RoundToInt(options.sample_rate / min_sample_rate);

    // Limit the ratio to avoid possible artifacts with too high ratio during
    // down or up sampling.
    return Min(25, Max(ratio, 1));
  }

  // Options the filter is configured for.
  // This is the requested configuration.
  Options configured_options_;

  // The actual bandwidth and transition band of the filter.
  // It might be different from the requested one due to clamping.
  T filter_bandwidth_{0};
  T filter_transition_band_{0};

  signal::SimpleFIRFilter<BaseComplex<T>, T, Allocator> filter_;

  int decimation_ratio_{1};

  signal::Decimator<BaseComplex<T>, T, Allocator> decimator_;
  signal::Interpolator<BaseComplex<T>, T, Allocator> interpolator_;

  // Buffer used to hold downsampled input signal.
  std::vector<BaseComplex<T>, Allocator<BaseComplex<T>>> downsample_buffer_;
};

}  // namespace radio_core::signal_path::internal
