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

 private:
  RealType r_;

  // One-delayed input and output samples.
  SampleType xml_{0};
  SampleType yml_{0};
};

}  // namespace radio_core::signal
