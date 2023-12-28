// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/interpolator.h"

#include <array>
#include <span>

#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(Interpolator, Basic) {
  constexpr int kNum22050Samples = 22050 * 2;

  Interpolator<float> interpolator(2);

  // Generate 1000 Hz sinewave at the original sample rate.
  LocalOscillator<float> oscillator22050(1000, 22050);
  std::array<float, kNum22050Samples> samples22050;
  for (int i = 0; i < kNum22050Samples; ++i) {
    samples22050[i] = oscillator22050.Sine();
  }

  std::array<float, 44100 * 2> samples44100;
  interpolator(samples22050, samples44100);

  // Skip first half: thats where the filter state might not have been fully
  // stabilized yet, making it hard to compare.
  std::span clean44100(samples44100.begin() + 44100, samples44100.end());

  LocalOscillator<float> oscillator44100(1000, 44100);
  // Offset the oscillator to align with the beginning of the sinewave which has
  // a delay w.r.t. the input sinewave.
  //
  // TODO(sergey): Make this calculation more scientific. Currently it is done
  // by plotting the signal. Or find a way to test the signal is a sinewave with
  // a known frequency but unknown phase.
  oscillator44100.OffsetPhase((-64.0f - 0.06f) * 2.0f * constants::pi_v<float> *
                              1000.0f / 44100.0f);
  for (int i = 0; i < 10; ++i) {
    const float sample = oscillator44100.Sine();
    EXPECT_NEAR(clean44100[i], sample, 1e-2f);
  }
}

TEST(Interpolator, CalcNeededOutputBufferSize) {
  Interpolator<float> interpolator(10);
  EXPECT_EQ(interpolator.CalcNeededOutputBufferSize(20), 201);
  EXPECT_EQ(interpolator.CalcNeededOutputBufferSize(21), 211);
}

}  // namespace radio_core::signal
