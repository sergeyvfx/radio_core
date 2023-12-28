// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Wideband frequency modulation information.

#pragma once

#include <array>

#include "radio_core/modulation/analog/type.h"

namespace radio_core::modulation::analog {

template <Type type>
class Info;

template <>
class Info<Type::kWFM> {
 public:
  // The canonical abbreviated name of the modulation type.
  static constexpr const char* kName = "WFM";

  // The commonly used default bandwidth of the receive filter for this
  // modulation type.
  // While the bandwidth might vary from application to application, the goal of
  // this field is to give decent default value to be used in the interface.
  static constexpr int kDefaultBandwidth = 150000;

  // An ascending array of bandwidth which are commonly used by the
  // modulation type.
  static constexpr auto kTypicalBandwidths =
      std::to_array<int>({80000, 150000, 187500, 250000});

  // Reference point for the radio bandwidth filter. Basically, the placement
  // of the center frequency within the bandwidth.
  static constexpr BandwidthReference kBandwidthReference =
      BandwidthReference::kCenter;
};

}  // namespace radio_core::modulation::analog
