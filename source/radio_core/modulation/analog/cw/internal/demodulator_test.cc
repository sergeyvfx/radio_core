// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/cw/demodulator.h"

#include <array>

#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/test.h"

namespace radio_core::modulation::analog::cw {

// Construct typical objects and test typical API, to ensure there is no typos
// in the templated code and that it compiles.
TEST(Demodulator, Compilation) {
  const Demodulator<float>::Options options = {
      .sample_rate = 1,
  };

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

TEST(Demodulator, Basic) {
  constexpr float kSampleRate = 11024;
  constexpr float kToneFrequency = 600;

  const Demodulator<float>::Options options = {
      .sample_rate = kSampleRate,
      .tone_frequency = kToneFrequency,
  };

  Demodulator<float> demodulator;
  demodulator.Configure(options);

  signal::LocalOscillator<float> oscillator(kToneFrequency, kSampleRate);

  // TODO(sergey): FOrmulate the test in a way that does not lead to printing of
  // a lot of text when the test failure occurs.
  for (int a = 0; a < 1000; ++a) {
    EXPECT_NEAR(demodulator(Complex(1.0f, 0.0f)), oscillator.Cosine(), 1e-5f);
  }
}

}  // namespace radio_core::modulation::analog::cw
