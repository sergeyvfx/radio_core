// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/ema_agc.h"

#include <array>
#include <span>
#include <vector>

#include "radio_core/math/complex.h"
#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(EMAAGC, Single) {
  EMAAGC<float> agc(0.8f, 0.00001f);

  LocalOscillator<float> oscillator(1000, 44100);
  for (int i = 0; i < 256; ++i) {
    const float unit_sample = oscillator.Sine();
    const float small_sample = unit_sample * 0.1f;
    const float agc_sample = agc(small_sample);

    // Allow some time to AGC to stabilize.
    if (i > 64) {
      EXPECT_NEAR(agc_sample, unit_sample, 1e-2f);
    }
  }
}

TEST(EMAAGC, Buffer) {
  EMAAGC<float> agc(0.8f, 0.00001f);

  LocalOscillator<float> oscillator(1000, 44100);

  // Allow some time to AGC to stabilize.
  for (int i = 0; i < 64; ++i) {
    const float unit_sample = oscillator.Sine();
    const float small_sample = unit_sample * 0.1f;
    const float agc_sample = agc(small_sample);
    (void)agc_sample;
  }

  std::vector<float> unit_samples;
  std::vector<float> small_samples;

  for (int i = 0; i < 256; ++i) {
    const float unit_sample = oscillator.Sine();
    const float small_sample = unit_sample * 0.1f;
    unit_samples.push_back(unit_sample);
    small_samples.push_back(small_sample);
  }

  std::array<float, 1024> normalized_buffer;
  std::span normalized_samples = agc(small_samples, normalized_buffer);

  EXPECT_THAT(normalized_samples, Pointwise(FloatNear(1e-2f), unit_samples));
}

TEST(EMAAGC, Complex) {
  // Simple test of complex input samples, currently only to ensure the code
  // compiles and does not have obvious mistakes.
  // TODO(sergey): Make it more comprehensive.

  EMAAGC<Complex, float> agc(0.8f, 0.00001f);

  LocalOscillator<float> oscillator(1000, 44100);
  for (int i = 0; i < 256; ++i) {
    const float unit_sample = oscillator.Sine();
    const float small_sample = unit_sample * 0.1f;
    const Complex agc_sample = agc(Complex(small_sample));

    // Allow some time to AGC to stabilize.
    if (i > 64) {
      EXPECT_NEAR(agc_sample.real, unit_sample, 1e-2f);
      EXPECT_NEAR(agc_sample.imag, 0.0f, 1e-2f);
    }
  }
}
}  // namespace radio_core::signal
