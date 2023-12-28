// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core::modulation::analog {

// Mnemonic for an analog modulation type.
enum class Type {
  // Amplitude Modulation.
  kAM,

  // Wide and Narrow Frequency Modulation.
  kNFM,
  kWFM,

  // Upper Side Band, and Lower Side Band.
  kUSB,
  kLSB,

  // Continuous Wave modulation.
  kCW,
};

// Reference point for the bandwidth control.
enum class BandwidthReference {
  // Bandwidth is measured relative to the center frequency.
  // Defines the receive filter [center - bandwidth/2 .. center + bandwidth/2].
  kCenter,

  // The center frequency is at the lower boundary of the bandwidth.
  // Defines the receive filter [center .. center + bandwidth].
  kLower,

  // The center frequency is at the upper boundary of the bandwidth.
  // Defines the receive filter [center - bandwidth .. center].
  kUpper,
};

}  // namespace radio_core::modulation::analog
