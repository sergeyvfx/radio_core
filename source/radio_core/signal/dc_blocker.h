// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Filter which removes DC component from a signal.
//
// The parameter R affects on the averaging window (which can be approximated
// with `num_samples = 1 / (1 - R)`). Smaller R values allows faster tracking
// but in the cost of higher low frequency attenuation.
//
// The implementation follows:
//
//   https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
//   https://groups.google.com/g/digitalvoice/c/RakFvpVWR2w/m/hQSWn7MO-2gJ

#pragma once

#include <cassert>
#include <span>

namespace radio_core::signal {

template <class SampleType, class RealType = SampleType>
class DCBlocker {
 public:
  explicit DCBlocker(const RealType r) : r_(r) {}

  inline auto operator()(const SampleType& x) -> SampleType {
    const SampleType y = x - xml_ + r_ * yml_;
    xml_ = x;
    yml_ = y;
    return y;
  }

  // Apply DC block on multiple input samples, writing the result to the
  // corresponding element of the output buffer:
  //
  //   output_samples = [ dc_block(x) for x in input_samples ]
  //
  // The input and output buffers are allowed to be the same.
  // Aliasing of the input and output buffers might lead to an undefined
  // behavior.
  //
  // Returns subspan of output where samples were actually written.
  //
  // The caller must ensure the output samples buffer is big enough should have
  // at least the size of the input samples).
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

 private:
  RealType r_;

  // One-delayed input and output samples.
  SampleType xml_{0};
  SampleType yml_{0};
};

}  // namespace radio_core::signal
