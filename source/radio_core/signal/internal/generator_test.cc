// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/generator.h"

#include <vector>

#include "radio_core/base/constants.h"
#include "radio_core/math/math.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

class SampleReceiver {
 public:
  std::vector<float> samples;

  void operator()(const float sample) { samples.push_back(sample); }
};

// Basic usage: one long continuous waveform.
TEST(Generator, Basic) {
  constexpr int kSampleRate = 44100;
  constexpr float kFrequency = 400.0f;

  Generator<float> generator(kSampleRate);

  SampleReceiver receiver;

  generator(FrequencyDuration(kFrequency, 1000.0f), receiver);

  // The very first sample is at time 0, which is an extra in the storage.
  EXPECT_EQ(receiver.samples.size(), kSampleRate + 1);

  // NOTE: Order of multiplication and offset do have precision difference which
  // is enough for sinewave to give quite different results. Hence rather high
  // tolerance value.
  for (int i = 0; i <= kSampleRate; ++i) {
    const float expected_sample =
        Sin(float(i) * 2.0f * float(constants::pi) * kFrequency / kSampleRate);
    EXPECT_NEAR(receiver.samples[i], expected_sample, 1e-3f);
  }
}

TEST(Generator, LotOfShortCalls) {
  constexpr int kSampleRate = 44100;
  constexpr float kFrequency = 400.0f;

  Generator<float> generator(kSampleRate);

  SampleReceiver receiver;

  for (int i = 0; i < 4 * kSampleRate; ++i) {
    generator(FrequencyDuration(kFrequency, 1000.0f / kSampleRate / 4),
              receiver);
  }

  // The very first sample is at time 0, which is an extra in the storage.
  EXPECT_EQ(receiver.samples.size(), kSampleRate + 1);

  // NOTE: Order of multiplication and offset do have precision difference which
  // is enough for sinewave to give quite different results. Hence rather high
  // tolerance value.
  for (int i = 0; i <= kSampleRate; ++i) {
    const float expected_sample =
        Sin(float(i) * 2.0f * float(constants::pi) * kFrequency / kSampleRate);
    EXPECT_NEAR(receiver.samples[i], expected_sample, 3e-3f)
        << " at sample " << i;
  }
}

TEST(Generator, FadeToZero) {
  constexpr int kSampleRate = 44100;
  constexpr float kFrequency = 400.0f;

  Generator<float> generator(kSampleRate);

  SampleReceiver receiver;

  generator(FrequencyDuration(kFrequency, 0.1f), receiver);

  // Verify generation stopped at non-0 amplitude.
  EXPECT_GT(receiver.samples.back(), 0.1f);

  // Fade to 0 and verify last sample is close to 0.
  generator.FadeToZero(receiver);
  EXPECT_NEAR(receiver.samples.back(), 0.0f, 1e-6f);

  // Make sure pushing "pause" sample after fade keeps amplitude at 0.
  generator(FrequencyDuration(0.0f, 0.1f), receiver);
  EXPECT_NEAR(receiver.samples.back(), 0.0f, 1e-6f);
}

}  // namespace radio_core::signal
