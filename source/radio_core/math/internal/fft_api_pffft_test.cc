// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/fft_api_pffft.h"

#include "radio_core/math/complex.h"
#include "radio_core/math/fft.h"
#include "radio_core/math/internal/fft_test_data.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::fft {

using testing::ComplexNear;
using testing::Pointwise;

TEST(PFFFT, Real) {
  {
    PFFFT<float> complex_fft(PFFFT<float>::SetupOptions{.num_points = 64});

    std::vector<Complex, FFTAllocator<Complex>> fft_buffer(128);
    std::span<Complex> fft_result =
        complex_fft.Forward(test::FloatSignal64::kInput, fft_buffer);

    EXPECT_THAT(fft_result,
                Pointwise(ComplexNear(1e-5f), test::FloatSignal64::kOutput));
  }

  // Test normalization.
  {
    // Prepare the expected samples.
    std::vector<Complex> expected(test::FloatSignal64::kOutput.begin(),
                                  test::FloatSignal64::kOutput.end());
    for (Complex& x : expected) {
      x /= 64;
    }

    PFFFT<float> complex_fft(PFFFT<float>::SetupOptions{.num_points = 64});

    std::vector<Complex, FFTAllocator<Complex>> fft_buffer(128);
    std::span<Complex> fft_result = complex_fft.Forward(
        test::FloatSignal64::kInput, fft_buffer, {.normalize = true});

    EXPECT_THAT(fft_result, Pointwise(ComplexNear(1e-5f), expected));
  }
}

TEST(PFFFT, Complex) {
  {
    PFFFT<Complex> complex_fft(PFFFT<Complex>::SetupOptions{.num_points = 64});

    std::vector<Complex, FFTAllocator<Complex>> fft_buffer(128);
    const std::span<Complex> fft_result =
        complex_fft.Forward(test::ComplexSignal64::kInput, fft_buffer);

    EXPECT_THAT(fft_result,
                Pointwise(ComplexNear(1e-5f), test::ComplexSignal64::kOutput));
  }

  // Test normalization.
  {
    // Prepare the expected samples.
    std::vector<Complex> expected(test::ComplexSignal64::kOutput.begin(),
                                  test::ComplexSignal64::kOutput.end());
    for (Complex& x : expected) {
      x /= 64;
    }

    PFFFT<Complex> complex_fft(PFFFT<Complex>::SetupOptions{.num_points = 64});

    std::vector<Complex, FFTAllocator<Complex>> fft_buffer(126);
    const std::span<Complex> fft_result = complex_fft.Forward(
        test::ComplexSignal64::kInput, fft_buffer, {.normalize = true});

    EXPECT_THAT(fft_result, Pointwise(ComplexNear(1e-5f), expected));
  }

  // Test shift.
  {
    // Prepare the expected samples.
    std::vector<Complex> expected(test::ComplexSignal64::kOutput.begin(),
                                  test::ComplexSignal64::kOutput.end());
    FFTShift<Complex>(expected);

    PFFFT<Complex> complex_fft(PFFFT<Complex>::SetupOptions{.num_points = 64});

    std::vector<Complex, FFTAllocator<Complex>> fft_buffer(126);
    const std::span<Complex> fft_result = complex_fft.Forward(
        test::ComplexSignal64::kInput, fft_buffer, {.shift = true});

    EXPECT_THAT(fft_result, Pointwise(ComplexNear(1e-5f), expected));
  }

  // Test shift and normalization.
  {
    // Prepare the expected samples.
    std::vector<Complex> expected(test::ComplexSignal64::kOutput.begin(),
                                  test::ComplexSignal64::kOutput.end());
    FFTNormalizeAndShift<Complex>(expected);

    PFFFT<Complex> complex_fft(PFFFT<Complex>::SetupOptions{.num_points = 64});

    std::vector<Complex, FFTAllocator<Complex>> fft_buffer(126);
    const std::span<Complex> fft_result =
        complex_fft.Forward(test::ComplexSignal64::kInput,
                            fft_buffer,
                            {.normalize = true, .shift = true});

    EXPECT_THAT(fft_result, Pointwise(ComplexNear(1e-5f), expected));
  }
}

}  // namespace radio_core::fft
