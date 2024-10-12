// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Amplitude demodulation implementation.
//
// Demodulates input complex IQ samples into audio floating point samples.
// Supports both per-sample operation, and demodulation of multiple samples in
// a buffer.

#pragma once

#include <span>

#include "radio_core/math/kernel/fast_abs.h"
#include "radio_core/modulation/analog/iq_demodulator.h"
#include "radio_core/signal/dc_blocker.h"

namespace radio_core::modulation::analog::am {

template <class T>
class Demodulator : public IQDemodulator<T> {
 public:
  struct Options : public IQDemodulator<T>::Options {};

  Demodulator() = default;
  explicit Demodulator(const Options& options) { Configure(options); }

  void Configure(const Options& /*options*/) {}

  inline auto operator()(const BaseComplex<T> sample) -> T override {
    const T dc_magnitude = Abs(sample);
    return dc_blocker_(dc_magnitude);
  }

  inline auto operator()(const std::span<const BaseComplex<T>> input_samples,
                         const std::span<T> output_samples)
      -> std::span<T> override {
    assert(input_samples.size() <= output_samples.size());

    // Calculate magnitude of all input samples.
    const std::span<T> written_samples =
        kernel::FastAbs(input_samples, output_samples);

    // Remove DC offset of the magnitude.
    for (T& output_sample : written_samples) {
      output_sample = dc_blocker_(output_sample);
    }

    return written_samples;
  }

 private:
  // TODO(sergey): Consider making parameter configurable.
  signal::DCBlocker<T> dc_blocker_{0.995f};
};

}  // namespace radio_core::modulation::analog::am
