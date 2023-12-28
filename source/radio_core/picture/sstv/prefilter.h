// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Prefilter of amplitude samples into frequency samples accepted by the SSTV
// decoder pipeline.
//
// The internal network:
//
//   ┌╌╌╌╌╌╌╌┐   ┌──────────╖   ┌───────────╖   ┌───────────╖   ┌╌╌╌╌╌╌╌╌┐
//   ┆       ┆   │          ║   │ Hilbert   ║   │ Frequency ║   ┆        ┆
//   ┆ Input ┆ → │ Bandpass ║ → │ Transform ║ → │ Filter    ║ → ┆ Output ┆
//   ┆       ┆   │          ║   │           ║   │           ║   ┆        ┆
//   └╌╌╌╌╌╌╌┘   ╘══════════╝   ╘═══════════╝   ╘═══════════╝   └╌╌╌╌╌╌╌╌┘

#pragma once

#include <memory>

#include "radio_core/picture/sstv/mode_limits.h"
#include "radio_core/signal/analytical_signal.h"
#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/filter_window_heuristic.h"
#include "radio_core/signal/frequency.h"
#include "radio_core/signal/instant_frequency.h"
#include "radio_core/signal/instant_phase.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::picture::sstv {

template <class RealType, template <class> class Allocator = std::allocator>
class Prefilter {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate;

    // Fine-tuned parameters.
    //
    // They are obtained empirically by tweaking kernel sized and optimizing
    // for the number of decoded VIS digital codes.
    //
    // TODO(sergey): Look into making a more well-defined way to check quality
    // of the decoder.

    // Configuration of the pre-filter which filters input samples.
    //
    // The transition bandwidth is provided in Hz and defines the order of the
    // filter.
    //
    // The frequency extent is measure in Hz from the both sides of the min/max
    // of the symbol frequencies: the band is extended by this many hertz from
    // upper and lower sides.
    RealType prefilter_transition_bandwidth_hz{412};
    RealType prefilter_frequency_extent{50};

    // Configuration of the hilbert transformer of the analytical signal
    // calculator.
    // The attenuation and transition frequency is used to calculate alpha and
    // dw parameters of the Kaiser window used by the transformer.
    RealType hilbert_attenuation{90};
    RealType hilbert_transition_frequency{ModeLimits::kMaxLUMAFrequency + 100};

    // Low-pass filter configuration for the frequency.
    //
    // Defines how fast the frequency is allowed to change. Helps dealing with
    // noise but reducing the transients, but also smears pixels if the value is
    // too low.
    RealType frequency_filter_transition_bandwidth{334};
    RealType frequency_filter_cutoff{1200};
  };

  inline void Configure(const Options& options) {
    ConfigurePrefilter(options);
    ConfigureAnalyticalSignal(options);
    ConfigureFrequencyFilter(options);

    instant_frequency_.SetSampleRate(options.sample_rate);
  }

  auto operator()(const RealType sample) -> RealType {
    // Prefilter the sample.
    const RealType clean_sample = prefilter_(sample);

    // Convert to analytical signal and calculate instantaneous frequency.
    const BaseComplex<RealType> analytic_sample =
        analytical_signal_(clean_sample);
    const RealType instant_phase = instant_phase_(analytic_sample);
    const RealType instant_frequency = instant_frequency_(instant_phase);

    // Low-pass the frequency to avoid ringing on frequency transition which is
    // especially noticeable on low sample rates.
    const RealType clean_instant_frequency =
        frequency_filter_(instant_frequency);

    return clean_instant_frequency;
  }

 private:
  inline void ConfigurePrefilter(const Options& options) {
    // TODO(sergey): Investigate whether pre-filtering using a specific set of
    // frequencies gives an advantage over a single bandpass.

    const RealType min_image_frequency{
        ModeLimits::kFrequencyInterval.lower_bound};
    const RealType max_image_frequency{
        ModeLimits::kFrequencyInterval.upper_bound};

    const int prefilter_num_taps =
        signal::EstimateFilterSizeForTransitionBandwidth(
            options.prefilter_transition_bandwidth_hz, options.sample_rate) |
        1;

    prefilter_.SetKernelSize(prefilter_num_taps);

    signal::DesignBandPassFilter(
        prefilter_.GetKernel(),
        signal::WindowEquation<RealType, signal::Window::kHamming>(),
        min_image_frequency - options.prefilter_frequency_extent,
        max_image_frequency + options.prefilter_frequency_extent,
        options.sample_rate);
  }

  inline void ConfigureAnalyticalSignal(const Options& options) {
    const RealType alpha = options.hilbert_attenuation;
    const RealType beta = signal::CalculateKaiserBeta(alpha);
    const RealType dw = signal::NormalizedAngularFrequency(
        options.hilbert_transition_frequency, options.sample_rate);

    const int kaiser_size = signal::CalculateKaiserSize(alpha, dw) | 1;

    analytical_signal_.Design(
        kaiser_size,
        signal::WindowEquation<RealType, signal::Window::kKaiser>(beta));
  }

  inline void ConfigureFrequencyFilter(const Options& options) {
    const int filter_num_taps =
        signal::EstimateFilterSizeForTransitionBandwidth(
            options.frequency_filter_transition_bandwidth,
            options.sample_rate) |
        1;

    frequency_filter_.SetKernelSize(filter_num_taps);

    signal::DesignLowPassFilter(
        frequency_filter_.GetKernel(),
        signal::WindowEquation<RealType, signal::Window::kHamming>(),
        options.frequency_filter_cutoff,
        options.sample_rate);
  }

  signal::SimpleFIRFilter<RealType, RealType, Allocator> prefilter_;
  signal::AnalyticalSignal<RealType, Allocator> analytical_signal_;
  signal::InstantPhase<RealType> instant_phase_;
  signal::InstantFrequency<RealType> instant_frequency_;
  signal::SimpleFIRFilter<RealType, RealType, Allocator> frequency_filter_;
};

}  // namespace radio_core::picture::sstv
