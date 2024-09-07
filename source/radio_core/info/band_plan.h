// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Band plan - radio frequencies allocation information.

#pragma once

#include <span>

#include "radio_core/base/frequency.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/info/band.h"
#include "radio_core/info/internal/bands_itu1.h"

namespace radio_core::info::band_plan {

class BandPlan {
 public:
  using Bands = std::span<const Band>;

  // Regions of the band plan allocations.
  // https://en.wikipedia.org/wiki/ITU_Region
  enum class Region {
    // ITU Region 1 comprises Europe, Africa, the Commonwealth of Independent
    // States, Mongolia, and the Middle East west of the Persian Gulf, including
    // Iraq.
    kITU1,
  };

  constexpr BandPlan() = default;

  // Create a new band plan with the given bands in it.
  //
  // The band plan does not take ownership over the bands and does not create
  // a local copy of them.
  explicit constexpr BandPlan(const Bands bands) : bands_(bands) {}

  // Create a new band plan for a requested ITU region.
  explicit constexpr BandPlan(const Region region)
      : BandPlan(GetBandsForRegion(region)) {}

  // Get all bands of the plan.
  constexpr auto GetAllBands() const -> Bands { return bands_; }

  // Return first band which contains the given frequency.
  // If none of the bands contain it nullptr is returned.
  constexpr auto GetBandForFrequency(const Frequency frequency) const
      -> const Band* {
    for (const Band& band : GetAllBands()) {
      if (band.interval.Contains(frequency)) {
        return &band;
      }
    }
    return nullptr;
  }

  // Align frequency so that it is centered to a channel within the band.
  //
  // The frequency stays within the band it originally belong to. It is aligned
  // to the closest channel within the band.
  //
  // If the frequency is outside of any known band allocation the value is
  // returned as-is.
  constexpr auto AlignFrequencyToChannel(const Frequency frequency) const
      -> Frequency {
    const Band* band = GetBandForFrequency(frequency);
    if (!band) {
      // No band allocation, return frequency as-is.
      return frequency;
    }

    if (band->channel_spacing <= 1) {
      // Spacing is unspecified or has no effect, so do early output.
      return frequency;
    }

    const Frequency relative = frequency - band->interval.lower_bound;

    const Frequency multiplier = Round(relative / band->channel_spacing);

    // TODO(sergey): Clamp.

    // Round the frequency, as it is commonly expected that the tuned frequency
    // is an integer number of Hertz.
    //
    // It is possible that it is not integer after the rounding to the channel
    // because some channels are a non-integer division: i.e. the airband which
    // uses 25 kHz / 3 spacing.
    return Round(band->interval.lower_bound +
                 band->channel_spacing * multiplier);
  }

 private:
  static constexpr auto GetBandsForRegion(const Region region) -> Bands {
    switch (region) {
      case Region::kITU1: return band_plan_internal::GetITU1Bands();
    }
    Unreachable();
  }

  Bands bands_;
};

}  // namespace radio_core::info::band_plan
