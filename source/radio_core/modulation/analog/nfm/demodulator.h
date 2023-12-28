// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Narrowband frequency demodulation implementation.
//
// Demodulates input complex IQ samples into audio floating point samples.
// Supports both per-sample operation, and demodulation of multiple samples in
// a buffer.

#pragma once

#include "radio_core/modulation/analog/fm/base_mono_demodulator.h"

namespace radio_core::modulation::analog::nfm {

template <class T>
class Demodulator : public fm::BaseMonoDemodulator<T> {
 public:
  struct Options : public fm::BaseMonoDemodulator<T>::Options {};

  using fm::BaseMonoDemodulator<T>::BaseMonoDemodulator;
};

}  // namespace radio_core::modulation::analog::nfm
