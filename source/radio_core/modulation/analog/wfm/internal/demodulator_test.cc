// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/wfm/demodulator.h"

#include "radio_core/signal/frequency.h"
#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/test.h"

namespace radio_core::modulation::analog::wfm {

TEST(Demodulator, DCBlock) {
  constexpr float kAmplitude = 1.21f;
  constexpr float kSampleRate = 25000.0f;
  constexpr float kFrequency = 12500.0f / 8;

  Demodulator<float> demodulator;
  demodulator.SetAngularDeviation(
      radio_core::signal::NormalizedAngularFrequency<float>(12500.0f / 2,
                                                            kSampleRate));

  signal::LocalOscillator<float> oscillator(kFrequency, kSampleRate);

  // Push a lot of samples with the constant frequency.
  for (int a = 0; a < int(kSampleRate * 4); ++a) {
    const float phase = oscillator.Phase();
    const float i = kAmplitude * Cos(phase);
    const float q = kAmplitude * Sin(phase);

    demodulator(Complex(i, q));
  }

  // Check that constant frequency gives a flat enough DC output.
  for (int a = 0; a < 10; ++a) {
    const float phase = oscillator.Phase();
    const float i = kAmplitude * Cos(phase);
    const float q = kAmplitude * Sin(phase);

    EXPECT_NEAR(Abs(demodulator(Complex(i, q))), 0.25f, 1e-2f);
  }
}

}  // namespace radio_core::modulation::analog::wfm
