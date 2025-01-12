// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/root_raised_cosine.h"

#include <array>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(root_raised_cosine, DesignLowpassRRCFilter) {
  std::array<float, 51> actual_kernel;
  DesignLowpassRRCFilter<float>(actual_kernel, 2, 0.2f);

  // The values are obtained by carefully verifying formula and running it prior
  // to any optimization. They were verified against GNU Radio:
  //
  //   >>> from gnuradio.filter import firdes
  //   >>> firdes.root_raised_cosine(gain=1, sampling_freq=2, symbol_rate=1,
  //   ...                           alpha=0.2, ntaps=51)
  //
  // The result of the firdes.root_raised_cosine() is within the same tolerance
  // from the values used by this test.
  EXPECT_THAT(
      actual_kernel,
      Pointwise(
          FloatNear(1e-6f),
          std::to_array({0.00128433f,  -0.00029283f, -0.00113273f, 0.00123459f,
                         0.00035790f,  -0.00201823f, 0.00097255f,  0.00222748f,
                         -0.00251670f, -0.00145610f, 0.00363281f,  -0.00058290f,
                         -0.00339835f, 0.00396075f,  0.00064315f,  -0.00847655f,
                         0.00609374f,  0.01366019f,  -0.01894098f, -0.01883961f,
                         0.04238274f,  0.02326111f,  -0.09223914f, -0.02623679f,
                         0.31312138f,  0.52659625f,  0.31312138f,  -0.02623679f,
                         -0.09223914f, 0.02326111f,  0.04238274f,  -0.01883961f,
                         -0.01894098f, 0.01366019f,  0.00609374f,  -0.00847655f,
                         0.00064315f,  0.00396075f,  -0.00339835f, -0.00058290f,
                         0.00363281f,  -0.00145610f, -0.00251670f, 0.00222748f,
                         0.00097255f,  -0.00201823f, 0.00035790f,  0.00123459f,
                         -0.00113273f, -0.00029283f, 0.00128433f})));
}

}  // namespace radio_core::signal
