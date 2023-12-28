// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/am/demodulator.h"

#include <array>

#include "radio_core/math/complex.h"
#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/test.h"

namespace radio_core::modulation::analog::am {

TEST(Demodulator, SingleSample) {
  constexpr float kAmplitude = 1.21f;
  constexpr float kSampleRate = 96000;
  constexpr float kFrequency = 1000;

  Demodulator<float> demodulator;

  signal::LocalOscillator<float> oscillator(kFrequency, kSampleRate);
  oscillator.OffsetPhase(0.1f);

  const float phase = oscillator.Phase();
  const float i = kAmplitude * Cos(phase);
  const float q = kAmplitude * Sin(phase);

  EXPECT_NEAR(demodulator(Complex(i, q)), kAmplitude, 1e-6f);
}

TEST(Demodulator, DCBlock) {
  constexpr float kAmplitude = 1.21f;
  constexpr float kSampleRate = 96000;
  constexpr float kFrequency = 1000;

  Demodulator<float> demodulator;

  signal::LocalOscillator<float> oscillator(kFrequency, kSampleRate);

  // Push a lot of samples with constant amplitude.
  for (int a = 0; a < int(kSampleRate * 4); ++a) {
    const float phase = oscillator.Phase();
    const float i = kAmplitude * Cos(phase);
    const float q = kAmplitude * Sin(phase);

    demodulator(Complex(i, q));
  }

  // The DC blocker is supposed to remove the DC offset which in this vase
  // moves signal to be constant 0.
  for (int a = 0; a < 10; ++a) {
    const float phase = oscillator.Phase();
    const float i = kAmplitude * Cos(phase);
    const float q = kAmplitude * Sin(phase);

    EXPECT_NEAR(demodulator(Complex(i, q)), 0.0f, 1e-6f);
  }
}

}  // namespace radio_core::modulation::analog::am
