// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Color utilities for SSTV transmission.

#pragma once

#include "radio_core/math/math.h"
#include "radio_core/picture/sstv/mode_spec.h"

namespace radio_core::picture::sstv {

template <class RealType>
inline auto FrequencyToLuma(const ModeSpec<RealType>& mode_spec,
                            const RealType frequency) -> RealType {
  return Saturate((frequency - mode_spec.black_frequency) /
                  (mode_spec.white_frequency - mode_spec.black_frequency));
}

template <class RealType>
inline auto LumaToFrequency(const ModeSpec<RealType>& mode_spec,
                            const RealType luma) -> RealType {
  return Saturate(luma) *
             (mode_spec.white_frequency - mode_spec.black_frequency) +
         mode_spec.black_frequency;
}

}  // namespace radio_core::picture::sstv
