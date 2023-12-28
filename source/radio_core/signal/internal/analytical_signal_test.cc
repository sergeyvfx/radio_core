// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/analytical_signal.h"

#include "radio_core/base/constants.h"
#include "radio_core/math/complex.h"
#include "radio_core/signal/window.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(AnalyticalSignal, Basic) {
  static constexpr int kSampleRate = 44100;

  AnalyticalSignal<float> analytical_signal{};
  analytical_signal.Design(81, WindowEquation<float, Window::kHamming>());

  float prev_diff_phase = 0.0f;
  int num_tested_sample = 0;
  for (int i = 0; i < kSampleRate; ++i) {
    const float sample =
        Sin(float(i) * 2.0f * float(constants::pi) * 1200.0f / kSampleRate);

    const Complex analytic_sample = analytical_signal(sample);

    const float phase = ArcTan2(analytic_sample.imag, analytic_sample.real);

    // Differentiate the phase into instant frequency in radians per sample.
    // Additionally unwrap the it, so that it stays positive when the phase
    // crosses 0 radians.
    float frequency = phase - prev_diff_phase;
    prev_diff_phase = phase;
    if (frequency > float(constants::pi)) {
      frequency -= 2 * float(constants::pi);
    } else if (frequency < -float(constants::pi)) {
      frequency += 2 * float(constants::pi);
    }

    // Allow the filter to stabilize.
    if (i < kSampleRate / 4) {
      continue;
    }

    // Frequency Hz.
    const float frequency_hz =
        frequency * kSampleRate / (2 * float(constants::pi));

    EXPECT_NEAR(frequency_hz, 1200.0f, 10.0f);

    // No need to test all samples, it will only make output nreadable in the
    // case of failure.
    ++num_tested_sample;
    if (num_tested_sample == 10) {
      break;
    }
  }
}

}  // namespace radio_core::signal
