// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <span>

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
    const float x0 = x[0];
    for (size_t i = 0; i < half; ++i) {
      x[i] = x[i + half + 1];
      x[i + half + 1] = x[i + 1];
    }
    x[half] = x0;
  }
}

// Normalize FFT values and shift the zero-frequency component to the center of
// the spectrum.
// This is similar to using FFTShift() with a separate normalization step, but
// is supposed to be more memory friendly due to a single pass over the memory.
template <class T>
inline void FFTNormalizeAndShift(std::span<T> x) {
  const size_t num_points = x.size();

  if (!num_points) {
    return;
  }

  const size_t half = num_points / 2;
  const T norm_fac = T(1) / num_points;

  if ((num_points & 1) == 0) {
    for (size_t i = 0; i < half; ++i) {
      T tmp = x[i] * norm_fac;
      x[i] = x[i + half] * norm_fac;
      x[i + half] = tmp;
    }
  } else {
    const T x0 = x[0];
    for (size_t i = 0; i < half; ++i) {
      x[i] = x[i + half + 1] * norm_fac;
      x[i + half + 1] = x[i + 1] * norm_fac;
    }
    x[half] = x0 * norm_fac;
  }
}

}  // namespace radio_core
