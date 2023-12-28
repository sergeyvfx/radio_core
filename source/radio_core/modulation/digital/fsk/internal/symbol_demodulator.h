// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Demodulator of a single symbol of FSK modulation.
//
// Generalized internal network:
//
//   ┌╌╌╌╌╌╌╌┐   ┌───────────╖   ┌────────────╖   ┌╌╌╌╌╌╌╌╌┐
//   ┆ Input ┆ → │  Symbol   ║ → │ Deemphasis ║ → ┆ Output ┆
//   └╌╌╌╌╌╌╌┘   │ Amplitude ║   ╘════════════╝   └╌╌╌╌╌╌╌╌┘
//               ╘═══════════╝
//
// Input
// =====
//
// The input is (optionally prefiltered) input signal from a receiver.
//
//
// Output
// ======
//
// The output is de-emphasized magnitude of the symbol the modulator is
// configured for. The outputs of 2 symbol demodulator chains are suitable for
// comparison: the stronger output is the currently receiving symbol.
//
//
// Symbol Amplitude
// ================
//
// A symbol amplitude is calculated as a magnitude of a complex value after
// quadrature down-conversion:
//
//                   ┌───┐   ┌─────╖
//       ┌─────────→ │ × │ → │ LPF ║ ─────────────────────┐
//       │           └───┘   ╘═════╝                      │
//       │             ↑                                  ↓
//   ┌╌╌╌╌╌╌╌┐   ┌────────────╖   ┌───────────────┐  ┌─────────╖   ┌╌╌╌╌╌╌╌╌╌╌╌┐
//   ┆ Input ┆   │ Quadrature ║ ← │ Cos(2*pi*f*t) │  │  Abs()  ║ → ┆  Symbol   ┆
//   └╌╌╌╌╌╌╌┘   │  Network   ║   └───────────────┘  ╘═════════╝   ┆ Amplitude ┆
//       │       ╘════════════╝                           ↑        └╌╌╌╌╌╌╌╌╌╌╌┘
//       │             ↓                                  │
//       │           ┌───┐   ┌─────╖                      │
//       └─────────→ │ × │ → │ LPF ║ ─────────────────────┘
//                   └───┘   ╘═════╝
//
// It is very similar to an arctangent demodulator network, described, i.e, in
//
//   Low-SNR Operation of FSK Demodulators
//   Armin Šabanović
//   https://repository.tudelft.nl/islandora/object/uuid%3A98a156a1-3899-4d7c-86cd-dc223b73ab40
//
//
// It might not be the best choice for low-SNR environment applications, but it
// is very simple to implement and to intuitively understand in a context of
// SDR project.
//
// In practice using 2 "Abs()" networks instead of classical arctangent one
// lead to better performance in noisy environment.
//
// It also showed better results when demodulating Track 01 of TNC Test CD than
// Frequency Discriminator (also described by Armin Šabanović). However, it
// showed worse results on the Track 02. Since this demodulator is aimed to be
// used by SDR which do not perform de-emphasis the quadrature down-conversion
// is the way to go until alternatives shows better results on Track 01.
//
//
// Deemphasis
// ==========
//
// It is unknown whether transmitter emphasized the transmission prio sending it
// so it is possible that one of the symbol will be received with an intrinsicly
// lower amplitude. This is described in the
//
//   Setting Your TNC's Audio Drive Level. Why it's important, and how to do it
//   John Ackermann N8UR
//
//   https://www.febo.com/packet/layer-one/transmit.html
//
// This demodulator uses idea from John Langner of using AGC. There is some
// explanation of the idea written up in the:
//
//   Building a Better Demodulator for APRS / AX.25 Packet Radio
//   Part 1, 1200 Baud AFSK
//   John Langner, WB2OSZ
//
//   https://github.com/wb2osz/direwolf/blob/master/doc/A-Better-APRS-Packet-Demodulator-Part-1-1200-baud.pdf
//
// The paper does not contain much details, but intuitively the AGC should have
// fast charge and slow discharge rate, so that when a symbol appears in the
// decoding stream its demodulated amplitude gets quickly normalized, but when
// the data stream does not contain that symbol for a while the demodulator does
// not normalize noise floor to what could be perceived as a symbol.
//
// This approach could fail in a specific cases when one of the symbols is not
// transmitted for a while, however this is not the case in practice. The coding
// of a signal is such that it is possible to recover the clock from the data
// stream. This is achieved by a choice of a coding scheme (such as NRZ) and by
// bit staffing (like in HDLC).

