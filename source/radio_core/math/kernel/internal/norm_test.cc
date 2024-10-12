// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/norm.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

TEST(Norm, Complex) {
  const auto samples = std::to_array<Complex>({
      Complex(0),
      Complex(0.1f),
      Complex(0, 0.1f),
      Complex(2.0f, 3.0f),
      Complex(2.0f, -3.0f),
      Complex(-2.0f, 3.0f),
      Complex(0, 0.2f),
      Complex(0.3f, 0.0f),
      Complex(2.0f, -3.0f),
      Complex(-2.0f, 3.0f),
  });

  std::array<float, 10> abs_values;
  Norm<float>(samples, abs_values);
  EXPECT_THAT(abs_values,
              Pointwise(FloatNear(2e-2f),
                        std::to_array<float>({
                            0.0f,
                            0.01f,
                            0.01f,
                            13.0f,
                            13.0f,
                            13.0f,
                            0.04f,
                            0.09f,
                            13.0f,
                            13.0f,
                        })));
}

#if RADIO_CORE_HAVE_HALF

TEST(Norm, HalfComplex) {
  const auto samples = std::to_array<HalfComplex>({
      HalfComplex(0),
      HalfComplex(0.1f),
      HalfComplex(0, 0.1f),
      HalfComplex(2.0f, 3.0f),
      HalfComplex(2.0f, -3.0f),
      HalfComplex(-2.0f, 3.0f),
      HalfComplex(0, 0.2f),
      HalfComplex(0.3f, 0.0f),
      HalfComplex(2.0f, -3.0f),
      HalfComplex(-2.0f, 3.0f),
  });

  std::array<Half, 10> abs_values;
  Norm<Half>(samples, abs_values);

  std::array<float, 10> abs_float_values;
  for (int i = 0; i < abs_values.size(); ++i) {
    abs_float_values[i] = float(abs_values[i]);
  }

  EXPECT_THAT(abs_float_values,
              Pointwise(FloatNear(2e-2f),
                        std::to_array<float>({
                            0.0f,
                            0.01f,
                            0.01f,
                            13.0f,
                            13.0f,
                            13.0f,
                            0.04f,
                            0.09f,
                            13.0f,
                            13.0f,
                        })));
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
