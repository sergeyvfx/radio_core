// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "radio_core/base/interval.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/modulation/analog/info.h"

namespace radio_core::modulation::analog {

// Get interval of frequencies relative to the center frequency which define the
// receive filter of the given bandwidth for the given modulation type.
template <class RealType>
auto GetBandwidthIntervalAroundCenterFrequency(
    const Type modulation, const RealType bandwidth) -> Interval<RealType> {
  switch (GetBandwidthReference(modulation)) {
    case BandwidthReference::kCenter: return {-bandwidth / 2, bandwidth / 2};
    case BandwidthReference::kLower: return {0, bandwidth};
    case BandwidthReference::kUpper: return {-bandwidth, 0};
  }

  Unreachable();
}

}  // namespace radio_core::modulation::analog
