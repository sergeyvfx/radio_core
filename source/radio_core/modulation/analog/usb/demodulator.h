// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Upper Side Band demodulation implementation.
//
// Demodulates input complex IQ samples into audio floating point samples.
// Supports both per-sample operation, and demodulation of multiple samples in
// a buffer.

#pragma once

#include "radio_core/modulation/analog/ssb/demodulator.h"

namespace radio_core::modulation::analog::usb {

template <class T, template <class> class Allocator = std::allocator>
class Demodulator
    : public ssb::Demodulator<T, ssb::Sideband::kUpper, Allocator> {
  using BaseClass = ssb::Demodulator<T, ssb::Sideband::kUpper, Allocator>;

 public:
  struct Options : public BaseClass::Options {};

  using BaseClass::BaseClass;
};

}  // namespace radio_core::modulation::analog::usb
