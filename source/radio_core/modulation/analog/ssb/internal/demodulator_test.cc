// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/modulation/analog/ssb/demodulator.h"

#include "radio_core/signal/local_oscillator.h"

namespace radio_core::modulation::analog::ssb {

// TODO(sergey): Find a way to test the demodulator without making the test
// generation too complicated or hard to justify improvements.
//
// Something like DSP SSB Generation from the [1] could work, but it is unclear
// how to verify the generation provides clean tone.
//
// References:
//
//   [1] Understanding the 'Phasing Method' of Single Sideband Demodulation
//       https://www.dsprelated.com/showarticle/176.php

}  // namespace radio_core::modulation::analog::ssb
