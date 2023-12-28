// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/rotator.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core::signal {

using testing::ComplexNear;

TEST(Rotator, Complex) {
  std::array<Complex, 40> samples{};
  for (int i = 0; i < samples.size(); ++i) {
    samples[i].real = Cos(0.1f * float(i));
    samples[i].imag = Sin(0.1f * float(i));
  }

  Complex phase(1.0f, 0.0f);
  Rotator<float>(samples, phase, Complex(Cos(-0.1f), Sin(-0.1f)), samples);

  for (Complex& sample : samples) {
    EXPECT_THAT(sample, ComplexNear(Complex(1, 0.0f), 1e-6f));
  }
}

#if RADIO_CORE_HAVE_HALF

TEST(Rotator, HalfComplex) {
  std::array<HalfComplex, 40> samples{};
  for (int i = 0; i < samples.size(); ++i) {
    samples[i].real = Cos(Half(0.1f * float(i)));
    samples[i].imag = Sin(Half(0.1f * float(i)));
  }

  HalfComplex phase(1.0f, 0.0f);
  Rotator<Half>(samples, phase, HalfComplex(Cos(-0.1f), Sin(-0.1f)), samples);

  for (HalfComplex& sample : samples) {
    const Complex complex_sample(float(sample.real), float(sample.imag));
    EXPECT_THAT(complex_sample, ComplexNear(Complex(1, 0.0f), 6e-3f));
  }
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::signal
