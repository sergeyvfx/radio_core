// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/local_oscillator.h"

#include <array>

#include "radio_core/base/algorithm.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(LocalOscillator, SineGenerator) {
  LocalOscillator<float> oscillator(5, 100);

  // >>> import numpy as np
  // >>> sample_rate = 100.0
  // >>> frequency = 5
  // >>> time = np.arange(0, 1, 1 / sample_rate)
  // >>> np.sin(2 * np.pi * frequency * time)
  constexpr auto kExpectedSamples = std::to_array({
      0.00000000e+00f,  3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,
      9.51056516e-01f,  1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,
      5.87785252e-01f,  3.09016994e-01f,  1.22464680e-16f,  -3.09016994e-01f,
      -5.87785252e-01f, -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f,
      -9.51056516e-01f, -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f,
      -2.44929360e-16f, 3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,
      9.51056516e-01f,  1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,
      5.87785252e-01f,  3.09016994e-01f,  3.67394040e-16f,  -3.09016994e-01f,
      -5.87785252e-01f, -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f,
      -9.51056516e-01f, -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f,
      -4.89858720e-16f, 3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,
      9.51056516e-01f,  1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,
      5.87785252e-01f,  3.09016994e-01f,  6.12323400e-16f,  -3.09016994e-01f,
      -5.87785252e-01f, -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f,
      -9.51056516e-01f, -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f,
      -7.34788079e-16f, 3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,
      9.51056516e-01f,  1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,
      5.87785252e-01f,  3.09016994e-01f,  -2.69546092e-15f, -3.09016994e-01f,
      -5.87785252e-01f, -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f,
      -9.51056516e-01f, -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f,
      -9.79717439e-16f, 3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,
      9.51056516e-01f,  1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,
      5.87785252e-01f,  3.09016994e-01f,  1.10218212e-15f,  -3.09016994e-01f,
      -5.87785252e-01f, -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f,
      -9.51056516e-01f, -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f,
  });

  float max_error = 0;

  for (int x = 0; x < 100000; ++x) {
    for (int i = 0; i < 100; ++i) {
      const float error = Abs(oscillator.Sine() - kExpectedSamples[i]);
      max_error = Max(max_error, error);
    }
  }

  EXPECT_LT(max_error, 1e-6f);
}

TEST(LocalOscillator, CosineGenerator) {
  LocalOscillator<float> oscillator(5, 100);

  // >>> import numpy as np
  // >>> sample_rate = 100.0
  // >>> frequency = 5
  // >>> time = np.arange(0, 1, 1 / sample_rate)
  // >>> np.sin(2 * np.pi * frequency * time)
  constexpr auto kExpectedSamples = std::to_array({
      1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,  5.87785252e-01f,
      3.09016994e-01f,  6.12323400e-17f,  -3.09016994e-01f, -5.87785252e-01f,
      -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f, -9.51056516e-01f,
      -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f, -1.83697020e-16f,
      3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,  9.51056516e-01f,
      1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,  5.87785252e-01f,
      3.09016994e-01f,  3.06161700e-16f,  -3.09016994e-01f, -5.87785252e-01f,
      -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f, -9.51056516e-01f,
      -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f, 1.34773046e-15f,
      3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,  9.51056516e-01f,
      1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,  5.87785252e-01f,
      3.09016994e-01f,  5.51091060e-16f,  -3.09016994e-01f, -5.87785252e-01f,
      -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f, -9.51056516e-01f,
      -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f, 1.10280110e-15f,
      3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,  9.51056516e-01f,
      1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,  5.87785252e-01f,
      3.09016994e-01f,  -9.80336420e-16f, -3.09016994e-01f, -5.87785252e-01f,
      -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f, -9.51056516e-01f,
      -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f, -2.69484194e-15f,
      3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,  9.51056516e-01f,
      1.00000000e+00f,  9.51056516e-01f,  8.09016994e-01f,  5.87785252e-01f,
      3.09016994e-01f,  2.81730662e-15f,  -3.09016994e-01f, -5.87785252e-01f,
      -8.09016994e-01f, -9.51056516e-01f, -1.00000000e+00f, -9.51056516e-01f,
      -8.09016994e-01f, -5.87785252e-01f, -3.09016994e-01f, 6.12942380e-16f,
      3.09016994e-01f,  5.87785252e-01f,  8.09016994e-01f,  9.51056516e-01f,
  });

  float max_error = 0;

  for (int x = 0; x < 100000; ++x) {
    for (int i = 0; i < 100; ++i) {
      const float error = Abs(oscillator.Cosine() - kExpectedSamples[i]);
      max_error = Max(max_error, error);
    }
  }

  EXPECT_LT(max_error, 1e-6f);
}

TEST(LocalOscillator, IQ) {
  LocalOscillator<float> oscillator(5, 100);

  LocalOscillator<float> oscillator_i(5, 100);
  LocalOscillator<float> oscillator_q(5, 100);

  float max_error = 0;

  for (int x = 0; x < 100000; ++x) {
    for (int a = 0; a < 100; ++a) {
      const float i = oscillator_i.Cosine();
      const float q = oscillator_q.Sine();

      const Complex iq = oscillator.IQ();

      max_error = Max(max_error, Abs(iq.real - i));
      max_error = Max(max_error, Abs(iq.imag - q));
    }
  }

  EXPECT_LT(max_error, 1e-6f);
}

}  // namespace radio_core::signal
