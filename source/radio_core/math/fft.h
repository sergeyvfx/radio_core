// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <span>

#include "radio_core/math/base_complex.h"

namespace radio_core {

// Shift the zero-frequency component to the center of the spectrum.
//
// This function swaps half-spaces for all axes listed (defaults to all). Note
// that y[0] is the Nyquist component only if len(x) is even.
//
// This is an equivalent of `numpy.fft.fftshift`.
template <class T>
inline void FFTShift(std::span<T> x) {
  const size_t num_points = x.size();

  if (!num_points) {
    return;
  }

  const size_t half = num_points / 2;

  if ((num_points & 1) == 0) {
    for (size_t i = 0; i < half; ++i) {
      std::swap(x[i], x[i + half]);
    }
  } else {
    const T x0 = x[0];
    for (size_t i = 0; i < half; ++i) {
      x[i] = x[i + half + 1];
      x[i + half + 1] = x[i + 1];
    }
    x[half] = x0;
  }
}

namespace fft_internal {

// An implementation of public FFTNormalizeAndShift().
template <class SampleType, class RealType>
inline void FFTNormalizeAndShift(const std::span<SampleType> x) {
  const size_t num_points = x.size();

  if (!num_points) {
    return;
  }

  const size_t half = num_points / 2;
  const RealType norm_fac = RealType(1) / num_points;

  if ((num_points & 1) == 0) {
    for (size_t i = 0; i < half; ++i) {
      const SampleType tmp = x[i] * norm_fac;
      x[i] = x[i + half] * norm_fac;
      x[i + half] = tmp;
    }
  } else {
    const SampleType x0 = x[0];
    for (size_t i = 0; i < half; ++i) {
      x[i] = x[i + half + 1] * norm_fac;
      x[i + half + 1] = x[i + 1] * norm_fac;
    }
    x[half] = x0 * norm_fac;
  }
}

}  // namespace fft_internal

// Normalize FFT values and shift the zero-frequency component to the center of
// the spectrum.
//
// This is similar to using FFTShift() with a separate normalization step, but
// is supposed to be more memory friendly due to a single pass over the memory.
//
// The normalization happens by the factor of 1/x.size().
//
// This function is designed to be used on the result of FFT which transfotms
// complex input.
template <class T>
inline void FFTNormalizeAndShift(const std::span<T> x) {
  fft_internal::FFTNormalizeAndShift<T, T>(x);
}
template <class T>
inline void FFTNormalizeAndShift(const std::span<BaseComplex<T>> x) {
  fft_internal::FFTNormalizeAndShift<BaseComplex<T>, T>(x);
}

}  // namespace radio_core
