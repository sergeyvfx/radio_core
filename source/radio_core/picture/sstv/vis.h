// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT
// Author: Sergey Sharybin (sergey.vfx@gmail.com)

// Vertical Interval Signaling (VIS). Used to indicate beginning of the image
// transmission (VIS is transmitted right before the first line of the image).
// VIS includes information about what modulation is used (PD120, Scottie, etc).
//
//                           Frequency over time diagram
//
//  Freq
//   Hz
//
//  1900  ┯━━━━━━━┓  ┏━━━━━━━┓ s                             parity ──┐
//        ┆       ┃  ┃       ┃ t                                      │    s
//        ┆       ┃  ┃       ┃ a   0         0         0         0    │    t
//  1300  ┆       ┃  ┃       ┃ r ┏━━━━┓    ┏━━━━┓    ┏━━━━┓    ┏━━━━┓ │    o
//        ┆       ┃  ┃       ┃ t ┃    ┃    ┃    ┃    ┃    ┃    ┃    ┃      p
//  1200  ┆       ┡━━┩       ┡━━━┛ b0 ┃ b1 ┃ b2 ┃ b3 ┃ b4 ┃ b5 ┃ b6 ┃ b7 ┏━━━━┯
//        ┆       ┆  ┆       ┆        ┃    ┃    ┃    ┃    ┃    ┃    ┃    ┃    ┆
//  1100  ┆       ┆  ┆       ┆        ┗━━━━┛    ┗━━━━┛    ┗━━━━┛    ┗━━━━┛    ┆
//        ┆       ┆  ┆       ┆          1         1         1         1       ┆
//        ┆       ┆  ┆       ┆                                                ┆
//        ┆       ┆  ┆       ┆                                                ┆
//  Time  ┆ 300ms ┆10┆ 300ms ┆                   10 * 30 ms                   ┆
//
//          Leader B   Leader
//                 r
//                 e
//                 a
//                 k
//
// Even parity is used. This means, that the number of logical ones must be even
// in the whole 8 bit code.

#pragma once

#include <array>

#include "radio_core/base/frequency.h"
#include "radio_core/base/frequency_duration.h"
#include "radio_core/base/interval.h"

namespace radio_core::picture::sstv {

template <class RealType>
struct VISTones {
  // Range of frequencies used by the VIS encoder.
  // This is pre-computed min/max of frequencies of all tones.
  static inline constexpr Interval<Frequency> kFrequencyInterval{1100, 1900};

  static inline constexpr FrequencyDuration<RealType> kLeaderTone{1900, 300};
  static inline constexpr FrequencyDuration<RealType> kBreakTone{1200, 10};

  // Leader tones sequence. The first 610ms of the transmission.
  static inline constexpr std::array<FrequencyDuration<RealType>, 3>
      kLeaderSequence = {{
          kLeaderTone,
          kBreakTone,
          kLeaderTone,
      }};

  static inline constexpr RealType kBitDurationInMilliseconds{30};

  static inline constexpr FrequencyDuration<RealType> kStartBit{
      1200, kBitDurationInMilliseconds};

  static inline constexpr FrequencyDuration<RealType> kBitValueZero{
      1300, kBitDurationInMilliseconds};
  static inline constexpr FrequencyDuration<RealType> kBitValueOne{
      1100, kBitDurationInMilliseconds};

  static inline constexpr FrequencyDuration<RealType> kStopBit{
      1200, kBitDurationInMilliseconds};
};

}  // namespace radio_core::picture::sstv
