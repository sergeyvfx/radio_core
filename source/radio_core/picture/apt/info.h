// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Automatic Picture Transmission (APT) protocol information.
//
// This file covers common for decoder and encoder constants.

#pragma once

#include <array>
#include <cstdint>

namespace radio_core::picture::apt {

struct Info {
  // Audio sub-carrier frequency at which the signal is being modulated at.
  //
  // The signal of the picture transmission is a 256-level amplitude modulated
  // at this audio frequency.
  static constexpr int kSubCarrierFrequency = 2400;

  // Width of an actual image.
  // Counts a single image, without sync, spacing, or telemetry.
  static constexpr int kImageWidth = 909;

  // Width of the space and telemetry fields in pixels.
  static constexpr int kSpaceWidth = 47;
  static constexpr int kTelemetryWidth = 45;

  // THe number of pixels per line of picture transmission.
  // This includes Sync, Space, Image, and Telemetry for Image A and B.
  static constexpr int kNumPixelsPerLine = 2080;

  // Baud rate of the transmission.
  static constexpr int kBaudRate = 4160;

  // Bits of synchronization markers.
  static constexpr std::array<uint8_t, 39> kSyncA = {
      0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static constexpr std::array<uint8_t, 39> kSyncB = {
      0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1,
      1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0};
};

}  // namespace radio_core::picture::apt
