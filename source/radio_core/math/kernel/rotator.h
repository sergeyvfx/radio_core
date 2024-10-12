// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Rotate input vector at a fixed rate per sample from the initial phase offset.
//
// For example, if the input quadrature signal oscillates with frequency 100 Hz
// applying a rotator of 200 Hz will make the output to oscillate at 300 Hz.

#pragma once

#include <span>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/kernel/internal/rotator_vectorized.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::kernel {

// Rotate input samples at a fixed rate per sample, staring from the given
// phase.
//
// The nw phase is stored in the argument, and the result samples are written
// to the given output buffer.
//
// The output buffer must have at least same number of elements as the input
// samples buffer. It is possible to have the output buffer bigger than input
// in which case the output buffer will only be partially written (only
// number of input samples will be written to the output).
//
// Returns subspan of the output buffer where values has actually been
// written.
template <class T>
inline auto Rotator(const std::span<const BaseComplex<T>> samples,
                    BaseComplex<T>& phase,
                    const BaseComplex<T> phase_increment_per_sample,
                    const std::span<BaseComplex<T>> output)
    -> std::span<BaseComplex<T>> {
  assert(samples.size() <= output.size());

  const size_t num_samples = samples.size();

  const BaseComplex<T>* samples_ptr = samples.data();
  BaseComplex<T>* output_ptr = output.data();

  const BaseComplex<T>* samples_end = samples_ptr + num_samples;

  while (samples_ptr < samples_end) {
    *output_ptr = *samples_ptr * phase;

    // e^(j*w(t+1)) = e^(j*w(t)) * e^(j*w)
    phase *= phase_increment_per_sample;

    ++samples_ptr;
    ++output_ptr;
  }

  phase /= Abs(phase);

  return output.subspan(0, num_samples);
}

// Specialization for rotation of single floating point precision complex
// values.
template <>
inline auto Rotator(const std::span<const Complex> samples,
                    Complex& phase,
                    const Complex phase_increment_per_sample,
                    const std::span<Complex> output) -> std::span<Complex> {
  return rotator_internal::Kernel<float, true>::Execute(
      samples, phase, phase_increment_per_sample, output);
}

#if RADIO_CORE_HAVE_HALF

// Specialization for rotation of half floating point precision complex values.
template <>
inline auto Rotator(const std::span<const HalfComplex> samples,
                    HalfComplex& phase,
                    const HalfComplex phase_increment_per_sample,
                    const std::span<HalfComplex> output)
    -> std::span<HalfComplex> {
  return rotator_internal::Kernel<Half, true>::Execute(
      samples, phase, phase_increment_per_sample, output);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel
