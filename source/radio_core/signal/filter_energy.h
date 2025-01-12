// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <span>

#include "radio_core/math/math.h"

namespace radio_core::signal {

// Calculate energy of the given filter.
template <class T>
inline auto CalculateFilterEnergy(const std::span<const T> h) -> T {
  T energy(0);
  for (const T h_k : h) {
    energy += h_k * h_k;
  }
  return energy;
}

// Scale filter to have unity energy.
// This means that filter's impulse response is normalized such that it does not
// amplify or attenuate the signal.
template <class T>
inline void ScaleFilterToUnityEnergy(const std::span<T> h) {
  const T energy = CalculateFilterEnergy<T>(h);
  const T gain_scale_inv = T(1) / Sqrt(energy);
  for (T& h_k : h) {
    h_k *= gain_scale_inv;
  }
}

}  // namespace radio_core::signal
