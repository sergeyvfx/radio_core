// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Limits of the built-in SSTV modes.

#pragma once

#include <cstdint>

#include "radio_core/base/frequency.h"
#include "radio_core/base/interval.h"

namespace radio_core::picture::sstv {

class ModeLimits {
 public:
  // TODO(sergey): Find some neat trick to either calculate these maximum limits
  // compile-time or do some reliable verification.

  // Range of frequencies used by the VIS encoder.
  // This is pre-computed min/max of frequencies of all tones.
  static inline constexpr Interval<Frequency> kFrequencyInterval{1100, 2300};

  // Maximum possible number of pixels per line.
  inline static constexpr int kMaxNumPixelsPerLine{4 * 800};

  // Maximum image resolution of image across all supported modes.
  inline static constexpr int kMaxImageWidth{800};
  inline static constexpr int kMaxImageHeight{616};

  // Minimum and maximum frequencies of luminance encoding.
  inline static constexpr Frequency kMinLUMAkFrequency{1500};
  inline static constexpr Frequency kMaxLUMAFrequency{2300};
};

}  // namespace radio_core::picture::sstv
