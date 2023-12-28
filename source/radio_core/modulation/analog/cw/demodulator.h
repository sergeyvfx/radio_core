// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Continuous wave (CW) demodulation implementation.
//
// Demodulates input complex IQ samples into audio floating point samples.
// Supports both per-sample operation, and demodulation of multiple samples in
// a buffer.

#pragma once

#include <array>
#include <span>

#include "radio_core/modulation/analog/iq_demodulator.h"
#include "radio_core/signal/frequency_shifter.h"

namespace radio_core::modulation::analog::cw {

template <class T>
class Demodulator : public IQDemodulator<T> {
 public:
  struct Options : public IQDemodulator<T>::Options {
    // Sample rate of the input samples.
    T sample_rate{0};

    // The presence of the signal at the center frequency during demodulation
    // will be converted to this tone.
    T tone_frequency{600};
  };

  Demodulator() = default;
  explicit Demodulator(const Options& options) { Configure(options); }

  void Configure(const Options& options) {
    frequency_shifter_.Configure(options.tone_frequency, options.sample_rate);
  }

  inline auto operator()(const BaseComplex<T> sample) -> T override {
    // Only use the real part because the IQ signal is converted to real at
    // around DC where the sine is 0.
    const BaseComplex<T> shifted = frequency_shifter_(sample);
    return shifted.real;
  }

  inline auto operator()(const std::span<const BaseComplex<T>> input_samples,
                         const std::span<T> output_samples)
      -> std::span<T> override {
    assert(input_samples.size() <= output_samples.size());

    const size_t num_samples = input_samples.size();

    // Storage for the frequency shifter.
    //
    // The kBufferSize samples are handled in a single call. Small values
    // increases the amount of overhead, the higher values might not fit into
    // the stack memory.
    constexpr size_t kBufferSize = 1024;
    std::array<BaseComplex<T>, kBufferSize> shifted_samples_buffer;

    for (size_t start_sample_index = 0; start_sample_index < num_samples;) {
      // Calculate the number of samples to be handled at the current loop
      // iteration.
      const size_t num_current_samples =
          Min(kBufferSize, num_samples - start_sample_index);

      // Shift the frequency of the samples.
      const std::span<const BaseComplex<T>> shifted_samples =
          frequency_shifter_(
              input_samples.subspan(start_sample_index, num_current_samples),
              shifted_samples_buffer);

      // Convert to audible signal.
      //
      // Only use the real part because the IQ signal is converted to real at
      // around DC where the sine is 0.
      //
      // TODO(sergey): Investigate if it is possible to benefit from
      // vectorization.
      for (size_t i = 0; i < num_current_samples; ++i) {
        output_samples[start_sample_index + i] = shifted_samples[i].real;
      }

      start_sample_index += num_current_samples;
    }

    return output_samples.subspan(0, num_samples);
  }

 private:
  signal::FrequencyShifter<T> frequency_shifter_;
};

}  // namespace radio_core::modulation::analog::cw
