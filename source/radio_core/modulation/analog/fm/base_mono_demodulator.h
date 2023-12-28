// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of a mono-channel frequency demodulator.
// Takes care of implementing common parts of demodulating NFM and a mono
// channel of WFM.

#pragma once

#include <span>

#include "radio_core/math/kernel/fast_arg.h"
#include "radio_core/modulation/analog/iq_demodulator.h"
#include "radio_core/signal/frequency.h"

namespace radio_core::modulation::analog::fm {

template <class T>
class BaseMonoDemodulator : public IQDemodulator<T> {
 public:
  struct Options : public IQDemodulator<T>::Options {
    // Sample rate of the input samples.
    T sample_rate{0};

    // Frequency deviation in Hertz.
    T deviation{0};
  };

  void Configure(const Options& options) {
    SetDeviation(options.deviation, options.sample_rate);
  }

  inline void SetDeviation(const T deviation, const T sample_rate) {
    SetAngularDeviation(
        signal::NormalizedAngularFrequency<T>(deviation, sample_rate));
  }

  inline void SetAngularDeviation(const T angular_deviation) {
    angular_deviation_ = angular_deviation;
    angular_deviation_inv_ = T(1) / angular_deviation_;
  }

  inline auto GetAngularDeviation() const -> T { return angular_deviation_; }

  inline auto operator()(const BaseComplex<T> sample) -> T override {
    const T phase = FastArg(sample);
    const T instant_frequency =
        signal::UnwrapInstantFrequency(phase - prev_phase_);

    prev_phase_ = phase;

    return instant_frequency * angular_deviation_inv_;
  }

  inline auto operator()(const std::span<const BaseComplex<T>> input_samples,
                         const std::span<T> output_samples)
      -> std::span<T> override {
    assert(input_samples.size() <= output_samples.size());

    // Calculate phase angle.
    // The phase is written to the output buffer.
    const std::span<T> written_samples = FastArg(input_samples, output_samples);

    // Calculate and unwrap instant frequency, and divide by deviation to get
    // amplitude of the output signal. Uses phase stored in the output buffer
    // from the previous algorithm step.
    //
    // Scalar multiplication is faster to happen here (as opposite of attempting
    // to use vectorization). At least on Raspberry Pi 4. where the current code
    // is 15% faster than multiplication performed as separate vectorized pass.
    // This is likely due to better memory access pattern, and the fact that
    // there are no additional load and store to/from registers.
    for (T& output_sample : written_samples) {
      const T phase = output_sample;
      output_sample = signal::UnwrapInstantFrequency(phase - prev_phase_) *
                      angular_deviation_inv_;
      prev_phase_ = phase;
    }

    return written_samples;
  }

 protected:
  BaseMonoDemodulator() = default;

  explicit BaseMonoDemodulator(const Options& options) { Configure(options); }

 private:
  T angular_deviation_{1};
  T angular_deviation_inv_{T(1) / angular_deviation_};

  T prev_phase_{0};
};

}  // namespace radio_core::modulation::analog::fm
