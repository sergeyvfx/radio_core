// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/instant_frequency.h"

#include "radio_core/base/constants.h"
#include "radio_core/math/math.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(InstantPhase, Basic) {
  static constexpr int kSampleRate = 44100;

  InstantFrequency<float> machine(kSampleRate);

  machine(0.0f);

  for (int i = 1; i < kSampleRate / 4; ++i) {
    const float phase =
        float(i) * (2.0f * float(constants::pi) * 1200.0f / kSampleRate);

    EXPECT_NEAR(machine(phase), 1200.0f, 1.0f)
        << "mismatch at sample " << i << ", phase " << phase;
  }

  // TODO(seregey): Figure out numerical stability issues with high number of
  // test samples. Seems that the math around calculating phase looses precision
  // overtime, causing quite measurable difference in frequency calculation.
}

}  // namespace radio_core::signal
