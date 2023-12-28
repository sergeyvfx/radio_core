// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Specification of PD290 SSTV mode.
//
// Color transmission of 800x616 images using YCrCb colorspace.

#pragma once

#include <cstdint>

#include "radio_core/picture/sstv/line_encoding.h"
#include "radio_core/picture/sstv/mode.h"
#include "radio_core/picture/sstv/mode_spec/common.h"

namespace radio_core::picture::sstv {

template <class RealType>
struct ModeSpecInfo<RealType, Mode::kPD290> {
  inline static constexpr Mode kMode{Mode::kPD290};

  // Digital code transmitted in the VIS.
  inline static constexpr uint8_t kVISCode{static_cast<uint8_t>(kMode)};

  // Line synchronization.
  inline static constexpr FrequencyDuration<RealType> kLineSync{1200, 20};

  // Frequencies of fully black and fully white values (in terms of luminosity:
  // black refers to luminosity 0, white refers to luminosity 1).
  inline static constexpr RealType kBlackFrequency{1500};
  inline static constexpr RealType kWhiteFrequency{2300};

  // Image resolution.
  inline static constexpr int kImageWidth{800};
  inline static constexpr int kImageHeight{616};

  // Number of channels per pixel.
  inline static constexpr int kNumChannels{3};

  // Porch after line synchronization.
  inline static constexpr FrequencyDuration<RealType> kLinePorch{1500, 2.08};

  // Encoding scheme for the lines of the image.
  inline static constexpr LineEncoding kLineEncoding{
      LineEncoding::kYccAverageCrCb};

  // Duration of tone of single pixel value, in milliseconds.
  inline static constexpr RealType kPixelDurationInMilliseconds{0.286};

  // Number of pixels encoded into a single line.
  inline static constexpr int kNumPixelsPerLine{4 * kImageWidth};

  // Duration of pixels transmission per single line.
  // Starting with the beginning of the very first pixel of the line ending
  // with the end of the last pixel of the line.
  inline static constexpr RealType kLinePixelsDurationInMilliseconds{
      kNumPixelsPerLine * kPixelDurationInMilliseconds};

  // Number of lines in the encoded message.
  // Note that it could be different from image height since some modes encode
  // two rows of pixels into a single line of transmission.
  inline static constexpr int kNumLines{kImageHeight / 2};
};

}  // namespace radio_core::picture::sstv
