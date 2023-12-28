// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Filter window.
//
// Useful links:
//
//   FIR Filters by Windowing
//   http://www.labbookpages.co.uk/audio/firWindowing.html
//
//   Window function
//   https://wikipedia.org/wiki/Window_function

#pragma once

#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/math/bessel.h"
#include "radio_core/math/math.h"

namespace radio_core::signal {

enum class Window {
  // Also known as rectangular or Dirichlet.
  // In SciPy terms is `scipy.signal.windows.boxcar`.
  kBoxcar,

  // Also known as Bartlett or Fejér.
  // In SciPy terms is `scipy.signal.windows.bartlett`.
  kTriangular,

  // In SciPy terms is `scipy.signal.windows.hann`.
  kHann,

  // Original hamming with alpha = 0.54
  // In SciPy terms is `scipy.signal.windows.hamming`.
  kHamming,

  // Optimal (in the equiripple sense) hamming with alpha = 0.53836.
  //
  //   https://wikipedia.org/wiki/Window_function#Hann_and_Hamming_windows
  //
  // In SciPy terms is `scipy.signal.windows.general_hamming` with
  // alpha = 0.53836.
  kOptimalHamming,

  // In SciPy terms is `scipy.signal.windows.blackman`,
  kBlackman,

  // NOTE: uses the SciPy's formula, which is different from the Wikipedia.
  // In SciPy terms is `scipy.signal.windows.cosine`.
  kCosine,

  // In SciPy terms is `scipy.signal.windows.kaiser`.
  kKaiser,
};

// Implementation of various window equations.
//
// Implemented as a templated functors, allowing to construct object with a
// desired parameters and pass it down the filter construction functionality
// and get maximum benefit of optimization.
//
// Filter order is equal to the number of taps minus 1 (filter length - 1).
// In formulas often called M.

template <class T, Window kWindow>
class WindowEquation;

// The code follows variable name notation from handbooks, which do not follow
// the code style conventions.

template <class T>
class WindowEquation<T, Window::kBoxcar> {
 public:
  inline auto operator()(const int /*order*/, const int /*n*/) const -> T {
    return T(1);
  }
};

template <class T>
class WindowEquation<T, Window::kTriangular> {
 public:
  inline auto operator()(const int order, const int n) const -> T {
    return T(1) - T(2) * Abs(n - T(order) / 2) / order;
  }
};

template <class T>
class WindowEquation<T, Window::kHann> {
 public:
  inline auto operator()(const int order, const int n) const -> T {
    const T s = Sin(T(constants::pi) * T(n) / order);
    return s * s;
  }
};

template <class T>
class WindowEquation<T, Window::kOptimalHamming> {
 public:
  inline auto operator()(const int order, const int n) const -> T {
    static constexpr T kA0{0.53836};
    static constexpr T kA1{0.46164};

    return kA0 - kA1 * Cos(2 * T(constants::pi) * T(n) / order);
  }
};

template <class T>
class WindowEquation<T, Window::kHamming> {
 public:
  inline auto operator()(const int order, const int n) const -> T {
    static constexpr T kA0{0.54f};
    static constexpr T kA1{0.46f};

    return kA0 - kA1 * Cos(2 * T(constants::pi) * T(n) / order);
  }
};

template <class T>
class WindowEquation<T, Window::kBlackman> {
 public:
  inline auto operator()(const int order, const int n) const -> T {
    // The exact Blackman approximation.

    // static constexpr T kA0{7938.0f / 18608.0f};
    // static constexpr T kA1{9240.0f / 18608.0f};
    // static constexpr T kA2{1430.0f / 18608.0f};

    static constexpr T kA0{0.42};
    static constexpr T kA1{0.5};
    static constexpr T kA2{0.08};

    const T n_order = T(n) / order;
    return kA0 - kA1 * Cos(2 * T(constants::pi) * n_order) +
           kA2 * Cos(4 * T(constants::pi) * n_order);
  }
};

template <class T>
class WindowEquation<T, Window::kCosine> {
 public:
  inline auto operator()(const int order, const int n) const -> T {
    return Sin(T(constants::pi) * (T(n) + T(0.5)) / (order + 1));
  }
};

template <class T>
class WindowEquation<T, Window::kKaiser> {
 public:
  constexpr explicit WindowEquation(const T beta) : beta_(beta) {
    i0_beta_inv_ = 1.0f / ModifiedBesselI0(beta_);
  }

  inline auto operator()(const int order, const int n) const -> T {
    const T half_order = order / T(2);
    const T a = (n - half_order) / half_order;

    return ModifiedBesselI0(beta_ * Sqrt(T(1) - a * a)) * i0_beta_inv_;
  }

 private:
  T beta_;
  T i0_beta_inv_;
};

// Generate kernel which consists of just a window equation.
template <class T, class WindowPredicateType>
inline void GenerateWindow(const std::span<T> window,
                           const WindowPredicateType& window_equation) {
  const size_t num_taps = window.size();
  const size_t M = num_taps - 1;
  for (size_t n = 0; n <= M; ++n) {
    window[n] = window_equation(M, n);
  }
}

}  // namespace radio_core::signal
