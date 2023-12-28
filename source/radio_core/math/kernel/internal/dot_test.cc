// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/kernel/dot.h"

#include <array>

#include "radio_core/base/half.h"
#include "radio_core/math/complex.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

#if RADIO_CORE_HAVE_HALF
#  include "radio_core/math/half_complex.h"
#endif

namespace radio_core {

using testing::ComplexNear;

// Common generator.
//
// NOTE: Re-initialize the `prng` before every suit.
//
// >>> import numpy
// >>> from numpy.random import RandomState
// >>> # The number of samples to give every vectorized loop 2 iterations.
// >>> num_samples = 32+32 + 16+16 + 8+8 + 4+4 + 3+3
// >>> prng = RandomState(0xdeadbeef)

template <class Float>
struct FloatFloatData {
  // >>> a = prng.randint(0, 7, num_samples)
  static inline const auto a = std::to_array<Float>(
      {5, 0, 6, 1, 4, 1, 2, 1, 2, 5, 2, 5, 1, 1, 4, 4, 2, 1, 1, 0, 3,
       2, 5, 4, 1, 0, 2, 2, 0, 0, 3, 4, 0, 4, 4, 0, 2, 1, 5, 1, 2, 3,
       1, 1, 3, 6, 1, 2, 3, 0, 1, 3, 4, 4, 6, 3, 3, 1, 5, 5, 2, 2, 5,
       6, 3, 4, 2, 1, 1, 3, 4, 5, 0, 3, 2, 1, 5, 2, 4, 5, 1, 3, 0, 0,
       2, 2, 4, 2, 1, 5, 4, 6, 4, 4, 5, 1, 4, 2, 0, 4, 5, 2, 3, 4, 0,
       6, 6, 5, 1, 6, 2, 6, 6, 5, 1, 2, 0, 0, 6, 3, 6, 5, 3, 4, 2, 2});

  // >>> b = prng.randint(0, 7, num_samples)
  static inline const auto b = std::to_array<Float>(
      {4, 6, 4, 4, 1, 6, 5, 0, 4, 4, 2, 3, 5, 4, 3, 6, 4, 2, 3, 1, 3,
       4, 1, 5, 2, 3, 3, 0, 1, 3, 2, 4, 5, 3, 3, 6, 5, 3, 6, 3, 1, 0,
       5, 5, 3, 1, 3, 1, 1, 6, 0, 4, 1, 5, 6, 6, 2, 4, 6, 5, 0, 1, 5,
       0, 4, 4, 2, 1, 1, 2, 6, 2, 6, 5, 3, 0, 5, 6, 6, 5, 3, 6, 5, 1,
       0, 3, 1, 6, 0, 4, 5, 3, 1, 5, 6, 1, 6, 0, 1, 4, 5, 6, 0, 0, 2,
       1, 0, 4, 4, 0, 2, 6, 1, 0, 5, 4, 4, 3, 2, 4, 6, 1, 5, 3, 6, 6});