#pragma once

#include <memory>

#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"
#include "radio_core/signal/ema_agc.h"
#include "radio_core/signal/filter.h"
#include "radio_core/signal/filter_window_heuristic.h"
#include "radio_core/signal/local_oscillator.h"
#include "radio_core/signal/root_raised_cosine.h"
#include "radio_core/signal/simple_fir_filter.h"

namespace radio_core::modulation::digital::fsk::internal {

template <class RealType, template <class> class Allocator = std::allocator>
class SymbolDemodulator {
 public:
  // High-level options describing type of the symbol this demodulators handles.
  // The demodulator will try to automatically match its internal filter
  // configuration to have good performance.
  struct Options {
    // Frequency of a tone of this symbol.
    RealType tone_frequency{0};

    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate{0};

    // Baud rate: symbols per second in the data stream.
    int data_baud{0};

    // Fine-tuned configuration of stages of the demodulator.

    // Configuration of RRC filter.
    //
    // The transition bandwidth is provided in Hz and defines the order of the
    // filter. It is measured in hertz.
    //
    // The beta the roll-off factor.
    RealType rrc_filter_transition_bandwidth{291};
    RealType rrc_beta{0.19};

    // Deemphasis AGC configuration.
    RealType agc_charge_rate{0.6};
    RealType agc_discharge_rate{0.0005};
  };

  SymbolDemodulator() = default;

  explicit SymbolDemodulator(const Options& options) { Configure(options); }

  void Configure(const Options& options) {
    // NOTE: The hard-coded constants corresponds to configuration which was
    // empirically found to be the best for sample rate 11025 and baud rate
    // 1200.

    // Configure the local oscillator.
    local_oscillator_.Configure(options.tone_frequency, options.sample_rate);

    // Configure the low-pass filter

    const int num_taps =
        signal::EstimateFilterSizeForTransitionBandwidth(
            options.rrc_filter_transition_bandwidth, options.sample_rate) |
        1;

    // Samples per symbol for the RRC filter.
    // Sometimes it is also called the oversampling factor.
    const RealType samples_per_symbol = options.sample_rate / options.data_baud;

    low_pass_filter_.SetKernelSize(num_taps);
    signal::DesignLowpassRRCFilter(
        low_pass_filter_.GetKernel(), samples_per_symbol, options.rrc_beta);

    // Configure the AGC.
    //
    // Intuitively the weights are to be scaled for different sample rates.
    // But in practice these values works good for both 11025 and 44100 sample
    // rates.
    agc_.Configure(options.agc_charge_rate, options.agc_discharge_rate);
  }

  // Process (possibly pre-filtered) sample of the input signal.
  // Outputs magnitude of the demodulated symbol.
  auto operator()(const RealType sample) -> RealType {
    const IQComplex iq = sample * local_oscillator_.IQ();
    const IQComplex iq_filtered = low_pass_filter_(iq);

    const RealType symbol_amplitude = Abs(iq_filtered);

    return agc_(symbol_amplitude);
  }

 private:
  using IQLocalOscillator = signal::LocalOscillator<RealType>;
  using IQComplex = BaseComplex<RealType>;
  using IQFilter = signal::SimpleFIRFilter<IQComplex, RealType, Allocator>;

  IQLocalOscillator local_oscillator_;
  IQFilter low_pass_filter_;
  signal::EMAAGC<RealType> agc_;
};

}  // namespace radio_core::modulation::digital::fsk::internal
