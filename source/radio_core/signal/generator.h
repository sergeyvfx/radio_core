// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Frequency generator which generates samples of given frequency with given
// duration and writes them into the provided processor.
//
// Example based on generating leader tones of SSTV VIS transmission:
//
//    Generator::Options options;
//    options.sample_rate = 22050;
//
//    Generator generator(options);
//
//    generator.pushSample(FrequencyDuration(1900, 300.0f));
//    generator.pushSample(FrequencyDuration(1200, 10.0f));
//    generator.pushSample(FrequencyDuration(1900, 300.0f));

#pragma once

#include <cassert>
#include <functional>

#include "radio_core/base/constants.h"
#include "radio_core/base/frequency_duration.h"
#include "radio_core/math/math.h"

namespace radio_core::signal {

template <class RealType>
class Generator {
 public:
  Generator() = default;

  explicit Generator(const RealType sample_rate) { Configure(sample_rate); }

  inline void Configure(const RealType sample_rate) {
    assert(sample_rate > 0);

    sample_rate_ = sample_rate;
    sample_rate_inv_ = RealType(1) / sample_rate_;
  }

  // Generate amplitude samples for the given frequency and its duration.
  //
  // The generated samples are passed to the callback, one per the callback
  // invocation. The given list of args... is passed to the callback after the
  // sample. This makes the required callback signature to be:
  //
  //   callback(RealType sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const FrequencyDuration<RealType>& frequency_duration,
                  F&& callback,
                  Args&&... args) {
    constexpr RealType kPi = constants::pi_v<RealType>;

    assert(sample_rate_inv_ > 0);
    assert(frequency_duration.frequency >= 0);
    assert(frequency_duration.duration_ms >= 0);

    // Duration of single amplitude sample in milliseconds.
    const RealType amplitude_sample_duration_in_ms = 1000 * sample_rate_inv_;

    // Advance of the phase per one amplitude sample.
    const RealType phase_advance_per_sample =
        2 * kPi * RealType(frequency_duration.frequency) * sample_rate_inv_;

    // For the very first frequency sample shift last phase back, so that the
    // next point after it lands at phase of 0.
    if (!has_phase_) {
      prev_phase_ = -phase_advance_per_sample;
      has_phase_ = true;
    }

    RealType last_phase = prev_phase_;
    for (size_t index = 0;; ++index) {
      // Time within the frequency sample.
      const RealType time_ms =
          time_offset_ms_ + index * amplitude_sample_duration_in_ms;

      if (time_ms > frequency_duration.duration_ms) {
        // Overshoot happened. Stop writing samples and store how much time we
        // need to compensate for at the next invocation of the `pushSample()`.
        time_offset_ms_ = time_ms - frequency_duration.duration_ms;
        break;
      }

      RealType phase = prev_phase_ + (index + 1) * phase_advance_per_sample;

      // Wrap the phase, to keep it small, avoiding possible numeric issues.
      phase = Modulo(phase, 2 * kPi);

      const RealType amplitude_sample = Sin(phase);
      last_phase = phase;

      std::invoke(std::forward<F>(callback),
                  amplitude_sample,
                  std::forward<Args>(args)...);

      if (time_ms == frequency_duration.duration_ms) {
        // Sample is written at the exact moment. Set offset to 0 and finish
        // writing.
        time_offset_ms_ = 0.0f;
        break;
      }
    }

    prev_phase_ = last_phase;
    previous_frequency_ = RealType(frequency_duration.frequency);
  }

  // Fade the output of the generator to 0.
  //
  // Uses frequency of the latest pushed sample and extrapolates generation
  // until a zero output sample is written. Will write at maximum
  // `sample_rate/2` samples.
  //
  // The generated samples are passed to the callback, one per the callback
  // invocation. The given list of args... is passed to the callback after the
  // sample. This makes the required callback signature to be:
  //
  //   callback(RealType sample, <optional arguments>)
  template <class F, class... Args>
  void FadeToZero(F&& callback, Args&&... args) {
    constexpr RealType kPi = constants::pi_v<RealType>;

    assert(sample_rate_inv_ > 0);

    const RealType phase_advance_per_sample =
        2 * kPi * previous_frequency_ * sample_rate_inv_;

    RealType last_amplitude_sample = Sin(prev_phase_);

    // Check whether output already stopped at the zero value.
    if (Abs(last_amplitude_sample) < RealType(1e-6)) {
      return;
    }

    for (size_t index = 1; index <= sample_rate_; ++index) {
      RealType phase = prev_phase_ + (index + 1) * phase_advance_per_sample;

      // Wrap the phase, to keep it small, avoiding possible numeric issues.
      phase = Modulo(phase, 2 * kPi);

      const RealType amplitude_sample = Sin(phase);

      if (index && last_amplitude_sample * amplitude_sample < 0.0f) {
        std::invoke(std::forward<F>(callback),
                    RealType(0),
                    std::forward<Args>(args)...);
        break;
      }

      std::invoke(std::forward<F>(callback),
                  amplitude_sample,
                  std::forward<Args>(args)...);

      if (Abs(amplitude_sample) < RealType(1e-6)) {
        break;
      }

      last_amplitude_sample = amplitude_sample;
    }

    // Assume that if new frequency samples will be pushed to this generator
    // they are not expected to be in phase with the signal prior to fading to
    // zero. This allows to easily push a delay as a frequency of 0 after the
    // fade.gen
    prev_phase_ = 0;
  }

 private:
  RealType sample_rate_{0};

  // Inverse value of sample rate. In other words, duration of one amplitude
  // sample in seconds.
  RealType sample_rate_inv_{0};

  // Phase at which the previous `pushSample()` left the signal.
  //
  // This phase will be used by a consecutive call to `pushSample()` in order
  // to keep signal as continuous as possible (without doing filtering).
  RealType prev_phase_{0};

  // Indicates whether the previous phase is known.
  // It is unknown for until after first call of `pushSample()`. Can not rely
  // on the default value because the `pushSample()` needs to actually know
  // previous phase.
  bool has_phase_ = false;

  // How much time the previous sample "overshoot".
  // Timing of samples is not necessarily aligned with the sample rate, which
  // means some samples will be written a bit too late. This value denotes how
  // much late previous sample was written, so that the next one can
  // compensate for it and keep overall timing accurate.
  //
  // Measured in milliseconds.
  RealType time_offset_ms_{0};

  RealType previous_frequency_{0};
};

}  // namespace radio_core::signal
