// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Band - radio frequency allocation information which covers a single band (or
// single block of frequencies).

#pragma once

#include <optional>

#include "radio_core/base/frequency.h"
#include "radio_core/base/interval.h"
#include "radio_core/modulation/analog/type.h"

namespace radio_core::info::band_plan {

// Special semantic type of a band.
enum class Type {
  kGeneric,

  kAirInstrument,
  kAirVoice,
  kBroadcast,
  kCB,
  kHam,
  kMarine,
};

using Modulation = modulation::analog::Type;

// Band definition.
struct Band {
  constexpr Band(const char* new_name,
                 const Interval<Frequency>& new_interval,
                 const Type new_type = Type::kGeneric,
                 const Frequency new_channel_spacing = 0,
                 const std::optional<Modulation> new_modulation = std::nullopt)
      : name(new_name),
        interval(new_interval),
        type(new_type),
        channel_spacing(new_channel_spacing),
        modulation(new_modulation) {}

  // Name of the band.
  // Typically displayed in the radio software UI.
  const char* name;

  // Start and end frequencies of the band.
  //
  // The lower frequency denotes the center frequency of the first channel in
  // the band, and the upper denotes the center frequency of the last channel.
  Interval<Frequency> interval{0, 0};

  Type type{Type::kGeneric};

  // Frequency difference between two adjacent channels of the band.
  Frequency channel_spacing{0};

  // Denotes the most common modulation type used in the band.
  std::optional<Modulation> modulation;
};

}  // namespace radio_core::info::band_plan
