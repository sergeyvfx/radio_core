// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Automatic gain control.
// Adaptively scales the signal to convert to make be within [-1 .. 1] range.
//
// The idea behind the implementation is to mimic to how a charging capacitor
// via a rectifier will behave: fast exponential charge and slower exponential
// discharge. The charge is used an an inverse of the normalization factor.
//
// Reference:
//   https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/math.h"

namespace radio_core::signal {

template <class SampleType, class RealType = SampleType>
class EMAAGC {
 public:
  EMAAGC() = default;

  // Charge rate is effectively an inertia of the filter in a situation when
  // input signal is quickly rising in amplitude. Lower the inertia lower the
  // overshoot of the output when input signal gains amplitude very quickly.
  //
  // Discharge rate is effectively an inertia of the filter in a situation
  // when input signal fades out. Higher the inertia is longer the output signal
  // will be preserved at the normalized range.
  EMAAGC(const RealType charge_rate, const RealType discharge_rate)
      : charge_rate_(charge_rate), discharge_rate_(discharge_rate) {}

  inline void Configure(const RealType charge_rate,
                        const RealType discharge_rate) {
    charge_rate_ = charge_rate;
    discharge_rate_ = discharge_rate;
  }

  // Process single sample.
  // Returns normalized sample value.
  inline auto operator()(const SampleType sample) -> SampleType {
    const SampleType abs_sample = Abs(sample);

    if (abs_sample > current_charge_) {
      current_charge_ = Lerp(current_charge_, abs_sample, charge_rate_);
    } else {
      current_charge_ = Lerp(current_charge_, abs_sample, discharge_rate_);
    }

    // Avoid division by zero.
    if (current_charge_ == 0) {
      return 0;
    }

    return sample / current_charge_;
  }

  // Apply AGC on multiple input samples, writing the result to the
  // corresponding element of the output buffer:
  //
  //   output_samples = [ agc(x) for x in input_samples ]
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
  inline auto operator()(const std::span<const SampleType> input_samples,
                         const std::span<SampleType> output_samples)
      -> std::span<SampleType> {
    assert(input_samples.size() <= output_samples.size());

    const size_t num_input_samples = input_samples.size();
    for (size_t i = 0; i < num_input_samples; ++i) {
      output_samples[i] = (*this)(input_samples[i]);
    }

    return output_samples.subspan(0, num_input_samples);
  }

  // In-place AGC.
  // Each sample is replaced with the AGC-normalized value:
  //
  //   samples = [ agc(x) for x in samples ]
  inline void operator()(const std::span<SampleType> samples) {
    (*this)(samples, samples);
  }

  // Reset the AGC to the initial state.
  inline void Reset() { current_charge_ = 0; }

 private:
  RealType charge_rate_ = 0;
  RealType discharge_rate_ = 0;

  SampleType current_charge_ = 0;
};

}  // namespace radio_core::signal
