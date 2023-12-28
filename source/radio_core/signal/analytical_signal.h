// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Discrete hilbert machine which calculates analytical signal for its input.
//
//
//            ┌─────────────────────╖  ┌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌┐
//          ↗ │        Delay        ║ →┆   Real part    ┆
// ┌╌╌╌╌╌╌╌┐  ╘═════════════════════╝  └╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌┘
// ┆ Input ┆
// └╌╌╌╌╌╌╌┘  ┌─────────────────────╖  ┌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌┐
//          ↘ │ Hilbert transformer ║ →┆ Imaginary part ┆
//            ╘═════════════════════╝  └╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌┘

#pragma once

#include <memory>

#include "radio_core/math/complex.h"
#include "radio_core/signal/filter.h"
#include "radio_core/signal/hilbert.h"
#include "radio_core/signal/integer_delay.h"
#include "radio_core/signal/simple_fir_filter.h"

namespace radio_core::signal {

template <class RealType, template <class> class Allocator = std::allocator>
class AnalyticalSignal {
 public:
  using Complex = BaseComplex<RealType>;

  AnalyticalSignal() = default;

  // Design the transformer kernel using given number of taps and window
  // equation.
  template <class WindowPredicateType>
  inline void Design(const size_t num_taps,
                     const WindowPredicateType& window_equation) {
    delay_.SetDelay(FilterDelayInSamples(num_taps));

    hilbert_transformer_.SetKernelSize(num_taps);
    GenerateWindowedHilbertTransformer(hilbert_transformer_.GetKernel(),
                                       window_equation);
  }

  auto operator()(const RealType sample) -> Complex {
    Complex result;
    result.real = delay_(sample);
    result.imag = hilbert_transformer_(sample);
    return result;
  }

 private:
  signal::SimpleFIRFilter<RealType, RealType, Allocator> hilbert_transformer_;
  signal::IntegerDelay<RealType, Allocator<RealType>> delay_;
};

}  // namespace radio_core::signal
