// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/power_spectral_density.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core {

TEST(PerPointLerpPeakDetector, Float) {
  std::array<Complex, 5> samples = {{Complex(2, 3),
                                     Complex(4, 5),
                                     Complex(6, 7),
                                     Complex(8, 9),
                                     Complex(10, 11)}};
  std::array<float, 5> power;

  PowerSpectralDensity(samples, power);

  // Vectorized log10 implementation might not allow better precision than this.
  EXPECT_NEAR(power[0], 11.1394335f, 1e-5f);
  EXPECT_NEAR(power[1], 16.12784f, 1e-5f);
  EXPECT_NEAR(power[2], 19.2941914f, 1e-5f);
  EXPECT_NEAR(power[3], 21.6136818f, 1e-5f);
  EXPECT_NEAR(power[4], 23.443924f, 1e-5f);
}

#if RADIO_CORE_HAVE_HALF

TEST(PerPointLerpPeakDetector, Half) {
  std::array<HalfComplex, 10> samples = {{HalfComplex(2, 3),
                                          HalfComplex(4, 5),
                                          HalfComplex(6, 7),
                                          HalfComplex(8, 9),
                                          HalfComplex(10, 11),
                                          HalfComplex(2, 3),
                                          HalfComplex(4, 5),
                                          HalfComplex(6, 7),
                                          HalfComplex(8, 9),
                                          HalfComplex(10, 11)}};
  std::array<Half, 10> power;

  PowerSpectralDensity(samples, power);

  // Vectorized log10 implementation might not allow better precision than this.
  EXPECT_NEAR(float(power[0]), 11.1394335f, 2e-2f);
  EXPECT_NEAR(float(power[1]), 16.12784f, 2e-2f);
  EXPECT_NEAR(float(power[2]), 19.2941914f, 2e-2f);
  EXPECT_NEAR(float(power[3]), 21.6136818f, 2e-2f);
  EXPECT_NEAR(float(power[4]), 23.443924f, 2e-2f);
  EXPECT_NEAR(float(power[5]), 11.1394335f, 2e-2f);
  EXPECT_NEAR(float(power[6]), 16.12784f, 2e-2f);
  EXPECT_NEAR(float(power[7]), 19.2941914f, 2e-2f);
  EXPECT_NEAR(float(power[8]), 21.6136818f, 2e-2f);
  EXPECT_NEAR(float(power[9]), 23.443924f, 2e-2f);
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
