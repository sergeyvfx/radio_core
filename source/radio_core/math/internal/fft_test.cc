// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/fft.h"

#include <array>
#include <vector>

#include "radio_core/math/complex.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::FloatNear;
using testing::Pointwise;

TEST(math, FFTShift) {
  // >>> import numpy as np
  // >>> np.fft.fftshift([1])
  // array([1])
  {
    std::vector<float> fft{1};
    FFTShift(std::span<float>(fft));

    EXPECT_THAT(fft, Pointwise(FloatNear(1e-6f), std::to_array<float>({1})));
  }

  // >>> import numpy as np
  // >>> np.fft.fftshift([1, 2])
  // array([2, 1])
  {
    std::vector<float> fft{1, 2};
    FFTShift(std::span<float>(fft));

    EXPECT_THAT(fft, Pointwise(FloatNear(1e-6f), std::to_array<float>({2, 1})));
  }

  // >>> import numpy as np
  // >>> np.fft.fftshift([1, 2, 3])
  // array([3, 1, 2])
  {
    std::vector<float> fft{1, 2, 3};
    FFTShift(std::span<float>(fft));

    EXPECT_THAT(fft,
                Pointwise(FloatNear(1e-6f), std::to_array<float>({3, 1, 2})));
  }

  // >>> import numpy as np
  // >>> np.fft.fftshift([1, 2, 3, 4])
  // array([3, 4, 1, 2])
  {
    std::vector<float> fft{1, 2, 3, 4};
    FFTShift(std::span<float>(fft));

    EXPECT_THAT(
        fft, Pointwise(FloatNear(1e-6f), std::to_array<float>({3, 4, 1, 2})));
  }

  // >>> import numpy as np
  // >>> np.fft.fftshift([1, 2, 3, 4, 5])
  // array([4, 5, 1, 2, 3])
  {
    std::vector<float> fft{1, 2, 3, 4, 5};
    FFTShift(std::span<float>(fft));

    EXPECT_THAT(
        fft,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({4, 5, 1, 2, 3})));
  }

  // >>> import numpy as np
  // >>> np.fft.fftshift([1, 2, 3, 4, 5, 6])
  // array([4, 5, 6, 1, 2, 3])
  {
    std::vector<float> fft{1, 2, 3, 4, 5, 6};
    FFTShift(std::span<float>(fft));

    EXPECT_THAT(
        fft,
        Pointwise(FloatNear(1e-6f), std::to_array<float>({4, 5, 6, 1, 2, 3})));
  }

  // Complex samples.
  {
    std::vector<Complex> fft{
        Complex(1), Complex(2), Complex(3), Complex(4), Complex(5), Complex(6)};
    FFTShift(std::span<Complex>(fft));

    EXPECT_THAT(fft,
                Pointwise(ComplexNear(1e-6f),
                          std::to_array<Complex>({Complex(4.0f),
                                                  Complex(5.0f),
                                                  Complex(6.0f),
                                                  Complex(1.0f),
                                                  Complex(2.0f),
                                                  Complex(3.0f)})));
  }
}

TEST(math, FFTNormalizeAndShift) {
  {
    std::vector<float> fft{1};
    FFTNormalizeAndShift(std::span<float>(fft));

    EXPECT_THAT(fft, Pointwise(FloatNear(1e-6f), std::to_array<float>({1})));
  }

  {
    std::vector<float> fft{1, 2};
    FFTNormalizeAndShift(std::span<float>(fft));

    EXPECT_THAT(fft,
                Pointwise(FloatNear(1e-6f),
                          std::to_array<float>({2.0f / 2, 1.0f / 2})));
  }

  {
    std::vector<float> fft{1, 2, 3};
    FFTNormalizeAndShift(std::span<float>(fft));

    EXPECT_THAT(
        fft,
        Pointwise(FloatNear(1e-6f),
                  std::to_array<float>({3.0f / 3, 1.0f / 3, 2.0f / 3})));
  }

  {
    std::vector<float> fft{1, 2, 3, 4};
    FFTNormalizeAndShift(std::span<float>(fft));

    EXPECT_THAT(fft,
                Pointwise(FloatNear(1e-6f),
                          std::to_array<float>(
                              {3.0f / 4, 4.0f / 4, 1.0f / 4, 2.0f / 4})));
  }

  {
    std::vector<float> fft{1, 2, 3, 4, 5};
    FFTNormalizeAndShift(std::span<float>(fft));

    EXPECT_THAT(
        fft,
        Pointwise(FloatNear(1e-6f),
                  std::to_array<float>(
                      {4.0f / 5, 5.0f / 5, 1.0f / 5, 2.0f / 5, 3.0f / 5})));
  }

  {
    std::vector<float> fft{1, 2, 3, 4, 5, 6};
    FFTNormalizeAndShift(std::span<float>(fft));

    EXPECT_THAT(
        fft,
        Pointwise(
            FloatNear(1e-6f),
            std::to_array<float>(
                {4.0f / 6, 5.0f / 6, 6.0f / 6, 1.0f / 6, 2.0f / 6, 3.0f / 6})));
  }

  // Complex samples.
  {
    std::vector<Complex> fft{
        Complex(1), Complex(2), Complex(3), Complex(4), Complex(5), Complex(6)};
    FFTNormalizeAndShift(std::span<Complex>(fft));

    EXPECT_THAT(fft,
                Pointwise(ComplexNear(1e-6f),
                          std::to_array<Complex>({Complex(4.0f / 6),
                                                  Complex(5.0f / 6),
                                                  Complex(6.0f / 6),
                                                  Complex(1.0f / 6),
                                                  Complex(2.0f / 6),
                                                  Complex(3.0f / 6)})));
  }
}

}  // namespace radio_core
