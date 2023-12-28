// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Only synthetic and simple tests are performed here.
//
// The actual performance in terms of dealing with low SNR is tested by a
// packet protocol specific tests.

#include "radio_core/modulation/digital/fsk/internal/symbol_demodulator.h"

#include "radio_core/base/frequency_duration.h"
#include "radio_core/signal/generator.h"
#include "radio_core/signal/local_oscillator.h"
#include "radio_core/unittest/test.h"

namespace radio_core::modulation::digital::fsk::internal {

TEST(SymbolDemodulator, ContinuousSymbol) {
  SymbolDemodulator<float>::Options options;
  options.tone_frequency = 1200;
  options.sample_rate = 11025;
  options.data_baud = 1200;

  SymbolDemodulator<float> demodulator(options);

  signal::LocalOscillator<float> oscillator(options.tone_frequency,
                                            options.sample_rate);

  // Get past all transients during the initial startup.
  for (int i = 0; i < int(options.sample_rate); ++i) {
    demodulator(oscillator.Sine());
  }

  // Expect high enough magnitude of symbol.
  for (int i = 0; i < 10; ++i) {
    EXPECT_GT(demodulator(oscillator.Sine()), 0.9f);
  }
}

TEST(SymbolDemodulator, OtherSymbol) {
  SymbolDemodulator<float>::Options options;
  options.tone_frequency = 1200;
  options.sample_rate = 11025;
  options.data_baud = 1200;

  SymbolDemodulator<float> demodulator(options);

  signal::Generator<float> generator(options.sample_rate);

  // Get past all transients during the initial startup.
  for (int i = 0; i < 5; ++i) {
    generator(FrequencyDuration(1200, 1000.0f / 1200.0f),
              [&demodulator](const float sample) { demodulator(sample); });
  }

  // Submit single symbol to let the filters to catch up a bit.
  // This is an alternative to what happens in practice when bit is sampled at
  // at its mid-time.
  generator(FrequencyDuration(2200, 1000.0f / 1200.0f),
            [&demodulator](const float sample) { demodulator(sample); });

  // Expect low enough symbol amplitude.
  generator(FrequencyDuration(2200, 1000.0f / 1200.0f),
            [&demodulator](const float sample) {
              EXPECT_LT(demodulator(sample), 0.7f);
            });
}

}  // namespace radio_core::modulation::digital::fsk::internal
