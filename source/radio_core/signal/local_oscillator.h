// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Oscillator of a given frequency with given sample rate.
//
// Supports generation of phase, sine wave, cosine wave, and quadrature signal.
// All generators are sharing the same internal phase state, and every call to
// the signal generation advances the phase. In practice this means that the
// same oscillator can not be used to provide independent sine and cosine
// signals and separate local oscillators are to be used.
//
// The implementation is such that it minimizes run-away due to precision
// issues.

#pragma once

#include "radio_core/base/constants.h"
#include "radio_core/math/math.h"

namespace radio_core {

// Forward declaration of complex numbers.
//
// Avoids pulling many headers when the local oscillator is only needed for
// floating point signals.
template <class T>
class BaseComplex;

namespace signal {

template <class RealType>
class LocalOscillator {
 public:
  LocalOscillator() = default;

  constexpr LocalOscillator(const RealType frequency,
                            const RealType sample_rate) {
    Configure(frequency, sample_rate);
  }

  constexpr void Configure(const RealType frequency,
                           const RealType sample_rate) {
    phase_advance_per_sample_ =
        2 * constants::pi_v<RealType> * frequency / sample_rate;
  }

  // Offset the phase from the current state by the given value.
  // Phase is measured in the range of [0 .. 2*pi].
  inline void OffsetPhase(const RealType phase_offset) {
    start_phase_ += phase_offset;
    start_phase_ = Modulo(start_phase_, constants::pi_v<RealType>);
  }

  // Generate next value for phase.
  // Phase is measured in the range of [0 .. 2*pi].
  inline auto Phase() -> RealType {
    RealType phase = start_phase_ + index_ * phase_advance_per_sample_;

    if (phase > 2 * constants::pi_v<RealType>) {
      phase -= 2 * constants::pi_v<RealType>;
      start_phase_ = phase;
      index_ = 0;
    }

    ++index_;

    return phase;
  }

  // Generate next sample of sine or a cosine wave.
  inline auto Sine() -> RealType { return Sin(Phase()); }
  inline auto Cosine() -> RealType { return Cos(Phase()); }

  // Generate sample of a quadrature signal.
  //
  // The real part of the complex value corresponds to the in-phase signal,
  // the imaginary part corresponds to the quadrature signal.
  //
  // The output value rotates counter-clockwise with an increase of the phase.
  inline auto IQ() -> BaseComplex<RealType> {
    BaseComplex<RealType> iq;
    SinCos(Phase(), iq.imag, iq.real);
    return iq;
  }

 private:
  RealType phase_advance_per_sample_{0};

  int index_{0};
  RealType start_phase_{0};
};

}  // namespace signal
}  // namespace radio_core
