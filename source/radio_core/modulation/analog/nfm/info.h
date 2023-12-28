// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Narrowband frequency modulation information.

#pragma once

#include <array>

#include "radio_core/modulation/analog/type.h"

namespace radio_core::modulation::analog {

template <Type type>
class Info;

template <>
class Info<Type::kNFM> {
 public:
  // The canonical abbreviated name of the modulation type.
  static constexpr const char* kName = "NFM";

  // The commonly used default bandwidth of the receive filter for this
  // modulation type.
  // While the bandwidth might vary from application to application, the goal of
  // this field is to give decent default value to be used in the interface.
  static constexpr int kDefaultBandwidth = 12500;

  // An ascending array of bandwidth which are commonly used by the
  // modulation type.
  static constexpr auto kTypicalBandwidths =
      std::to_array<int>({6000, 9000, 12500, 22000, 34000, 40000});

  // Reference point for the radio bandwidth filter. Basically, the placement
  // of the center frequency within the bandwidth.
  static constexpr BandwidthReference kBandwidthReference =
      BandwidthReference::kCenter;
};

}  // namespace radio_core::modulation::analog
