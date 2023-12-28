// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Mode specification with parameters stored in a unified manner in an object.
// It is typically constructed from a static mode information.

#pragma once

#include <cstdint>

#include "radio_core/base/frequency_duration.h"
#include "radio_core/base/interval.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/picture/sstv/line_encoding.h"
#include "radio_core/picture/sstv/mode.h"
#include "radio_core/picture/sstv/mode_spec/pd120.h"
#include "radio_core/picture/sstv/mode_spec/pd160.h"
#include "radio_core/picture/sstv/mode_spec/pd180.h"
#include "radio_core/picture/sstv/mode_spec/pd240.h"
#include "radio_core/picture/sstv/mode_spec/pd290.h"
#include "radio_core/picture/sstv/mode_spec/pd90.h"

namespace radio_core::picture::sstv {

template <class RealType>
class ModeSpec {
 public:
  static inline auto Get(const Mode mode) -> ModeSpec {
    switch (mode) {
      case Mode::kUnknown:
        // This is more of an error situation which is not supposed to happen
        // under normal usage of modem API.
        //
        // TODO(sergey): What is the proper way to handle it?
        return ModeSpec();

      case Mode::kPD90:
        return ModeSpec::Make<ModeSpecInfo<RealType, Mode::kPD90>>();
      case Mode::kPD120:
        return ModeSpec::Make<ModeSpecInfo<RealType, Mode::kPD120>>();
      case Mode::kPD160:
        return ModeSpec::Make<ModeSpecInfo<RealType, Mode::kPD160>>();
      case Mode::kPD180:
        return ModeSpec::Make<ModeSpecInfo<RealType, Mode::kPD180>>();
      case Mode::kPD240:
        return ModeSpec::Make<ModeSpecInfo<RealType, Mode::kPD240>>();
      case Mode::kPD290:
        return ModeSpec::Make<ModeSpecInfo<RealType, Mode::kPD290>>();
    }

    UNREACHABLE();
  }

  ModeSpec() = default;

  Mode mode{Mode::kUnknown};

  // Digital code transmitted in the VIS.
  uint8_t vis_code{0};

  // Image resolution.
  int image_width{0};
  int image_height{0};

  // Number of channels per pixel.
  int num_channels{0};

  // Line synchronization.
  FrequencyDuration<RealType> line_sync;

  // Porch after line synchronization.
  FrequencyDuration<RealType> line_porch;

  // Encoding scheme for the lines of the image.
  LineEncoding line_encoding{LineEncoding::kUnknown};

  // Duration of tone of single pixel value, in milliseconds.
  RealType pixel_duration_ms{0};

  // Number of pixels encoded into a single line.
  int num_pixels_per_line{0};

  // Duration of pixels transmission per single line.
  //
  // Starting with the beginning of the very first pixel of the line ending
  // with the end of the last pixel of the line.
  RealType line_pixels_duration_ms{0};

  // Number of lines in the encoded message.
  // Note that it could be different from image height since some modes encode
  // two rows of pixels into a single line of transmission.
  int num_lines{0};

  // Frequencies of fully black and fully white values (in terms of
  // luminosity: black refers to luminosity 0, white refers to luminosity
  // 1).
  RealType black_frequency{0};
  RealType white_frequency{0};

 private:
  template <class T>
  static auto Make() -> ModeSpec {
    ModeSpec mode_spec;

    mode_spec.mode = T::kMode;
    mode_spec.vis_code = T::kVISCode;
    mode_spec.image_width = T::kImageWidth;
    mode_spec.image_height = T::kImageHeight;
    mode_spec.num_channels = T::kNumChannels;
    mode_spec.line_sync = T::kLineSync;
    mode_spec.line_porch = T::kLinePorch;
    mode_spec.line_encoding = T::kLineEncoding;
    mode_spec.pixel_duration_ms = T::kPixelDurationInMilliseconds;
    mode_spec.num_pixels_per_line = T::kNumPixelsPerLine;
    mode_spec.line_pixels_duration_ms = T::kLinePixelsDurationInMilliseconds;
    mode_spec.num_lines = T::kNumLines;
    mode_spec.black_frequency = T::kBlackFrequency;
    mode_spec.white_frequency = T::kWhiteFrequency;

    return mode_spec;
  }
};

}  // namespace radio_core::picture::sstv
