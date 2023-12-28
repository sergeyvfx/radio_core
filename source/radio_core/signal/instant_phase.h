// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// This processor receives analytical signal as a complex numbers and calculates
// an instantaneous phase of the signal.

#pragma once

#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"

namespace radio_core::signal {

template <class RealType>
class InstantPhase {
 public:
  inline auto operator()(const BaseComplex<RealType>& sample) -> RealType {
    // TODO(sergey): Consider adding some threshold so that sample with very low
    // magnitude does not cause numerical issues.

    return ArcTan2(sample.imag, sample.real);
  }
};

}  // namespace radio_core::signal
