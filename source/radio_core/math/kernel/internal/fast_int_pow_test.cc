// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/fast_int_pow.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core {

using testing::ComplexNear;
using testing::Pointwise;

TEST(FastIntPow, Complex) {
  const auto base = std::to_array<Complex>({
      Complex(0.0f, 0.0f),
      Complex(0.1f, 0.2f),
      Complex(-0.3f, 0.4f),
      Complex(-0.4f, 0.5f),
      Complex(0.1f, -0.2f),
      Complex(-0.3f, -0.4f),
      Complex(-0.4f, -0.5f),
      Complex(0.5f, -0.6f),
      Complex(0.0f, 0.0f),
      Complex(0.1f, 0.2f),
  });

  std::array<Complex, 10> pow_values;
  kernel::FastIntPow<Complex>(base, 4, pow_values);
  EXPECT_THAT(pow_values,
              Pointwise(ComplexNear(1e-6f),
                        std::to_array<Complex>({
                            Complex(0.0f, 0.0f),
                            Complex(-0.0007f, -0.0024f),
                            Complex(-0.0527f, 0.0336f),
                            Complex(-0.1519f, 0.072f),
                            Complex(-0.0007f, 0.0024f),
                            Complex(-0.0527f, -0.0336f),
                            Complex(-0.1519f, -0.072f),
                            Complex(-0.3479, 0.132f),
                            Complex(0.0f, 0.0f),
                            Complex(-0.0007f, -0.0024f),
                        })));
}

#if RADIO_CORE_HAVE_HALF

TEST(FastIntPow, HalfComplex) {
  const auto base = std::to_array<HalfComplex>({
      HalfComplex(0.0f, 0.0f),
      HalfComplex(0.1f, 0.2f),
      HalfComplex(-0.3f, 0.4f),
      HalfComplex(-0.4f, 0.5f),
      HalfComplex(0.1f, -0.2f),
      HalfComplex(-0.3f, -0.4f),
      HalfComplex(-0.4f, -0.5f),
      HalfComplex(0.5f, -0.6f),
      HalfComplex(0.0f, 0.0f),
      HalfComplex(0.1f, 0.2f),
  });

  std::array<HalfComplex, 10> pow_values;
  kernel::FastIntPow<HalfComplex>(base, 4, pow_values);
  EXPECT_THAT(pow_values,
              Pointwise(ComplexNear(1e-3f),
                        std::to_array<HalfComplex>({
                            HalfComplex(0.0f, 0.0f),
                            HalfComplex(-0.0007f, -0.0024f),
                            HalfComplex(-0.0527f, 0.0336f),
                            HalfComplex(-0.1519f, 0.072f),
                            HalfComplex(-0.0007f, 0.0024f),
                            HalfComplex(-0.0527f, -0.0336f),
                            HalfComplex(-0.1519f, -0.072f),
                            HalfComplex(-0.3479, 0.132f),
                            HalfComplex(0.0f, 0.0f),
                            HalfComplex(-0.0007f, -0.0024f),
                        })));
}
#endif

}  // namespace radio_core
