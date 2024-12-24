// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// A processor which shift frequency of an input quadrature signal by the given
// offset.

#pragma once

#include <span>

#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/rotator.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/frequency.h"

namespace radio_core::signal {

template <class T>
class FrequencyShifter {
 public:
  FrequencyShifter() = default;

  // The frequency is provided in Hz.
  FrequencyShifter(const T frequency_shift, const T sample_rate) {
    Configure(frequency_shift, sample_rate);
  }

  // Configure the frequency shift.
  // The frequency is provided in Hz.
  //
  // If the input frequency is oscillating at frequency 100 Hz and the frequency
  // shift is 400 Hz then the output is an oscillating signal a frequency of
  // 500 Hz.
  void Configure(const T frequency_shift, const T sample_rate) {
    const T normalized_frequency_shift =
        NormalizedAngularFrequency(frequency_shift, sample_rate);

    phase_increment_per_sample_ = BaseComplex<T>(
        Cos(normalized_frequency_shift), Sin(normalized_frequency_shift));
  }

  // Shift frequency of a single sample.
  auto operator()(BaseComplex<T> sample) -> BaseComplex<T> {
    const BaseComplex<T> shifted = sample * phase_;
    phase_ *= phase_increment_per_sample_;

    // TODO(sergey): Only do it every N samples.
    phase_ /= Abs(phase_);

    return shifted;
  }

  // Shift frequency of input samples.
  //
  // The output buffer must have at least same number of elements as the input
  // samples buffer. It is possible to have the output buffer bigger than input
  // in which case the output buffer will only be partially written (only
  // number of input samples will be written to the output).
  //
  // Returns subspan of the output buffer where values has actually been
  // written.
  auto operator()(const std::span<const BaseComplex<T>> samples,
                  const std::span<BaseComplex<T>> output)
      -> std::span<BaseComplex<T>> {
    return kernel::Rotator(
        samples, phase_, phase_increment_per_sample_, output);
  }

  // Shift samples frequency in-place.
  void operator()(const std::span<BaseComplex<T>> samples) {
    (*this)(samples, samples);
  }

 private:
  // Current phase by which the input signal is rotated.
  BaseComplex<T> phase_{1, 0};

  // How much the phase is incremented per sample of the input signal.
  BaseComplex<T> phase_increment_per_sample_{1, 0};
};

}  // namespace radio_core::signal