  // >>> numpy.dot(a, b)
  static inline const Float dot{1151};
};

template <class Float>
struct ComplexFloatData {
  // >>> a = (prng.randint(0, 7, num_samples) +
  // ...      prng.randint(0, 7, num_samples) * 1j)
  // >>> for x in a: print(f"{{{x.real}, {x.imag}}},")
  static inline const auto a = std::to_array<BaseComplex<Float>>({
      {5.0, 4.0}, {0.0, 6.0}, {6.0, 4.0}, {1.0, 4.0}, {4.0, 1.0}, {1.0, 6.0},
      {2.0, 5.0}, {1.0, 0.0}, {2.0, 4.0}, {5.0, 4.0}, {2.0, 2.0}, {5.0, 3.0},
      {1.0, 5.0}, {1.0, 4.0}, {4.0, 3.0}, {4.0, 6.0}, {2.0, 4.0}, {1.0, 2.0},
      {1.0, 3.0}, {0.0, 1.0}, {3.0, 3.0}, {2.0, 4.0}, {5.0, 1.0}, {4.0, 5.0},
      {1.0, 2.0}, {0.0, 3.0}, {2.0, 3.0}, {2.0, 0.0}, {0.0, 1.0}, {0.0, 3.0},
      {3.0, 2.0}, {4.0, 4.0}, {0.0, 5.0}, {4.0, 3.0}, {4.0, 3.0}, {0.0, 6.0},
      {2.0, 5.0}, {1.0, 3.0}, {5.0, 6.0}, {1.0, 3.0}, {2.0, 1.0}, {3.0, 0.0},
      {1.0, 5.0}, {1.0, 5.0}, {3.0, 3.0}, {6.0, 1.0}, {1.0, 3.0}, {2.0, 1.0},
      {3.0, 1.0}, {0.0, 6.0}, {1.0, 0.0}, {3.0, 4.0}, {4.0, 1.0}, {4.0, 5.0},
      {6.0, 6.0}, {3.0, 6.0}, {3.0, 2.0}, {1.0, 4.0}, {5.0, 6.0}, {5.0, 5.0},
      {2.0, 0.0}, {2.0, 1.0}, {5.0, 5.0}, {6.0, 0.0}, {3.0, 4.0}, {4.0, 4.0},
      {2.0, 2.0}, {1.0, 1.0}, {1.0, 1.0}, {3.0, 2.0}, {4.0, 6.0}, {5.0, 2.0},
      {0.0, 6.0}, {3.0, 5.0}, {2.0, 3.0}, {1.0, 0.0}, {5.0, 5.0}, {2.0, 6.0},
      {4.0, 6.0}, {5.0, 5.0}, {1.0, 3.0}, {3.0, 6.0}, {0.0, 5.0}, {0.0, 1.0},
      {2.0, 0.0}, {2.0, 3.0}, {4.0, 1.0}, {2.0, 6.0}, {1.0, 0.0}, {5.0, 4.0},
      {4.0, 5.0}, {6.0, 3.0}, {4.0, 1.0}, {4.0, 5.0}, {5.0, 6.0}, {1.0, 1.0},
      {4.0, 6.0}, {2.0, 0.0}, {0.0, 1.0}, {4.0, 4.0}, {5.0, 5.0}, {2.0, 6.0},
      {3.0, 0.0}, {4.0, 0.0}, {0.0, 2.0}, {6.0, 1.0}, {6.0, 0.0}, {5.0, 4.0},
      {1.0, 4.0}, {6.0, 0.0}, {2.0, 2.0}, {6.0, 6.0}, {6.0, 1.0}, {5.0, 0.0},
      {1.0, 5.0}, {2.0, 4.0}, {0.0, 4.0}, {0.0, 3.0}, {6.0, 2.0}, {3.0, 4.0},
      {6.0, 6.0}, {5.0, 1.0}, {3.0, 5.0}, {4.0, 3.0}, {2.0, 6.0}, {2.0, 6.0},
  });

  // >>> b = prng.randint(0, 7, num_samples)
  static inline const auto b = std::to_array<Float>(
      {1, 1, 3, 5, 0, 1, 2, 4, 5, 3, 1, 2, 1, 5, 5, 6, 5, 1, 6, 4, 2,
       0, 5, 6, 3, 2, 0, 2, 5, 2, 2, 4, 0, 1, 1, 1, 0, 0, 0, 4, 6, 6,
       4, 2, 3, 4, 0, 4, 2, 0, 6, 1, 0, 2, 1, 5, 0, 1, 5, 3, 3, 2, 1,
       6, 3, 2, 6, 6, 0, 0, 6, 3, 2, 2, 5, 6, 6, 2, 5, 0, 3, 3, 6, 5,
       3, 5, 6, 1, 6, 0, 1, 0, 4, 1, 4, 0, 4, 5, 1, 6, 4, 4, 1, 4, 4,
       5, 2, 1, 3, 3, 0, 1, 3, 2, 1, 1, 4, 1, 3, 5, 2, 1, 3, 3, 6, 1});

  // >>> numpy.dot(a, b)
  static inline const BaseComplex<Float> dot{991, 1067};
};

TEST(Dot, float_float) {
  const float dot =
      Dot<float, float>(FloatFloatData<float>::a, FloatFloatData<float>::b);

  EXPECT_NEAR(dot, FloatFloatData<float>::dot, 1e-6f);
}

TEST(Dot, complex_float) {
  const Complex dot = Dot<Complex, float>(ComplexFloatData<float>::a,
                                          ComplexFloatData<float>::b);

  EXPECT_THAT(dot, ComplexNear(ComplexFloatData<float>::dot, 1e-6f));
}

#if RADIO_CORE_HAVE_HALF

TEST(Dot, half_half) {
  const Half dot =
      Dot<Half, Half>(FloatFloatData<Half>::a, FloatFloatData<Half>::b);

  EXPECT_NEAR(float(dot), float(FloatFloatData<Half>::dot), 1e-6f);
}

TEST(Dot, half_complex_half) {
  const HalfComplex dot = Dot<HalfComplex, Half>(ComplexFloatData<Half>::a,
                                                 ComplexFloatData<Half>::b);

  EXPECT_THAT(Complex(float(dot.real), float(dot.imag)),
              ComplexNear(ComplexFloatData<float>::dot, 1e-6f));
}

#endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core
