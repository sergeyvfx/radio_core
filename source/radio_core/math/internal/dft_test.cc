// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/dft.h"

#include <array>

#include "radio_core/math/complex.h"
#include "radio_core/math/internal/fft_test_data.h"
#include "radio_core/math/unittest/complex_matchers.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

using testing::ComplexNear;
using testing::Pointwise;

TEST(DFT, CalculateDFTBinNaive) {
  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[0])
  // (0.07947733307549136-0.02980438081000146j)
  EXPECT_THAT(
      CalculateDFTBinNaive<float>(fft::test::ComplexSignal64::kInput, 0),
      ComplexNear(Complex(0.07947733307549136f, -0.02980438081000146f), 1e-5f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[1])
  // (-0.014407815155715296+0.05006039075493518j)
  EXPECT_THAT(
      CalculateDFTBinNaive<float>(fft::test::ComplexSignal64::kInput, 1),
      ComplexNear(Complex(-0.014407815155715296f, 0.05006039075493518f),
                  1e-6f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[31])
  // (0.012031196737491683-0.09528264753642554j)
  EXPECT_THAT(
      CalculateDFTBinNaive<float>(fft::test::ComplexSignal64::kInput, 31),
      ComplexNear(Complex(0.012031196737491683f, -0.09528264753642554f),
                  1e-5f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[32])
  // (-0.2028785831942147+0.03576012172792265j)
  EXPECT_THAT(
      CalculateDFTBinNaive<float>(fft::test::ComplexSignal64::kInput, 32),
      ComplexNear(Complex(-0.2028785831942147f, 0.03576012172792265f), 1e-5f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[63])
  // (-0.03229625028059946-0.09105716778952469j)
  EXPECT_THAT(
      CalculateDFTBinNaive<float>(fft::test::ComplexSignal64::kInput, 63),
      ComplexNear(Complex(-0.03229625028059946, -0.09105716778952469f), 1e-5f));
}

TEST(DFT, CalculateDFTBinGoertzel) {
  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[0])
  // (0.07947733307549136-0.02980438081000146j)
  EXPECT_THAT(
      CalculateDFTBinGoertzel<float>(fft::test::ComplexSignal64::kInput, 0),
      ComplexNear(Complex(0.07947733307549136f, -0.02980438081000146f), 1e-5f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[1])
  // (-0.014407815155715296+0.05006039075493518j)
  EXPECT_THAT(
      CalculateDFTBinGoertzel<float>(fft::test::ComplexSignal64::kInput, 1),
      ComplexNear(Complex(-0.014407815155715296f, 0.05006039075493518f),
                  1e-6f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[31])
  // (0.012031196737491683-0.09528264753642554j)
  EXPECT_THAT(
      CalculateDFTBinGoertzel<float>(fft::test::ComplexSignal64::kInput, 31),
      ComplexNear(Complex(0.012031196737491683f, -0.09528264753642554f),
                  1e-5f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[32])
  // (-0.2028785831942147+0.03576012172792265j)
  EXPECT_THAT(
      CalculateDFTBinGoertzel<float>(fft::test::ComplexSignal64::kInput, 32),
      ComplexNear(Complex(-0.2028785831942147f, 0.03576012172792265f), 1e-5f));

  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> print(dft[63])
  // (-0.03229625028059946-0.09105716778952469j)
  EXPECT_THAT(
      CalculateDFTBinGoertzel<float>(fft::test::ComplexSignal64::kInput, 63),
      ComplexNear(Complex(-0.03229625028059946, -0.09105716778952469f), 1e-5f));
}

TEST(DFT, CalculateMultipleDFTBinsGoertzel) {
  // >>> m = scipy.linalg.dft(num_samples)
  // >>> dft = (samples @ m) / num_samples
  // >>> for i in [0, 1, 2, 3, 31, 32, 33, 34, 35, 61, 62, 63]:
  // ...     print(dft[i])
  // (0.07947733307549136-0.02980438081000146j)
  // (-0.014407815155715296+0.05006039075493518j)
  // (0.0075523305624986305-0.005597310636749439j)
  // (0.01822684429966189-0.11225639742980004j)
  // (0.012031196737491683-0.09528264753642554j)
  // (-0.2028785831942147+0.03576012172792265j)
  // (-0.027666451826504308-0.011778676677530943j)
  // (0.01663719690078352-0.09250764134601784j)
  // (0.029832827009134977-0.05579896483127814j)
  // (0.04149480978071162+0.04905582586238337j)
  // (-0.04414432636955623+0.1826809699808777j)
  // (-0.03229625028059946-0.09105716778952469j)
  //
  // The DFT from above is duplicated twice to allow testing wider register
  // implementations.
  std::vector<Complex> dft(32);
  // clang-format off
  EXPECT_THAT(CalculateMultipleDFTBinsGoertzel<float>(
                  fft::test::ComplexSignal64::kInput,
                  {{
                    // Iteration 1.
                    0, 1, 2, 3, 31, 32, 33, 34, 35, 61, 62, 63,
                    // Iteration 2.
                    0, 1, 2, 3, 31, 32, 33, 34, 35, 61, 62, 63}},
                  dft),
              Pointwise(ComplexNear(1e-5f),
                        std::to_array<Complex>({
                            // Iteration 1.
                            {0.07947733307549136, -0.02980438081000146},
                            {-0.014407815155715296, 0.05006039075493518},
                            {0.0075523305624986305, -0.005597310636749439},
                            {0.01822684429966189, -0.11225639742980004},
                            {0.012031196737491683, -0.09528264753642554},
                            {-0.2028785831942147, 0.03576012172792265},
                            {-0.027666451826504308, -0.011778676677530943},
                            {0.01663719690078352, -0.09250764134601784},
                            {0.029832827009134977, -0.05579896483127814},
                            {0.04149480978071162, 0.04905582586238337},
                            {-0.04414432636955623, 0.1826809699808777},
                            {-0.03229625028059946, -0.09105716778952469},
                            // Iteration 2.
                            {0.07947733307549136, -0.02980438081000146},
                            {-0.014407815155715296, 0.05006039075493518},
                            {0.0075523305624986305, -0.005597310636749439},
                            {0.01822684429966189, -0.11225639742980004},
                            {0.012031196737491683, -0.09528264753642554},
                            {-0.2028785831942147, 0.03576012172792265},
                            {-0.027666451826504308, -0.011778676677530943},
                            {0.01663719690078352, -0.09250764134601784},
                            {0.029832827009134977, -0.05579896483127814},
                            {0.04149480978071162, 0.04905582586238337},
                            {-0.04414432636955623, 0.1826809699808777},
                            {-0.03229625028059946, -0.09105716778952469},
                      })));
  // clang-format on
}

}  // namespace radio_core
