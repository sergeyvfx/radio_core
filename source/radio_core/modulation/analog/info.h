// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <span>
#include <string_view>

#include "radio_core/base/unreachable.h"
#include "radio_core/modulation/analog/am/info.h"
#include "radio_core/modulation/analog/cw/info.h"
#include "radio_core/modulation/analog/lsb/info.h"
#include "radio_core/modulation/analog/nfm/info.h"
#include "radio_core/modulation/analog/type.h"
#include "radio_core/modulation/analog/usb/info.h"
#include "radio_core/modulation/analog/wfm/info.h"

namespace radio_core::modulation::analog {

// TODO(sergey): Is there a neat way to avoid duplication of the switch
// statements.

// The canonical abbreviated name of the modulation type.
// For example, "WFM", "AM", "USB".
inline auto GetName(const Type modulation) -> const char* {
  switch (modulation) {
    case Type::kAM: return Info<Type::kAM>::kName;

    case Type::kNFM: return Info<Type::kNFM>::kName;
    case Type::kWFM: return Info<Type::kWFM>::kName;

    case Type::kLSB: return Info<Type::kLSB>::kName;
    case Type::kUSB: return Info<Type::kUSB>::kName;

    case Type::kCW: return Info<Type::kCW>::kName;
  }

  Unreachable();
}

namespace analog_internal {

template <Type kType, Type... TrailingTypes>
inline auto TypeFromName(const std::string_view name, Type& type) -> bool {
  if (name == Info<kType>::kName) {
    type = kType;
    return true;
  }

  if constexpr (sizeof...(TrailingTypes) != 0) {
    return TypeFromName<TrailingTypes...>(name, type);
  }

  return false;
}

}  // namespace analog_internal

// Convert name of modulation to its enumeration value.
// If the name is not known then false is returned and the type is left
// unchanged.
inline auto TypeFromName(const std::string_view name, Type& type) -> bool {
  return analog_internal::TypeFromName<Type::kAM,
                                       Type::kNFM,
                                       Type::kWFM,
                                       Type::kLSB,
                                       Type::kUSB,
                                       Type::kCW>(name, type);
}

// Get the commonly used default bandwidth of the receive filter for the given
// modulation type.
// While the bandwidth might vary from application to application, the goal of
// this value is to give decent default value to be used in the interface.
inline auto GetDefaultBandwidth(const Type modulation) -> int {
  switch (modulation) {
    case Type::kAM: return Info<Type::kAM>::kDefaultBandwidth;

    case Type::kNFM: return Info<Type::kNFM>::kDefaultBandwidth;
    case Type::kWFM: return Info<Type::kWFM>::kDefaultBandwidth;

    case Type::kLSB: return Info<Type::kLSB>::kDefaultBandwidth;
    case Type::kUSB: return Info<Type::kUSB>::kDefaultBandwidth;

    case Type::kCW: return Info<Type::kCW>::kDefaultBandwidth;
  }
  Unreachable();
}

// For the given modulation type get typical filter bandwidths.
// The result values are in hertz.
inline auto GetTypicalBandwidths(const Type modulation)
    -> std::span<const int> {
  switch (modulation) {
    case Type::kAM: return Info<Type::kAM>::kTypicalBandwidths;

    case Type::kNFM: return Info<Type::kNFM>::kTypicalBandwidths;
    case Type::kWFM: return Info<Type::kWFM>::kTypicalBandwidths;

    case Type::kLSB: return Info<Type::kLSB>::kTypicalBandwidths;
    case Type::kUSB: return Info<Type::kUSB>::kTypicalBandwidths;

    case Type::kCW: return Info<Type::kCW>::kTypicalBandwidths;
  }

  Unreachable();
}

// Get the reference point for the radio bandwidth filter.
// Basically, the placement of the center frequency within the bandwidth.
inline auto GetBandwidthReference(const Type modulation) -> BandwidthReference {
  switch (modulation) {
    case Type::kAM: return Info<Type::kAM>::kBandwidthReference;

    case Type::kNFM: return Info<Type::kNFM>::kBandwidthReference;
    case Type::kWFM: return Info<Type::kWFM>::kBandwidthReference;

    case Type::kLSB: return Info<Type::kLSB>::kBandwidthReference;
    case Type::kUSB: return Info<Type::kUSB>::kBandwidthReference;

    case Type::kCW: return Info<Type::kCW>::kBandwidthReference;
  }

  Unreachable();
}

}  // namespace radio_core::modulation::analog
