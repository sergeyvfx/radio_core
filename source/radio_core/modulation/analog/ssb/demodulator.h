// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Base logic implementing the Single Side Band demodulator.
// Demodulates input complex IQ samples into audio floating point samples.
//
// Supports both per-sample operation, and demodulation of multiple samples in
// a buffer.
//
// The implementation is baaed on the phasing method of demodulation described
// in the articles linked below. It is simplified and does not contain all the
// blocks as those calculation stages are expected to be handled by the signal
// path in the typical usage of the demodulator.
//
// Namely, there is no IQ conversion done, as the input samples are already in
// their quadrature form. Another missing block in the implementation is the
// low-pass filter. THe article [1] mention is is there to reduce artifacts
// caused by down-conversion and decimation. Those stages are taken care by the
// signal path, and the signal path already has such filters in place.
//
// Effectively, this leaves the implementation to just a hilbert transformer in
// the Q signal path, and a delay in the I signal path.
//
// References:
//
//   [1] Understanding the 'Phasing Method' of Single Sideband Demodulation
//       https://www.dsprelated.com/showarticle/176.php
//
//   [2] SSB Demodulation
//       https://panoradio-sdr.de/ssb-demodulation/

#pragma once

#include <memory>
#include <span>

#include "radio_core/modulation/analog/iq_demodulator.h"

#include "radio_core/signal/filter.h"
#include "radio_core/signal/hilbert.h"
#include "radio_core/signal/integer_delay.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::modulation::analog::ssb {

enum class Sideband {
  kUpper,
  kLower,
};

template <class T,
          Sideband kSideband,
          template <class> class Allocator = std::allocator>
class Demodulator : public IQDemodulator<T> {
 public:
  struct Options : public IQDemodulator<T>::Options {};

  Demodulator() = default;
  explicit Demodulator(const Options& options) { Configure(options); }

  void Configure(const Options& /*options*/) {
    // TODO(sergey): The value is obtained by running demodulator on 240000
    // sample rate where it gives good results. Need to investigate how it
    // behaves on other sample rates, and, maybe, make it configurable via the
    // options.
    const size_t num_hilbert_taps = 121;

    delay_.SetDelay(signal::FilterDelayInSamples(num_hilbert_taps));

    hilbert_transformer_.SetKernelSize(num_hilbert_taps);
    GenerateWindowedHilbertTransformer(
        hilbert_transformer_.GetKernel(),
        signal::WindowEquation<T, signal::Window::kHamming>());
  }

  inline auto operator()(const BaseComplex<T> sample) -> T override {
    // Implementation follows the diagram and description of the A DSP SSB
    // demodulator [1].
    //
    // The first multiplication stage by a quadrature signal is not performed
    // here as the samples are already provided in the quadrature form. However,
    // seems that typical SDR radio gives quadrature signal by multiplying the
    // analog signal by cos(w*n*t) and sin(w*n*t), and the demodulator expects
    // it to be -sin(w*n*t), hence the negation of the Q (imaginary part) of
    // the input sample.

    const T i = delay_(sample.real);
    const T q = hilbert_transformer_(-sample.imag);

    if constexpr (kSideband == Sideband::kUpper) {
      return i - q;
    }

    if constexpr (kSideband == Sideband::kLower) {
      return i + q;
    }
  }

  inline auto operator()(const std::span<const BaseComplex<T>> input_samples,
                         const std::span<T> output_samples)
      -> std::span<T> override {
    assert(input_samples.size() <= output_samples.size());

    const size_t num_samples = input_samples.size();
    for (size_t i = 0; i < num_samples; ++i) {
      output_samples[i] = (*this)(input_samples[i]);
    }

    return output_samples.subspan(0, num_samples);
  }

 private:
  signal::IntegerDelay<T, Allocator<T>> delay_;
  signal::SimpleFIRFilter<T, T, Allocator> hilbert_transformer_;
};

}  // namespace radio_core::modulation::analog::ssb
