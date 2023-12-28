// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/math/math.h"

namespace radio_core {

// Calculate the zeroth-order modified Bessel function of the first kind.
template <class T>
inline auto ModifiedBesselI0(T x) -> T {
  // This is a naive implementation which comes from the C++ reference:
  //
  //   https://en.cppreference.com/w/cpp/numeric/special_functions/cyl_bessel_i
  //
  // It is not fast and is likely not very precise. There doesn't seem to be
  // a small and portable drop-in replacement. Possibilities are:
  //
  //  - Wait for macOS to catch up and implement cyl_bessel_i in their libc and
  //    use `std::cyl_bessel_i()`.
  //
  //  - Use `boost::math::cyl_bessel_i`. It seems that the minimal subset of
  //    Boost will be about 1.6 MiB to have the `cyl_bessel_i` implementation.
  //
  // While the Boost implementation gives about 5 extra decimal digits of
  // accuracy of this function it does not have big affect on the accuracy of
  // Kaiser window calculation: even with more accurate Bessel function it is
  // currently not possible to get the calculated window closer to SciPy values.
  // Point is: is not immediately obvious that pulling Boost or other bigger
  // library will have measurable impact on DSP aspects.

  T fct = 1;
  T sum = 0;
  for (int k = 0; k < 10; fct *= ++k) {
    sum += Pow((x / 2), T(2 * k)) / Pow(fct, T(2));
  }
  return sum;
}

}  // namespace radio_core
