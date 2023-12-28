// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Lower Side Band modulation information.

#pragma once

#include <array>

#include "radio_core/modulation/analog/ssb/info.h"

namespace radio_core::modulation::analog {

template <Type type>
class Info;

template <>
class Info<Type::kLSB> {
 public:
  // The canonical abbreviated name of the modulation type.
  static constexpr const char* kName = "LSB";

  // The commonly used default bandwidth of the receive filter for this
  // modulation type.
  // While the bandwidth might vary from application to application, the goal of
  // this field is to give decent default value to be used in the interface.
  static constexpr int kDefaultBandwidth = ssb::Info::kDefaultBandwidth;

  // An ascending array of bandwidth which are commonly used by the
  // modulation type.
  static constexpr auto kTypicalBandwidths = ssb::Info::kTypicalBandwidths;

  // Reference point for the radio bandwidth filter. Basically, the placement
  // of the center frequency within the bandwidth.
  static constexpr BandwidthReference kBandwidthReference =
      BandwidthReference::kUpper;
};

}  // namespace radio_core::modulation::analog
