// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Calculate power spectral density of every complex sample point.
//
// The power is calculated as 10 * Log10(real^2 + imag^2).
//
// The output is to contain the same number of points as the input.
//
// More details: https://en.wikipedia.org/wiki/Spectral_density

#pragma once

#include <cassert>
#include <cmath>
#include <span>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/internal/power_spectral_density_vectorized.h"
#include "radio_core/math/math.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::kernel {

// The output buffer must have at least same number of elements as the input
// samples buffer. It is possible to have the output buffer bigger than input
// in which case the output buffer will only be partially written (only
// number of input samples will be written to the output).
//
// Returns subspan of the output buffer where values has actually been
// written.

inline auto PowerSpectralDensity(const std::span<const Complex> samples,
                                 const std::span<float> power)
    -> std::span<float> {
  return power_spectral_density_internal::Kernel<float, true>::Execute(samples,
                                                                       power);
}

#if RADIO_CORE_HAVE_HALF

inline auto PowerSpectralDensity(const std::span<const HalfComplex> samples,
                                 const std::span<Half> power)
    -> std::span<Half> {
  return power_spectral_density_internal::Kernel<Half, true>::Execute(samples,
                                                                      power);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel
