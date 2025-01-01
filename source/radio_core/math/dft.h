// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cassert>
#include <span>

#include "radio_core/base/constants.h"
#include "radio_core/math/complex.h"

// Headers for optimized vectorized implementation.
#include "radio_core/base/unroll.h"
#include "radio_core/math/complex4.h"
#include "radio_core/math/complex8.h"
#include "radio_core/math/float4.h"
#include "radio_core/math/float8.h"

namespace radio_core {

// Calculate value of a single DFT bin k.
//
// Uses naive non-optimized calculation of the formula from
// https://wikipedia.org/wiki/Discrete_Fourier_transform
template <class RealType>
inline auto CalculateDFTBinNaive(
    const std::span<const BaseComplex<RealType>> samples, const int k)
    -> BaseComplex<RealType> {
  const size_t N = samples.size();
  const RealType alpha = RealType(k) / N;

  BaseComplex<RealType> sum{0};
  for (int n = 0; n < N; ++n) {
    // TODO(sergey): Wrap phase to avoid possible precision issues.
    const RealType phase = -RealType(2) * constants::pi_v<RealType> * alpha * n;
    sum += samples[n] * ComplexExp(phase);
  }

  return sum / N;
}

// Calculate value of a single DFT bin k using Goertzel algorithm.
//
// References:
// - [Wikipedia-Goertzel] "Goertzel algorithm," Wikipedia, [Online].
//       Available: https://wikipedia.org/wiki/Goertzel_algorithm.
//       [Accessed: Dec. 28, 2024].
//
// - [DT0089] The Goertzel Algorithm to Compute Individual Terms of the Discrete
//       Fourier Transform (DFT), STMicroelectronics, Design Tip DT0089,
//       [Online]. Available:
//       https://www.st.com/resource/en/design_tip/dt0089-the-goertzel-algorithm-to-compute-individual-terms-of-the-discrete-fourier-transform-dft-stmicroelectronics.pdf.
//       [Accessed: Dec. 28, 2024].
template <class RealType>
inline auto CalculateDFTBinGoertzel(
    const std::span<const BaseComplex<RealType>> samples, const RealType k)
    -> BaseComplex<RealType> {
  const size_t N = samples.size();

  const RealType w = RealType(2) * constants::pi_v<RealType> * k / N;
  const RealType cw = Cos(w);
  const RealType sw = Sin(w);
  const RealType coeff = RealType(2) * cw;

  // Apply the IIR filter.
  //
  // It follows the implementation from Wikipedia (the pseudocode) with the
  // difference that here complex samples are handled (and complex storage is
  // used for the S[N-1] and S[N-2].
  //
  // This follows the idea from the Complex signals in real arithmetic section:
  // Since complex signals decompose linearly into real and imaginary parts,
  // the Goertzel algorithm can be computed in real arithmetic separately
  // over the sequence of real and imaginary parts.
  BaseComplex<RealType> s_prev{0};   // S[N-1]
  BaseComplex<RealType> s_prev2{0};  // S[N-2]
  for (int n = 0; n < N; ++n) {
    const BaseComplex<RealType> s = samples[n] + coeff * s_prev - s_prev2;
    s_prev2 = s_prev;
    s_prev = s;
  }

  // Real and imaginary parts of the individual sequences (real and imaginary)
  // from the "Single DFT term with real-valued arithmetic" section.
  const BaseComplex<RealType> It = s_prev * cw - s_prev2;
  const BaseComplex<RealType> Qt = s_prev * sw;

  // Correction for the non-integer index k [DT0089].
  // TODO(sergey): Avoid Sin/Cos if the k is integer.
  const RealType w2 = RealType(2) * constants::pi_v<RealType> * k;
  const RealType cw2 = Cos(w2);
  const RealType sw2 = Sin(w2);
  const BaseComplex<RealType> I = It * cw2 + Qt * sw2;
  const BaseComplex<RealType> Q = -It * sw2 + Qt * cw2;

  // Combine the real and imaginary sequences together:
  // result = real_part * 1j*imag_part.
  const BaseComplex<RealType> result(I.real - Q.imag, I.imag + Q.real);

  return result / N;
}

namespace dft_internal {

// Implementation of Goertzel algorithm which calculates multiple bins at a
// time. The bins are provided as an array of vectorized types.
template <int K, class RealType, int N>
void VectorizedGoertzel(const std::span<const BaseComplex<RealType>> samples,
                        const VectorizedFloatType<RealType, N> k[K],
                        BaseComplex<RealType>* dft_ptr) {
  using FloatN = VectorizedFloatType<RealType, N>;
  using ComplexN = VectorizedComplexType<RealType, N>;

  // 2 * pi, loaded into vectorized type FloatN.
  const FloatN k2Pi{RealType(2) * constants::pi_v<RealType>};

  const RealType num_samples = samples.size();
  const RealType num_samples_inv = RealType(1) / num_samples;

  const FloatN num_samples_inv4{num_samples_inv};

  FloatN w[K];
  Unroll<K>([&](const auto i) { w[i] = k2Pi * k[i] * num_samples_inv4; });

  FloatN sw[K], cw[K];
  Unroll<K>([&](const auto i) { SinCos(w[i], sw[i], cw[i]); });

  FloatN coeff[K];
  Unroll<K>([&](const auto i) { coeff[i] = RealType(2) * cw[i]; });

  ComplexN s_prev[K], s_prev2[K];
  Unroll<K>([&](const auto i) {
    s_prev[i] = ComplexN(RealType(0));
    s_prev2[i] = ComplexN(RealType(0));
  });
  for (size_t n = 0; n < num_samples; ++n) {
    const ComplexN x(samples[n]);

    ComplexN s[K];
    Unroll<K>([&](const auto i) {
      s[i] = MultiplyAdd(x, s_prev[i], coeff[i]) - s_prev2[i];
      s_prev2[i] = s_prev[i];
      s_prev[i] = s[i];
    });
  }

  ComplexN It[K], Qt[K];
  Unroll<K>([&](const auto i) {
    It[i] = s_prev[i] * cw[i] - s_prev2[i];
    Qt[i] = s_prev[i] * sw[i];
  });

  ComplexN result[K];
  Unroll<K>([&](const auto i) {
    const FloatN w2 = k2Pi * k[i];

    FloatN sw2, cw2;
    SinCos(w2, sw2, cw2);

    const ComplexN I = It[i] * cw2 + Qt[i] * sw2;
    const ComplexN Q = -It[i] * sw2 + Qt[i] * cw2;

    const FloatN result_real =
        (I.ExtractReal() - Q.ExtractImag()) * num_samples_inv4;
    const FloatN result_imag =
        (I.ExtractImag() + Q.ExtractReal()) * num_samples_inv4;

    result[i] = ComplexN(result_real, result_imag);
  });

  Unroll<K>([&](const auto i) { result[i].Store(dft_ptr + i * N); });
}

}  // namespace dft_internal

// Calculate DFT using Goertzel for the bin indices from the bins array.
// The result is written to the corresponding elements in the dft_storage.
// Returns span of the DFT where result has been actually written.
//
// NOTE: The size of the dft_storage needs to be at least the size of the bins.
template <class RealType>
inline auto CalculateMultipleDFTBinsGoertzel(
    const std::span<const BaseComplex<RealType>> samples,
    const std::span<const RealType> bins,
    const std::span<BaseComplex<RealType>> dft_storage)
    -> std::span<BaseComplex<RealType>> {
  const size_t num_samples = samples.size();
  for (size_t i = 0; i < num_samples; ++i) {
    dft_storage[i] = CalculateDFTBinGoertzel(samples, bins[i]);
  }
  return dft_storage.subspan(0, bins.size());
}

// Specialized implementation of CalculateMultipleDFTBinsGoertzel() for
// RealType=float.
template <>
inline auto CalculateMultipleDFTBinsGoertzel(
    const std::span<const Complex> samples,
    const std::span<const float> bins,
    const std::span<Complex> dft_storage) -> std::span<Complex> {
  assert(bins.size() <= dft_storage.size());

  const size_t num_bins = bins.size();

  const float* bins_begin = bins.data();
  const float* bins_end = bins_begin + num_bins;

  const float* __restrict bins_ptr = bins_begin;
  Complex* __restrict dft_ptr = dft_storage.data();

  // Handle 16 elements at a time.
  //
  // Even without full underlying vectorization this gives extra 50% speedup
  // on Apple M3 Max. Possibly due to better memory access pattern.
  {
    const size_t num_bins_aligned = num_bins & ~size_t(15);
    const float* aligned_bins_end = bins_begin + num_bins_aligned;
    while (bins_ptr < aligned_bins_end) {
      Float8 k[2];
      k[0] = Float8(bins_ptr);
      k[1] = Float8(bins_ptr + 8);
      dft_internal::VectorizedGoertzel<2>(samples, k, dft_ptr);
      bins_ptr += 16;
      dft_ptr += 16;
    }
  }

  // Handle 8 elements at a time.
  //
  // Even if the 8-way vectorized types do not have full hardware implementation
  // handling 2x4-way vectorized types gives advantage of utilizing more
  // registers at a time, giving measurable speedup.
  {
    const size_t num_bins_aligned = num_bins & ~size_t(7);
    const float* aligned_bins_end = bins_begin + num_bins_aligned;
    while (bins_ptr < aligned_bins_end) {
      const Float8 k(bins_ptr);
      dft_internal::VectorizedGoertzel<1>(samples, &k, dft_ptr);
      bins_ptr += 8;
      dft_ptr += 8;
    }
  }

  // Handle 4 elements at a time.
  {
    const size_t num_bins_aligned = num_bins & ~size_t(3);
    const float* aligned_bins_end = bins_begin + num_bins_aligned;
    while (bins_ptr < aligned_bins_end) {
      const Float4 k(bins_ptr);
      dft_internal::VectorizedGoertzel<1>(samples, &k, dft_ptr);
      bins_ptr += 4;
      dft_ptr += 4;
    }
  }

  while (bins_ptr < bins_end) {
    *dft_ptr = CalculateDFTBinGoertzel(samples, *bins_ptr);
    ++bins_ptr;
    ++dft_ptr;
  }

  return dft_storage.subspan(0, bins.size());
}

}  // namespace radio_core
