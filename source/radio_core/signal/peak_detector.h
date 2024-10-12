// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Peak detector using different strategies.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/math.h"

namespace radio_core::signal {

// Detect peaks using linear interpolation between current state of the peak
// detector the new samples.
//
// The weight of interpolation is different for cases when new value is higher
// and lower than the detected peak. This allows to cover situations when a new
// peak is to be detected quickly, but be slowly decaying to make it easier to
// see in the interface.
template <class T>
class LerpPeakDetector {
 public:
  explicit LerpPeakDetector() = default;

  explicit LerpPeakDetector(const T charge_rate, const T discharge_rate) {
    Configure(charge_rate, discharge_rate);
  }

  inline void Configure(const T charge_rate, const T discharge_rate) {
    charge_rate_ = charge_rate;
    discharge_rate_ = discharge_rate;
  }

  // Process the given sample and return an updated peak.
  inline auto operator()(const T x) -> T {
    if (x > current_peak_) {
      current_peak_ = Lerp(current_peak_, x, charge_rate_);
    } else {
      current_peak_ = Lerp(current_peak_, x, discharge_rate_);
    }

    return current_peak_;
  }

  // Detect peak of multiple input samples, writing the result to the
  // corresponding element of the output buffer:
  //
  //   output_samples = [ peak_detector(x) for x in input_samples ]
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
  inline auto operator()(const std::span<const T> input_samples,
                         const std::span<T> output_samples) -> std::span<T> {
    assert(input_samples.size() <= output_samples.size());

    const size_t num_input_samples = input_samples.size();
    for (size_t i = 0; i < num_input_samples; ++i) {
      output_samples[i] = (*this)(input_samples[i]);
    }

    return output_samples.subspan(0, num_input_samples);
  }

  // In-place peak detector samples.
  // Each sample is replaced with peak calculated it:
  //
  //   samples = [ peak_detector(x) for x in samples ]
  inline void operator()(const std::span<T> samples) {
    (*this)(samples, samples);
  }

  // Reset the peak value to the given value.
  inline void ResetPeak(const T value) { current_peak_ = value; }

 private:
  T charge_rate_ = 0;
  T discharge_rate_ = 0;

  T current_peak_ = 0;
};

}  // namespace radio_core::signal
