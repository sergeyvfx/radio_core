// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the rotator kernel which uses the available vectorized
// types on the current platform. It does not perform any more specific
// optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/kernel/internal/kernel_common.h"
#include "radio_core/math/math.h"

namespace radio_core::kernel::rotator_internal {

template <class Real, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(const std::span<const BaseComplex<Real>> samples,
                             BaseComplex<Real>& phase,
                             const BaseComplex<Real> phase_increment_per_sample,
                             const std::span<BaseComplex<Real>> output)
      -> std::span<BaseComplex<Real>> {
    using kernel_internal::VectorizedBase;

    using RealComplex = BaseComplex<Real>;
    using RealComplex4 =
        typename VectorizedBase<RealComplex>::template VectorizedType<4>;
    using RealComplex8 =
        typename VectorizedBase<RealComplex>::template VectorizedType<8>;

    assert(samples.size() <= output.size());

    const size_t num_samples = samples.size();

    const RealComplex* samples_ptr = samples.data();
    RealComplex* output_ptr = output.data();

    const RealComplex* samples_begin = samples_ptr;
    const RealComplex* samples_end = samples_ptr + num_samples;

    if constexpr (RealComplex8::kIsVectorized) {
      RealComplex phase_local_increment(1, 0);
      RealComplex phase_data8[8] = {
          phase, phase, phase, phase, phase, phase, phase, phase};
      for (int i = 0; i < 8; ++i) {
        phase_data8[i] *= phase_local_increment;
        phase_local_increment *= phase_increment_per_sample;
      }

      // Per-element increment in phase by 8 samples.
      const RealComplex8 phase_increment8(phase_local_increment);

      // Nth element of the phase is advanced from N-1th by the phase increment
      // per sample.
      RealComplex8 phase8(phase_data8);

      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const RealComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const RealComplex8 samples8(samples_ptr);
        const RealComplex8 rotated8 = samples8 * phase8;

        phase8 = phase8 * phase_increment8;

        rotated8.Store(output_ptr);

        samples_ptr += 8;
        output_ptr += 8;
      }
    }

    // TODO(sergey): Investigate if auto-vectorization helps for non-explicitly
    // vectorized complex4.
    if constexpr (RealComplex4::kIsVectorized) {
      RealComplex phase_local_increment(1, 0);
      RealComplex phase_data4[4] = {phase, phase, phase, phase};
      for (int i = 0; i < 4; ++i) {
        phase_data4[i] *= phase_local_increment;
        phase_local_increment *= phase_increment_per_sample;
      }

      // Per-element increment in phase by 4 samples.
      const RealComplex4 phase_increment4(phase_local_increment);

      // Nth element of the phase is advanced from N-1th by the phase increment
      // per sample.
      RealComplex4 phase4(phase_data4);

      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const RealComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const RealComplex4 samples4(samples_ptr);

        const RealComplex4 rotated4 = samples4 * phase4;

        rotated4.Store(output_ptr);

        phase4 = phase4 * phase_increment4;

        samples_ptr += 4;
        output_ptr += 4;
      }

      phase = phase4.template Extract<0>();
    }

    while (samples_ptr < samples_end) {
      *output_ptr = *samples_ptr * phase;

      // e^(j*w(t+1)) = e^(j*w(t)) * e^(j*w)
      phase *= phase_increment_per_sample;

      ++samples_ptr;
      ++output_ptr;
    }

    phase /= Abs(phase);

    return output.subspan(0, num_samples);
  }
};

}  // namespace radio_core::kernel::rotator_internal
