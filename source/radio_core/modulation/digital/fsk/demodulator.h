// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Demodulator of frequency-shift keying (FSK).
//
// Such modulation represents binary data with changes in frequency.
//
// This demodulator converts changes in frequency to binary data. The input of
// the demodulator is an analog signal in the time amplitude-time domain, the
// output is stream of binary bits.
//
// The implementation is a non-coherent demodulator with the clock recovery.
// It is based on ideas from various sources:
//
//   Digital Communication with Audio Frequency Shift Keying (AFSK)
//   https://inst.eecs.berkeley.edu/~ee123/sp17/lab/lab5/Lab5_Part_2-Audio_Frequency_Shift_Keying(AFSK).html
//
//   A High-Performance Sound-Card AX.25 Modem
//   Sivan Toledo, 4X6IZ
//   https://www.cs.tau.ac.il/~stoledo/Bib/Pubs/QEX-JulAug-2012.pdf
//
//   Low-SNR Operation of FSK Demodulators
//   Armin Šabanović
//   https://repository.tudelft.nl/islandora/object/uuid%3A98a156a1-3899-4d7c-86cd-dc223b73ab40

#pragma once

#include <functional>

#include "radio_core/base/algorithm.h"
#include "radio_core/base/result.h"
#include "radio_core/math/math.h"
#include "radio_core/modulation/digital/fsk/internal/symbol_demodulator.h"
#include "radio_core/modulation/digital/fsk/tones.h"
#include "radio_core/signal/digital_hysteresis.h"
#include "radio_core/signal/digital_pll.h"
#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/filter_window_heuristic.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::modulation::digital::fsk {

template <class RealType, template <class> class Allocator = std::allocator>
class Demodulator {
 public:
  struct Options {
    // Tones of modulated mark and space symbols.
    // Expected to have matched duration between the mark and the space.
    Tones tones{0, 0};

    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate{0};

    // Baud rate: symbols per second in the data stream.
    int data_baud{0};

    // Fine-tuned configuration of stages of the demodulator.
    //
    // The values are obtained by manual optimization of the maximum number of
    // messages decoded from TNC Test CD. Since the demodulator is intended to
    // be used in an SDR with pretty much direct input from the IF the Track 1
    // is preferred over Track 2.
    //
    // TODO(sergey): Find a way to de-duplicate default values with the symbol
    // demodulator.

    // Configuration of the pre-filter which filters input samples.
    //
    // The transition bandwidth is provided in Hz and defines the order of the
    // filter. It is measured in hertz.
    //
    // The frequency extent is measure in Hz from the both sides of the min/max
    // of the symbol frequencies: the band is extended by this many hertz from
    // upper and lower sides.
    RealType prefilter_transition_bandwidth{70};
    RealType prefilter_frequency_extent{190};

    // Configuration of RRC filter in symbol demodulation chain.
    //
    // The transition bandwidth is provided in Hz and defines the order of the
    // filter. It is measured in hertz.
    //
    // The beta the roll-off factor.
    RealType symbol_rrc_filter_transition_bandwidth_{291};
    RealType symbol_rrc_beta{0.19};

    // AGC configuration used for symbol deemphasis.
    RealType symbol_agc_charge_rate{0.6};
    RealType symbol_agc_discharge_rate{0.0005};

    // Hysteresis threshold which is used on a difference between mark and
    // space magnitudes. Avoids ringing issues.
    RealType hysteresis_threshold{0.02};

    // PLL configuration.
    // The PLL is used for the clock recovery.
    RealType pll_inertia{0.75};
  };

  // Error code for result.
  enum class Error {
    // Signal sample has been processed, but no bit is decoded yet.
    //
    // This code does not indicate a processing, it just  indicates that more
    // samples are needed to decode bit.
    kUnavailable,
  };

  using Result = radio_core::Result<bool, Error>;

  Demodulator() = default;

  explicit constexpr Demodulator(const Options& options) { Configure(options); }

