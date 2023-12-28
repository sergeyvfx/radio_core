// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Wideband frequency demodulation implementation.
//
// Demodulates input complex IQ samples into audio floating point samples.
// Supports both per-sample operation, and demodulation of multiple samples in
// a buffer.

// TODO(sergey): Support stereo mode.

// TODO(sergey): Support deemphasis. Although, it might not be a part of the
// demodulator.

#pragma once

#include <span>

#include "radio_core/modulation/analog/iq_demodulator.h"
#include "radio_core/modulation/analog/nfm/demodulator.h"
#include "radio_core/signal/frequency.h"

namespace radio_core::modulation::analog::wfm {

template <class T>
class Demodulator : public IQDemodulator<T> {
 public:
  struct Options : public IQDemodulator<T>::Options {
    // Sample rate of the input samples.
    T sample_rate{0};

    // Frequency deviation in Hertz.
    T deviation{0};
  };

  Demodulator() = default;
  explicit Demodulator(const Options& options) { Configure(options); }

  void Configure(const Options& options) {
    SetDeviation(options.deviation, options.sample_rate);
  }

  inline void SetDeviation(const T deviation, const T sample_rate) {
    SetAngularDeviation(
        signal::NormalizedAngularFrequency<T>(deviation, sample_rate));
  }

  inline void SetAngularDeviation(const T angular_deviation) {
    mono_demodulator_.SetAngularDeviation(angular_deviation);
  }

  inline auto GetAngularDeviation() const -> T {
    return mono_demodulator_.GetAngularDeviation();
  }

  inline auto operator()(const BaseComplex<T> sample) -> T override {
    return mono_demodulator_(sample);
  }

  inline auto operator()(const std::span<const BaseComplex<T>> input_samples,
                         const std::span<T> output_samples)
      -> std::span<T> override {
    return mono_demodulator_(input_samples, output_samples);
  }

 private:
  nfm::Demodulator<T> mono_demodulator_;
};

}  // namespace radio_core::modulation::analog::wfm
