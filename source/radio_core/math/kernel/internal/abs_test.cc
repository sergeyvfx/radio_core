// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/abs.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

TEST(Abs, Generic) {
  const std::array<int, 10> samples = {{1, -2, 3, -4, 5, -6, 7, -8, 9, -10}};
  std::array<int, 10> abs_values;
  Abs<int, int>(samples, abs_values);
  EXPECT_THAT(
      abs_values,
      Pointwise(Eq(), std::to_array<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})));
}

TEST(Abs, Float) {
  const std::array<float, 10> samples = {{1, -2, 3, -4, 5, -6, 7, -8, 9, -10}};
  std::array<float, 10> abs_values;
  Abs<float, float>(samples, abs_values);
  EXPECT_THAT(abs_values,
              Pointwise(FloatNear(1e-6f),
                        std::to_array<float>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})));
}

TEST(Abs, Complex) {
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
  Abs<Complex, float>(samples, abs_values);
  EXPECT_THAT(abs_values,
              Pointwise(FloatNear(1e-6f),
                        std::to_array<float>({
                            0,
                            0.1f,
                            0.1f,
                            3.6055513f,
                            3.6055513f,
                            3.6055513f,
                            0.2f,
                            0.3f,
                            3.6055513f,
                            3.6055513f,
                        })));
}

#if RADIO_CORE_HAVE_HALF

TEST(Abs, Half) {
  const std::array<Half, 10> samples = {{1, -2, 3, -4, 5, -6, 7, -8, 9, -10}};
  std::array<Half, 10> abs_values;

  Abs<Half, Half>(samples, abs_values);

  std::array<float, 10> abs_float_values;
  for (int i = 0; i < abs_values.size(); ++i) {
    abs_float_values[i] = float(abs_values[i]);
  }

  EXPECT_THAT(abs_float_values,
              Pointwise(FloatNear(1e-6f),
                        std::to_array<float>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})));
}

TEST(Abs, HalfComplex) {
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
  Abs<HalfComplex, Half>(samples, abs_values);

  std::array<float, 10> abs_float_values;
  for (int i = 0; i < abs_values.size(); ++i) {
    abs_float_values[i] = float(abs_values[i]);
  }

  EXPECT_THAT(abs_float_values,
              Pointwise(FloatNear(1e-3f),
                        std::to_array<float>({
                            0,
                            0.1f,
                            0.1f,
                            3.6055513f,
                            3.6055513f,
                            3.6055513f,
                            0.2f,
                            0.3f,
                            3.6055513f,
                            3.6055513f,
                        })));
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
