// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/fast_arg.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core {

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

TEST(FastArg, Complex) {
  std::array<Complex, 6> samples = {{
      Complex(1.0f, 0.0f),
      Complex(0.0f, 1.0f),
      Complex(-3.0f, 2.0),
      Complex(3.0f, -2.0f),
      Complex(1.0f, 1.0f),
      Complex(1.0f, -1.0f),
  }};

  std::array<float, 6> arg_values;
  kernel::FastArg<float>(samples, arg_values);
  EXPECT_THAT(arg_values,
              Pointwise(FloatNear(2e-2f),
                        std::array<float, 6>({{
                            0.0f,
                            1.57079637f,
                            2.55359006f,
                            -0.58800262f,
                            0.785398f,
                            -0.785398f,
                        }})));
}

#if RADIO_CORE_HAVE_HALF

TEST(FastArg, HalfComplex) {
  std::array<HalfComplex, 6> samples = {{
      HalfComplex(1.0f, 0.0f),
      HalfComplex(0.0f, 1.0f),
      HalfComplex(-3.0f, 2.0),
      HalfComplex(3.0f, -2.0f),
      HalfComplex(1.0f, 1.0f),
      HalfComplex(1.0f, -1.0f),
  }};

  std::array<Half, 6> arg_values;
  kernel::FastArg<Half>(samples, arg_values);

  std::array<float, 6> arg_values_values;
  for (int i = 0; i < arg_values.size(); ++i) {
    arg_values_values[i] = float(arg_values[i]);
  }

  EXPECT_THAT(arg_values_values,
              Pointwise(FloatNear(2e-2f),
                        std::array<float, 6>({{
                            0.0f,
                            1.57079637f,
                            2.55359006f,
                            -0.58800262f,
                            0.785398f,
                            -0.785398f,
                        }})));
}
#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
