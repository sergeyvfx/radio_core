// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Bands allocation for the ITU Region 1.
//
// Region 1 comprises Europe, Africa, the Commonwealth of Independent States,
// Mongolia, and the Middle East west of the Persian Gulf, including Iraq.
//
// https://en.wikipedia.org/wiki/ITU_Region

#pragma once

#include <array>
#include <span>

#include "radio_core/info/band.h"

namespace radio_core::info::band_plan::band_plan_internal {

// TODO(sergey): Finish filling in the channel spacing.

class ITU1 {
 public:
  static constexpr auto kBandPlan = std::to_array({

      // https://en.wikipedia.org/wiki/Longwave
      //
      // ITU1 AM broadcasting.
      //
      // TODO(sergey): Figure out the proper channel spacing. The Wiki page
      // mentioned the channels are aligned to 9 kHz, but the band start/stop
      // frequencies are not aligned to it.
      Band("LW",
           {148.5_kHz, 283.5_kHz},
           Type::kBroadcast,
           100_Hz,
           Modulation::kAM),

      // https://en.wikipedia.org/wiki/Medium_wave
      //
      // Following the text after the table.
      Band("MW", {531_kHz, 1602_kHz}, Type::kBroadcast, 9_kHz, Modulation::kAM),

      // https://en.wikipedia.org/wiki/Shortwave_bands
      Band("SW broadcast",  // 120 m
           {2.3_MHz, 2.495_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 90 m
           {3.2_MHz, 3.4_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 75 m
           {3.9_MHz, 4_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 60 m
           {4.75_MHz, 4.995_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 49 m
           {5.9_MHz, 6.2_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 41 m
           {7.2_MHz, 7.45_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 31 m
           {9.4_MHz, 9.9_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 25 m
           {11.6_MHz, 12.1_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 22 m
           {13.57_MHz, 13.87_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 19 m
           {15.1_MHz, 15.83_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 16 m
           {17.48_MHz, 17.9_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 15 m
           {18.9_MHz, 19.02_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 13 m
           {21.45_MHz, 21.85_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),
      Band("SW broadcast",  // 11 m
           {25.67_MHz, 26.1_MHz},
           Type::kBroadcast,
           5_kHz,
           Modulation::kAM),

      // https://en.wikipedia.org/wiki/Citizens_band_radio
      Band("CB", {26.965_MHz, 27.405_MHz}, Type::kCB, 5_kHz, Modulation::kAM),

      // https://en.wikipedia.org/wiki/FM_broadcast_band
      //
      // In the Netherlands the spacing seems to be 100 kHz, which is actually
      // aligned with the Center frequencies section of the Wikipedia.
      Band("FM broadcast", {87.5_MHz, 108.0_MHz}, Type::kBroadcast, 100_kHz),

      // https://en.wikipedia.org/wiki/Airband
      Band("air band ILS/VOR",
           {108.0_MHz, 118.0_MHz},
           Type::kAirInstrument,
           50_kHz,
           Modulation::kAM),
      Band("air band voice",
           {118.0_MHz, 136.975_MHz},
           Type::kAirVoice,
           25_kHz / 3,
           Modulation::kAM),

      // https://en.wikipedia.org/wiki/Marine_VHF_radio
      //
      // The WIki mentions 25 kHz spacing. Use more narrow spacing which matches
      // the NFM filter width.
      Band("marine L/F",
           {155.5_MHz, 155.825_MHz},
           Type::kMarine,
           12.5_kHz,
           Modulation::kNFM),
      Band("marine",
           {156.0_MHz, 174.0_MHz},
           Type::kMarine,
           12.5_kHz,
           Modulation::kNFM),

      // https://en.wikipedia.org/wiki/Amateur_radio_frequency_allocations
      Band("2200m ham", {135.7_kHz, 137.8_kHz}, Type::kHam),
      Band("630m ham", {472.0_kHz, 479.0_kHz}, Type::kHam),
      Band("160m ham", {1810.0_kHz, 2000.0_kHz}, Type::kHam),
      Band("80m ham", {3500.0_kHz, 3800.0_kHz}, Type::kHam),
      Band("60m ham", {5258.5_kHz, 5450.0_kHz}, Type::kHam),
      Band("40m ham", {7000.0_kHz, 7200.0_kHz}, Type::kHam),
      Band("30m ham", {10100.0_kHz, 10150.0_kHz}, Type::kHam),
      Band("20m ham", {14000.0_kHz, 14350.0_kHz}, Type::kHam),
      Band("17m ham", {18068.0_kHz, 18168.0_kHz}, Type::kHam),
      Band("15m ham", {21000.0_kHz, 21450.0_kHz}, Type::kHam),
      Band("12m ham", {24890.0_kHz, 24990.0_kHz}, Type::kHam),
      Band("10m ham", {28000.0_kHz, 29700.0_kHz}, Type::kHam),
      Band("8m ham", {39.9_MHz, 40.7_MHz}, Type::kHam),
      Band("8m ham", {50.0_MHz, 52.0_MHz}, Type::kHam),
      Band("5m ham", {58.5_MHz, 60.1_MHz}, Type::kHam),
      Band("4m ham", {69.9_MHz, 70.5_MHz}, Type::kHam),
      Band("2m ham", {144.0_MHz, 146.0_MHz}, Type::kHam, 12.5_kHz),
      // 1.25 m ham band is not available in ITU 1.
      Band("70cm ham", {430.0_MHz, 440.0_MHz}, Type::kHam),
      Band("23cm ham", {1260.0_MHz, 1270.0_MHz}, Type::kHam),
      Band("13cm ham", {2400.0_MHz, 2450.0_MHz}, Type::kHam),
      Band("9cm ham", {3300.0_MHz, 3500.0_MHz}, Type::kHam),
  });
};

inline constexpr auto GetITU1Bands() -> std::span<const Band> {
  return ITU1::kBandPlan;
}

}  // namespace radio_core::info::band_plan::band_plan_internal
