// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for demodulators which use complex IQ samples as input, and
// produce floating point sample for every input IQ sample.

#pragma once

#include "radio_core/math/complex.h"

namespace radio_core::modulation::analog {

template <class T>
class IQDemodulator {
 public:
  struct Options {};

  IQDemodulator() = default;
  explicit IQDemodulator(const Options& /*options*/) {}

  virtual ~IQDemodulator() = default;

  // Demodulate single input IQ sample.
  // Returns demodulated value.
  virtual auto operator()(BaseComplex<T> sample) -> T = 0;

  // Demodulate multiple input samples and write the, to the output buffer.
  //
  // The output buffer must have at least same number of elements as the input
  // samples buffer. It is possible to have the output buffer bigger than input
  // in which case the output buffer will only be partially written (only
  // number of input samples will be written to the output).
  //
  // Returns subspan of the output buffer where samples has actually been
  // written.
  virtual auto operator()(std::span<const BaseComplex<T>> input_samples,
                          std::span<T> output_samples) -> std::span<T> = 0;
};

}  // namespace radio_core::modulation::analog
