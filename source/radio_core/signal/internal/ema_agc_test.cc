// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/ema_agc.h"

#include <array>
#include <span>
#include <vector>

#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"
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

// Test whether the AGC really reaches expected state with the configuration
// tips provided in the documentation (which are derived from
// [Wikipedia-TimeConstant]).
TEST(EMAAGC, TimeConstant) {
  const int N = 25;
  const float kAlpha = 1.0f - Exp(-3.0f / N);

  EMAAGC<float> agc(kAlpha, kAlpha);

  // Push the first part of the step functions (all zeroes) to the AGC. This
  // ensures that internal state is fully stabilized.
  for (int i = 0; i < 100; ++i) {
    (void)agc(0.0f);
    EXPECT_NEAR(agc.GetCharge(), 0.0f, 1e-6f);
  }

  // Push the second part of the step function (all ones) to the AGC.
  // Ensure that the AGC is monotonously charging, and that the charge does not
  // exceed expected value (with some tolerance).
  float previous_charge = 0.0f;
  for (int i = 0; i < N; ++i) {
    (void)agc(1.0f);
    const float new_charge = agc.GetCharge();
    EXPECT_LT(new_charge, 0.98f);
    EXPECT_GT(new_charge, previous_charge);
    previous_charge = new_charge;
  }

  // After N samples it is expected that the AGC reaches 95% of the final value
  // ([Wikipedia-TimeConstant], also documentation in the ema_agc.h).
  // Test for this (with some tolerance).
  EXPECT_NEAR(previous_charge, 0.95f, 1e-2f);
}

}  // namespace radio_core::signal