  constexpr void Configure(const Options& options) {
    // Minimum and maximum frequencies of symbol tones.
    const auto min_symbol_frequency =
        RealType(Min(options.tones.mark, options.tones.space));
    const auto max_symbol_frequency =
        RealType(Max(options.tones.mark, options.tones.space));

    // Configure prefilter.

    const int prefilter_num_taps =
        signal::EstimateFilterSizeForTransitionBandwidth(
            options.prefilter_transition_bandwidth, options.sample_rate) |
        1;

    prefilter_.SetKernelSize(prefilter_num_taps);

    signal::DesignBandPassFilter(
        prefilter_.GetKernel(),
        signal::WindowEquation<RealType, signal::Window::kBoxcar>(),
        min_symbol_frequency - options.prefilter_frequency_extent,
        max_symbol_frequency + options.prefilter_frequency_extent,
        options.sample_rate);

    // Configure symbols.

    using SymbolDemodulator = internal::SymbolDemodulator<RealType, Allocator>;

    typename SymbolDemodulator::Options symbol_options;
    symbol_options.sample_rate = options.sample_rate;
    symbol_options.data_baud = options.data_baud;
    symbol_options.rrc_filter_transition_bandwidth =
        options.symbol_rrc_filter_transition_bandwidth_;
    symbol_options.rrc_beta = options.symbol_rrc_beta;
    symbol_options.agc_charge_rate = options.symbol_agc_charge_rate;
    symbol_options.agc_discharge_rate = options.symbol_agc_discharge_rate;

    typename SymbolDemodulator::Options mark_options = symbol_options;
    mark_options.tone_frequency = RealType(options.tones.mark);
    mark_demodulator_.Configure(mark_options);

    typename SymbolDemodulator::Options space_options = symbol_options;
    space_options.tone_frequency = RealType(options.tones.space);
    space_demodulator_.Configure(space_options);

    // Configure hysteresis.
    hysteresis_.SetThreshold(0, options.hysteresis_threshold);

    // Configure PLL.

    typename signal::DigitalPLL<RealType>::Options pll_options;
    pll_options.data_baud = options.data_baud;
    pll_options.sample_rate = options.sample_rate;
    pll_options.inertia = options.pll_inertia;

    pll_.Configure(pll_options);
  }

  // Process sample of an input signal.
  //
  // Returns value of a newly demodulated bit when it is available.
  // Otherwise returns an error code.
  auto operator()(const RealType sample) -> Result {
    const RealType prefiltered_sample = prefilter_(sample);

    const RealType mark_amplitude = mark_demodulator_(prefiltered_sample);
    const RealType space_amplitude = space_demodulator_(prefiltered_sample);

    const RealType demodulated_sample = mark_amplitude - space_amplitude;
    const bool demodulated_bit = hysteresis_(demodulated_sample);

    if (pll_(demodulated_bit)) {
      return Result(demodulated_bit);
    }

    return Result(Error::kUnavailable);
  }

  // Process sample of an input signal, and invoke the callback with the
  // demodulated bit.
  //
  // The given list of args... is passed to the callback first. This makes the
  // required callback signature to be:
  //
  //   callback(<optional arguments>, const bool demodulated_bit)
  template <class F, class... Args>
  void operator()(const RealType sample, F&& callback, Args&&... args) {
    const Result result = (*this)(sample);

    if (result.Ok()) {
      std::invoke(std::forward<F>(callback),
                  std::forward<Args>(args)...,
                  result.GetValue());
    }
  }

 private:
  signal::SimpleFIRFilter<RealType, RealType, Allocator> prefilter_;

  internal::SymbolDemodulator<RealType, Allocator> mark_demodulator_;
  internal::SymbolDemodulator<RealType, Allocator> space_demodulator_;

  signal::DigitalHysteresis<RealType> hysteresis_;
  signal::DigitalPLL<RealType> pll_;
};

}  // namespace radio_core::modulation::digital::fsk
