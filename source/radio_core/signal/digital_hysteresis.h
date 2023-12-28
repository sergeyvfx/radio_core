// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Simple digital signal hysteresis implementation, similar to Schmitt trigger:
//
//   - If the input signal above the given window then true is returned.
//   - If the input signal below the given window then false is returned.
//   - Otherwise previous value is returned.
//
// The initial state is false.

#pragma once

#include "radio_core/signal/hysteresis.h"

namespace radio_core::signal {

template <class T>
class DigitalHysteresis {
 public:
  DigitalHysteresis() = default;

  DigitalHysteresis(const T level, const T window) {
    SetThreshold(level, window);
  }

  constexpr void SetThreshold(const T level, const T window) {
    hysteresis_.SetThreshold(level, window);
    level_ = level;
  }

  constexpr auto operator()(const T sample) -> bool {
    return hysteresis_(sample) > level_;
  }

 protected:
  Hysteresis<T> hysteresis_;
  T level_{0};
};

}  // namespace radio_core::signal
