// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// An injector of noise into an input signal using Additive White Gaussian Noise
// (AWGN) noise model.
//
// The processor will normalize the output value to make it fit into [-1 .. 1]
// range. Note that due to the nature of Gaussian noise ideal normalization is
// not possible due to unlimited range of noise signal samples. A 3σ estimate is
// used to estimate peak amplitude of the noise signal. The clipping is still
// possible but in worst case of ~0.03% of samples.
//
// References:
//
//   Additive white Gaussian noise
//   https://wikipedia.org/wiki/Additive_white_Gaussian_noise
//
//   68–95–99.7 rule
//   https://wikipedia.org/wiki/68%E2%80%9395%E2%80%9399.7_rule
//
//   Adding noise to a signal in Python
//   https://stackoverflow.com/a/53688043

#pragma once

#include <random>

#include "radio_core/math/distribution.h"
#include "radio_core/math/math.h"

namespace radio_core::signal {

template <class RealType>
class AWGNNoiseInjector {
 public:
  // Default input is a sinewave with peak amplitude of 1 volt.
  struct Options {
    // Power of the input signal in dBm. Assumes a constant power of the signal.
    RealType signal_db{0};

    // Peak amplitude (maximum of absolute value of voltage of the signal).
    RealType signal_peak_amplitude{1};

    // Signal to noise ratio measured in dB.
    RealType snr_db{0};
  };

  AWGNNoiseInjector() = default;

  explicit AWGNNoiseInjector(const Options& options) { Configure(options); }

  inline void Configure(const Options& options) {
    // For a Gaussian random variable X, the average power E[X²], also known as
    // the second moment, is
    //
    //   E[X²] = μ² + σ²
    //
    // So for white noise, μ=0 and the average power is then equal to the
    // variance σ².

    const RealType noise_db = options.signal_db - options.snr_db;
    const RealType noise_watts = Pow(RealType(10), noise_db / 10);

    const RealType sigma = Sqrt(noise_watts);

    distribution_ = NormalDistribution<RealType>(0, sigma);

    normalize_factor_ =
        RealType(1) / (options.signal_peak_amplitude + 3 * sigma);
  }

  // TODO(sergey): Support injecting noise to complex signals.
  inline auto operator()(const RealType sample) -> RealType {
    const RealType noise = distribution_(generator_);
    return (sample + noise) * normalize_factor_;
  }

 private:
  std::mt19937 generator_{5489};
  NormalDistribution<RealType> distribution_;

  // Factor to multiply the calculated noise sample to normalize it to the
  // [-1 .. 11] range.
  RealType normalize_factor_ = 1.0f;
};

}  // namespace radio_core::signal
