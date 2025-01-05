// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

// Base class for FFT engines.
//
// This class defines the API which is expected to be implemented by various
// DSP routines. It also provides common building blocks of the FFT calculation
// that is not directly related to the FFT algorithm itself: for example
// windowing.
//
// The class is templated by the element of the input signal. For example,
// FFT<float> calculates FFT of a real signal, FFT<Complex> calculates FFT of a
// complex signal.
//
// The allocator which is provided to the FFT API is to allocate memory with the
// same requirement w.r.t alignment as the underlying implementation expects it
// to be.

#pragma once

#include <span>

#include "radio_core/base/aligned_allocator.h"
#include "radio_core/math/base_complex.h"

namespace radio_core::fft {

// Aligned allocator with the most common alignment used by FFT libraries.
template <class T>
using FFTAllocator = AlignedAllocator<T, 16>;

namespace fft_internal {

// Normalize the output by multiplying all elements by 1/num_points.
// TODO(sergey): Use vectorized kernel to perform the multiplication.
template <class T>
void Normalize(const std::span<T> output, const std::size_t num_points) {
  const T norm_fac = T(1) / num_points;
  for (T& x : output) {
    x *= norm_fac;
  }
}
template <class T>
void Normalize(const std::span<BaseComplex<T>> output,
               const std::size_t num_points) {
  const T norm_fac = T(1) / num_points;
  for (BaseComplex<T>& x : output) {
    x *= norm_fac;
  }
}

// Base class for all FFT API specialization.
//
// Defines all the common bits for which it is only needed to know the RealType.
// The RealType is the same as the element type for FFT which operates on real
// signal, and it is BaseComplex<T>::value_type for FFT which operates on
// complex signal.
template <class RealType, template <class> class Allocator = FFTAllocator>
class BaseFFT {
 public:
  // Options that define the FFT algorithm which stay invariant across different
  // transform calls.
  struct SetupOptions {
    // The number of points for which FFT will be calculated.
    int num_points{1024};
  };

  virtual ~BaseFFT() = default;

  // Configure the FFT.
  virtual void Configure(const SetupOptions& options) = 0;

 protected:
  BaseFFT() = default;
};

}  // namespace fft_internal

// Specialization of the FFT API for the real input type.
template <class T, template <class> class Allocator = FFTAllocator>
class FFT : public fft_internal::BaseFFT<T, Allocator> {
 public:
  // Options that affect the way how transform is calculated that do not require
  // the FFT reconfiguration.
  struct TransformOptions {
    // Normalize the output by multiplying every element by 1/N where M is the
    // number of FFT points.
    bool normalize{false};
  };

  // Perform forward FFT of the given input.
  // The output must be at least the input.size()/2+1.
  // Returns the subspan of the output which is sized to the exact size of the
  // calculated FFT.
  virtual auto Forward(std::span<const T> input,
                       std::span<BaseComplex<T>> output,
                       const TransformOptions& options = TransformOptions())
      -> std::span<BaseComplex<T>> = 0;

 protected:
  FFT() = default;
};

// Specialization of the FFT API for the complex input type.
template <class T, template <class> class Allocator>
class FFT<BaseComplex<T>, Allocator>
    : public fft_internal::BaseFFT<T, Allocator> {
 public:
  // Options that affect the way how transform is calculated that do not require
  // the FFT reconfiguration.
  struct TransformOptions {
    // Normalize the output by multiplying every element by 1/N where M is the
    // number of FFT points.
    bool normalize{false};

    // Apply FFTShift() on the output to make it so DC is at the center of the
    // output.
    bool shift{false};
  };

  // Perform forward FFT of the given input.
  // The output must be at least the size of the input. The input and output
  // might match, but not alias.
  // Returns the subspan of the output which is sized to the exact size of the
  // calculated FFT.
  virtual auto Forward(std::span<const BaseComplex<T>> input,
                       std::span<BaseComplex<T>> output,
                       const TransformOptions& options = TransformOptions())
      -> std::span<BaseComplex<T>> = 0;

 protected:
  FFT() = default;
};

}  // namespace radio_core::fft
