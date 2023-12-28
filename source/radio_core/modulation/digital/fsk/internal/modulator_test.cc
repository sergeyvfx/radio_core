// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/digital/fsk/modulator.h"

#include <array>
#include <vector>

#include "radio_core/base/frequency_duration.h"
#include "radio_core/modulation/digital/fsk/tones_bell.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::modulation::digital::fsk {

using testing::Eq;
using testing::Pointwise;

class FrequencyDurationReceiver {
 public:
  std::vector<FrequencyDuration<float>> frequencies_and_durations;

  void operator()(const FrequencyDuration<float> frequency_duration) {
    frequencies_and_durations.push_back(frequency_duration);
  }
};

TEST(fsk, Modulator) {
  Modulator<float>::Options options;
  options.tones = kBell202Tones;
  options.data_baud = 1200;

  Modulator<float> modulator(options);

  FrequencyDurationReceiver receiver;

  modulator(false, receiver);
  modulator(true, receiver);
  modulator(false, receiver);
  modulator(false, receiver);
  modulator(true, receiver);
  modulator(true, receiver);

  const float bit_duration_ms = 1000.0f / float(options.data_baud);
  const FrequencyDuration mark_signal(kBell202Tones.mark, bit_duration_ms);
  const FrequencyDuration space_signal(kBell202Tones.space, bit_duration_ms);

  EXPECT_THAT(receiver.frequencies_and_durations,
              Pointwise(Eq(),
                        std::to_array({
                            space_signal,
                            mark_signal,
                            space_signal,
                            space_signal,
                            mark_signal,
                            mark_signal,
                        })));
}

}  // namespace radio_core::modulation::digital::fsk
