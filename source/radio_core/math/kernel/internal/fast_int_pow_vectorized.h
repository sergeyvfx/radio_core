// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

// Implementation of the FastIntPow() kernel which uses the available vectorized
// types on the current platform. It does not perform any more specific
// optimizations like utilization of multiple registers.

#pragma once

#include <cassert>
#include <span>

#include "radio_core/math/base_complex.h"
#include "radio_core/math/kernel/internal/kernel_common.h"
#include "radio_core/math/math.h"

namespace radio_core::kernel::fast_int_pow_internal {

template <class RealType, bool SpecializationMarker>
struct Kernel {
  static inline auto Execute(const std::span<const BaseComplex<RealType>>& base,
                             const int exp,
                             const std::span<BaseComplex<RealType>>& pow)
      -> std::span<BaseComplex<RealType>> {
    using kernel_internal::VectorizedBase;

    using ComplexType = BaseComplex<RealType>;
    using ComplexType4 = typename VectorizedBase<
        BaseComplex<RealType>>::template VectorizedType<4>;
    using ComplexType8 = typename VectorizedBase<
        BaseComplex<RealType>>::template VectorizedType<8>;

    assert(base.size() <= pow.size());

    const size_t size = base.size();

    const ComplexType* __restrict base_ptr = base.data();
    ComplexType* __restrict pow_ptr = pow.data();

    const ComplexType* base_begin = base_ptr;
    const ComplexType* base_end = base_ptr + size;

    // Handle 8 elements at a time.
    if constexpr (ComplexType8::kIsVectorized) {
      const size_t size_aligned = size & ~size_t(7);
      const ComplexType* aligned_base_end = base_begin + size_aligned;

      while (base_ptr < aligned_base_end) {
        // TODO(sergey): Investigate prefetching.

        const ComplexType8 base8(base_ptr);
        const ComplexType8 pow8 = FastIntPow(base8, exp);

        pow8.Store(pow_ptr);

        base_ptr += 8;
        pow_ptr += 8;
      }
    }

    // Handle 4 elements at a time.
    if constexpr (ComplexType4::kIsVectorized) {
      const size_t size_aligned = size & ~size_t(3);
      const ComplexType* aligned_base_end = base_begin + size_aligned;

      while (base_ptr < aligned_base_end) {
        // TODO(sergey): Investigate prefetching.

        const ComplexType4 base4(base_ptr);
        const ComplexType4 pow4 = FastIntPow(base4, exp);

        pow4.Store(pow_ptr);

        base_ptr += 4;
        pow_ptr += 4;
      }
    }

    // Handle the remaining tail.
    while (base_ptr < base_end) {
      *pow_ptr = FastIntPow(*base_ptr, exp);

      ++base_ptr;
      ++pow_ptr;
    }

    return pow.subspan(0, size);
  }
};

}  // namespace radio_core::kernel::fast_int_pow_internal
