// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Specialization of FastArg() kernels for Arm Neon platforms.

#pragma once

#include "radio_core/base/build_config.h"

#if ISA_CPU_ARM_NEON

#  include <cassert>
#  include <span>

#  include "radio_core/base/half.h"
#  include "radio_core/math/complex4.h"
#  include "radio_core/math/float4.h"
#  include "radio_core/math/math.h"

#  if RADIO_CORE_HAVE_HALF
#    include "radio_core/math/half4.h"
#    include "radio_core/math/half8.h"
#    include "radio_core/math/half_complex4.h"
#    include "radio_core/math/half_complex8.h"
#  endif

namespace radio_core::kernel::fast_arg_internal {

template <class Real, bool SpecializationMarker>
struct Kernel;

template <>
struct Kernel<float, true> {
  static inline auto Execute(const std::span<const Complex>& samples,
                             const std::span<float>& arg) -> std::span<float> {
    assert(samples.size() <= arg.size());

    const size_t num_samples = samples.size();

    // TODO(sergey): On Apple M2 and clang-1403.0.22.14.1 the vectorized code
    // is 10% slower than the naive implementation.
    // Using multiple register to perform calculation does not seem to help.
#  if 0
    const Complex* __restrict samples_ptr = samples.data();
    float* __restrict arg_ptr = arg.data();

    const Complex* samples_begin = samples_ptr;
    const Complex* samples_end = samples_ptr + num_samples;

    // Handle 4 elements at a time.
    {
      const size_t num_samples_aligned = num_samples & ~size_t(3);
      const Complex* aligned_samples_end = samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const Complex4 samples4(samples_ptr);
        const Float4 abs4 = FastArg(samples4);

        abs4.Store(arg_ptr);

        samples_ptr += 4;
        arg_ptr += 4;
      }
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      *arg_ptr = FastArg(*samples_ptr);

      ++samples_ptr;
      ++arg_ptr;
    }
#  else
    for (size_t i = 0; i < num_samples; ++i) {
      arg[i] = FastArg(samples[i]);
    }

#  endif

    return arg.subspan(0, num_samples);
  }
};

#  if RADIO_CORE_HAVE_HALF

template <>
struct Kernel<Half, true> {
  static inline auto Execute(const std::span<const HalfComplex>& samples,
                             const std::span<Half>& arg) -> std::span<Half> {
    assert(samples.size() <= arg.size());

    const size_t num_samples = samples.size();

    const HalfComplex* __restrict samples_ptr = samples.data();
    Half* __restrict arg_ptr = arg.data();

    const HalfComplex* samples_begin = samples_ptr;
    const HalfComplex* samples_end = samples_ptr + num_samples;

    // Handle 8 elements at a time.
    {
      const size_t num_samples_aligned = num_samples & ~size_t(7);
      const HalfComplex* aligned_samples_end =
          samples_begin + num_samples_aligned;

      while (samples_ptr < aligned_samples_end) {
        // TODO(sergey): Investigate prefetching.

        const HalfComplex8 samples8(samples_ptr);
        const Half8 abs8 = FastArg(samples8);

        abs8.Store(arg_ptr);

        samples_ptr += 8;
        arg_ptr += 8;
      }
    }

    // Handle the remaining tail.
    while (samples_ptr < samples_end) {
      *arg_ptr = FastArg(*samples_ptr);

      ++samples_ptr;
      ++arg_ptr;
    }

    return arg.subspan(0, num_samples);
  }
};

#  endif  // RADIO_CORE_HAVE_HALF

}  // namespace radio_core::kernel::fast_arg_internal

#endif  // ISA_CPU_ARM_NEON
