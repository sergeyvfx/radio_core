// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the peak detection kernels which uses the available
// vectorized types on the current platform. It does not perform any more
// specific optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/kernel/internal/kernel_common.h"
#include "radio_core/math/math.h"

namespace radio_core::kernel::peak_detector_internal {

template <class T, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(const std::span<const T> samples,
                             const std::span<T> peak,
                             const T charge_rate,
                             const T discharge_rate) -> std::span<T> {
    using kernel_internal::VectorizedBase;

    using Type4 = typename VectorizedBase<T>::template VectorizedType<4>;
    using Type8 = typename VectorizedBase<T>::template VectorizedType<8>;

    assert(samples.size() == peak.size());

    const size_t num_samples = samples.size();

    const T* __restrict samples_ptr = samples.data();
    T* __restrict peak_ptr = peak.data();

    const T* samples_begin = samples_ptr;
    const T* samples_end = samples_ptr + num_samples;

    if constexpr (Type8::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const T* aligned_samples_end = samples_begin + num_samples_aligned;

      const Type8 charge_rate4(charge_rate);
      const Type8 discharge_rate4(discharge_rate);

      while (samples_ptr < aligned_samples_end) {
        const Type8 samples4(samples_ptr);
        const Type8 peak4(peak_ptr);
        const typename Type8::MaskType mask4 = samples4 > peak4;
        const Type8 weight4 = Select(mask4, charge_rate4, discharge_rate4);
        const Type8 new_peak4 = Lerp(peak4, samples4, weight4);

        new_peak4.Store(peak_ptr);

        samples_ptr += 8;
        peak_ptr += 8;
      }
    }

    if constexpr (Type4::kIsVectorized) {
      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const T* aligned_samples_end = samples_begin + num_samples_aligned;

      const Type4 charge_rate4(charge_rate);
      const Type4 discharge_rate4(discharge_rate);

      while (samples_ptr < aligned_samples_end) {
        const Type4 samples4(samples_ptr);
        const Type4 peak4(peak_ptr);
        const typename Type4::MaskType mask4 = samples4 > peak4;
        const Type4 weight4 = Select(mask4, charge_rate4, discharge_rate4);
        const Type4 new_peak4 = Lerp(peak4, samples4, weight4);

        new_peak4.Store(peak_ptr);

        samples_ptr += 4;
        peak_ptr += 4;
      }
    }

    while (samples_ptr < samples_end) {
      if (*samples_ptr > *peak_ptr) {
        *peak_ptr = Lerp(*peak_ptr, *samples_ptr, charge_rate);
      } else {
        *peak_ptr = Lerp(*peak_ptr, *samples_ptr, discharge_rate);
      }

      ++samples_ptr;
      ++peak_ptr;
    }

    return peak.subspan(0, num_samples);
  }
};

}  // namespace radio_core::kernel::peak_detector_internal
