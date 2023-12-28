// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/filter_design.h"

#include <array>

#include "radio_core/signal/window.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(FilterDesign, LowPass) {
  std::array<float, 31> actual_kernel;
  DesignLowPassFilter<float>(actual_kernel,
                             WindowEquation<float, Window::kBoxcar>(),
                             1000.0f,
                             44100.0f);

  // The expected kernel is generated with:
  //   scipy.signal.firwin(31, 1000, window="boxcar", fs=44100, pass_zero=True)

  EXPECT_THAT(actual_kernel,
              Pointwise(FloatNear(1e-6f),
                        std::to_array(
                            {0.01663398f, 0.01925057f, 0.02184959f, 0.0244021f,
                             0.02687923f, 0.02925261f, 0.03149473f, 0.03357936f,
                             0.0354819f,  0.03717974f, 0.03865257f, 0.0398827f,
                             0.04085528f, 0.04155854f, 0.04198394f, 0.04212632f,
                             0.04198394f, 0.04155854f, 0.04085528f, 0.0398827f,
                             0.03865257f, 0.03717974f, 0.0354819f,  0.03357936f,
                             0.03149473f, 0.02925261f, 0.02687923f, 0.0244021f,
                             0.02184959f, 0.01925057f, 0.01663398f})));
}

TEST(FilterDesign, BandPass) {
  std::array<float, 31> actual_kernel;
  DesignBandPassFilter<float>(actual_kernel,
                              WindowEquation<float, Window::kBoxcar>(),
                              1000.0f,
                              2000.0f,
                              44100.0f);

  // The expected kernel is generated with:
  //   scipy.signal.firwin(31, [1000, 2000], window="boxcar", fs=44100,
  //                       pass_zero=False)

  EXPECT_THAT(
      actual_kernel,
      Pointwise(FloatNear(1e-6f),
                std::to_array({
                    -0.05452687f, -0.05547836f, -0.05373721f, -0.04927093f,
                    -0.04219943f, -0.03279306f, -0.02146121f, -0.00873228f,
                    0.00477412f,  0.01838092f,  0.03139157f,  0.04312964f,
                    0.0529778f,   0.06041379f,  0.0650409f,   0.06661135f,
                    0.0650409f,   0.06041379f,  0.0529778f,   0.04312964f,
                    0.03139157f,  0.01838092f,  0.00477412f,  -0.00873228f,
                    -0.02146121f, -0.03279306f, -0.04219943f, -0.04927093f,
                    -0.05373721f, -0.05547836f, -0.05452687f,
                })));
}

TEST(FilterDesign, FractionalDelay) {
  std::array<float, 31> actual_kernel;
  DesignFractionalDelayFilter<float>(
      actual_kernel, WindowEquation<float, Window::kBoxcar>(), 0.3f);

  // The kernel is obtained by following code from the article:
  //
  //  https://tomroelandts.com/articles/how-to-create-a-fractional-delay-filter
  //
  // The code used to generate the expected values:
  //
  //   import numpy as np
  //
  //   tau = 0.3  # Fractional delay [samples].
  //   N = 31     # Filter length.
  //   n = np.arange(N)
  //
  //   h = np.sinc(n - (N - 1) / 2 - tau)  # Compute sinc filter.
  //   h /= np.sum(h)                      # Normalize to get unity gain.
  //
  //   print(h)

  EXPECT_THAT(
      actual_kernel,
      Pointwise(FloatNear(1e-6f),
                std::to_array({
                    -0.01682585f, 0.01800249f,  -0.01935606f, 0.02092972f,
                    -0.02278191f, 0.02499374f,  -0.02768124f, 0.03101633f,
                    -0.03526514f, 0.04086279f,  -0.04857275f, 0.05986873f,
                    -0.07801077f, 0.1119285f,   -0.19802735f, 0.8581185f,
                    0.36776507f,  -0.15143268f, 0.0953465f,   -0.06957718f,
                    0.05477352f,  -0.04516413f, 0.03842322f,  -0.03343319f,
                    0.02959029f,  -0.02653975f, 0.0240594f,   -0.02200304f,
                    0.02027052f,  -0.01879092f, 0.01751262f,
                })));
}

}  // namespace radio_core::signal
