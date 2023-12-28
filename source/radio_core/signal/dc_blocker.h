// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Filter which removes DC component from a signal.
//
// The parameter R affects on the averaging window (which can be approximated
// with `1 / (1 - R)`). Smaller R values allows faster tracking but in the cost
// of higher low frequency attenuation.
//
// The implementation follows:
//
//   https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
//   https://groups.google.com/g/digitalvoice/c/RakFvpVWR2w/m/hQSWn7MO-2gJ

#pragma once

namespace radio_core::signal {

template <class T>
class DCBlocker {
 public:
  explicit DCBlocker(const T r) : r_(r) {}

  inline auto operator()(const T x) -> T {
    const T y = x - xml_ + r_ * yml_;
    xml_ = x;
    yml_ = y;
    return y;
  }

 private:
  T r_;

  // One-delayed input and output samples.
  T xml_ = T(0), yml_ = T(0);
};

}  // namespace radio_core::signal
