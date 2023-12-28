// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/awgn_noise_injector.h"

#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(signal, AWGNNoiseInjector) {
  AWGNNoiseInjector<float>::Options options;
  options.snr_db = 30;
  AWGNNoiseInjector<float> noise_injector(options);

  float total_noise = 0;
  int num_noisy_samples = 0;
  LocalOscillator<float> oscillator(1000, 44100);
  for (int i = 0; i < 100; ++i) {
    const float clean_signal = oscillator.Sine();
    const float noisy_signal = noise_injector(clean_signal);

    const float noise = Abs(clean_signal - noisy_signal);

    total_noise += noise;
    ++num_noisy_samples;
  }

  const float average_noise = total_noise / float(num_noisy_samples);

  // Not very scientific: just ensure that some noise has been added, and it is
  // not too much and not too little.
  EXPECT_LT(average_noise, 0.08f);
  EXPECT_GT(average_noise, 0.05f);
}

}  // namespace radio_core::signal
