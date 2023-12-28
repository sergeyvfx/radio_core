// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT
// Author: Sergey Sharybin (sergey.vfx@gmail.com)

// SSTV VOX tone specification which leads the VIS (Vertical Interval
// Signaling).

#pragma once

#include <array>

#include "radio_core/base/frequency_duration.h"

namespace radio_core::picture::sstv {

template <class RealType>
struct VOXTones {
  static inline constexpr std::array<FrequencyDuration<RealType>, 8> kTones = {
      {{1900, 100},
       {1500, 100},
       {1900, 100},
       {1500, 100},
       {2300, 100},
       {1500, 100},
       {2300, 100},
       {1500, 100}}};
};

}  // namespace radio_core::picture::sstv
