// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/usb/demodulator.h"

#include <array>

#include "radio_core/unittest/test.h"

namespace radio_core::modulation::analog::usb {

// Construct typical objects and test typical API, to ensure there is no typos
// in the templated code and that it compiles.
TEST(Demodulator, Compilation) {
  const Demodulator<float>::Options options;

  Demodulator<float> demodulator;
  demodulator.Configure(options);

  // Single sample demodulation.
  (void)demodulator(Complex(0, 0));

  // Multiple sample demodulation.
  {
    std::array<Complex, 2> input{Complex(0, 0), Complex(0, 0)};
    std::array<float, 2> demodulated;
    (void)demodulator(input, demodulated);
  }
}

// NOTE: For the actual demodulation verification rely on the test of the base
// class.

}  // namespace radio_core::modulation::analog::usb
