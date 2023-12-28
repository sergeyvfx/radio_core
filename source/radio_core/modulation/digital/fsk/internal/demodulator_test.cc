// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Only synthetic and simple tests are performed here.
//
// The actual performance in terms of dealing with low SNR is tested by a
// packet protocol specific tests.

#include "radio_core/modulation/digital/fsk/demodulator.h"

#include <vector>

#include "radio_core/base/frequency_duration.h"
#include "radio_core/modulation/digital/fsk/tones_bell.h"
#include "radio_core/signal/generator.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::modulation::digital::fsk {

using testing::Eq;
using testing::Pointwise;

class BitReceiver {
 public:
  std::vector<bool> bits;

  void operator()(const bool bit) { bits.push_back(bit); }
};

TEST(fsk, Demodulator) {
  Demodulator<float>::Options options;
  options.tones = kBell202Tones;
  options.sample_rate = 11025;
  options.data_baud = 1200;

  Demodulator<float> demodulator(options);

  signal::Generator<float> generator(options.sample_rate);

  BitReceiver receiver;

  const float bit_duration_ms = 1000.0f / float(options.data_baud);
  for (int i = 0; i < 4; ++i) {
    generator(FrequencyDuration(kBell202Tones.mark, bit_duration_ms),
              [&](const float sample) { demodulator(sample, receiver); });
    generator(FrequencyDuration(kBell202Tones.space, bit_duration_ms),
              [&](const float sample) { demodulator(sample, receiver); });
  }

  EXPECT_THAT(
      receiver.bits,
      Pointwise(
          Eq(),
          std::to_array({true, false, true, false, true, false, true, false})));
}

}  // namespace radio_core::modulation::digital::fsk
