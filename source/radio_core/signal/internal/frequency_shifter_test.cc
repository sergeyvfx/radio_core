// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/frequency_shifter.h"

#include <array>

#include "radio_core/math/complex.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::ComplexNear;

TEST(FrequencyShifter, Basic) {
  LocalOscillator<float> local_oscillator(100, 44100);

  std::array<Complex, 1000> samples;
  for (Complex& sample : samples) {
    sample = local_oscillator.IQ();
  }

  FrequencyShifter<float> frequency_shifter(-100, 44100);
  frequency_shifter(samples);

  for (Complex& sample : samples) {
    // On Apple Intel from 2018 the required precision to pass the test is
    // higher than on Apple M2. The former one passes with 3e-5f, while the
    // latter one passes with 1e-6f.
    EXPECT_THAT(sample, ComplexNear(Complex(1, 0.0f), 3e-5f));
  }
}

TEST(FrequencyShifter, SingleSample) {
  LocalOscillator<float> local_oscillator(100, 44100);

  std::array<Complex, 1000> samples;
  for (Complex& sample : samples) {
    sample = local_oscillator.IQ();
  }

  FrequencyShifter<float> frequency_shifter(-100, 44100);
  for (Complex& sample : samples) {
    sample = frequency_shifter(sample);
  }

  for (Complex& sample : samples) {
    EXPECT_THAT(sample, ComplexNear(Complex(1, 0.0f), 1e-6f));
  }
}

}  // namespace radio_core::signal
