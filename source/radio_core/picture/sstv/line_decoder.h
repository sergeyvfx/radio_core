// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Processor which takes care of scanlines decoding.
//
// Uses an external trigger to lock to vertical or horizontal synchronization
// and decodes image from that point.
//
// The input is filtered frequency samples.

#pragma once

#include <algorithm>
#include <array>
#include <span>

#include "radio_core/base/verify.h"
#include "radio_core/math/average.h"
#include "radio_core/math/color.h"
#include "radio_core/math/colorspace.h"
#include "radio_core/math/math.h"
#include "radio_core/math/quantize.h"
#include "radio_core/picture/sstv/luma.h"
#include "radio_core/picture/sstv/message.h"
#include "radio_core/picture/sstv/mode.h"
#include "radio_core/picture/sstv/mode_limits.h"
#include "radio_core/picture/sstv/mode_spec.h"
#include "radio_core/picture/sstv/result.h"

namespace radio_core::picture::sstv {

template <class RealType>
class LineDecoder {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate{0};

    // Mode of encoded picture.
    // Used by default, when mode is not known.
    Mode mode{Mode::kUnknown};

    // Weight for exponential averaging of frequency used for smoothing
    // frequency of pixels.
    RealType pixel_sample_average_weight{0.85};
  };

  using Error = sstv::Error;
  using Result = sstv::DecodeResult;

  inline LineDecoder() { Reset(); }

  inline void Configure(const Options& options) {
    const RealType sample_rate = options.sample_rate;

    time_ms_per_sample_ = RealType(1000) / sample_rate;
    pixel_sample_average_weight_ = options.pixel_sample_average_weight;

    SetMode(options.mode);
  }

  inline void SetMode(const Mode mode) {
    mode_spec_ = ModeSpec<RealType>::Get(mode);

    Verify(mode_spec_.mode != Mode::kUnknown, "SSTV mode");
    Verify(mode_spec_.num_pixels_per_line <= line_pixels_luma_.size(),
           "SSTV number of pixels per line");

    total_synchronization_time_ =
        mode_spec_.line_sync.duration_ms + mode_spec_.line_porch.duration_ms;

    total_line_time_in_ms_ =
        total_synchronization_time_ + mode_spec_.line_pixels_duration_ms;
  }

  inline void Reset() { state_ = State::kWaitForSyncEvent; }

  inline auto operator()(const RealType frequency) -> Result {
    switch (state_) {
      case State::kWaitForSyncEvent: return HandleWaitForSyncEvent(frequency);
      case State::kDecodeLine: return HandleDecodeLine(frequency);
    }
    Unreachable();
  }

  // Inform the machine that a vertical synchronization (VIS) has been decoded.
  // The `line_time_offset_ms` indicates how much the line decoder is into the
  // line synchronization pulse when the `onVerticalSync()` is called. This
  // time offset is caused by a delay in the processing time of the VIS decoder.
  inline void OnVerticalSync(const uint8_t vis_code,
                             const RealType line_time_offset_ms) {
    const Mode mode = GetModeFromVISCode(vis_code);
    if (mode == Mode::kUnknown) {
      return;
    }

    SwitchToPictureDecoding(mode, line_time_offset_ms);
  }

  // Inform the machine that line synchronization has been detected at the
  // current sample.
  inline void OnLineSync() {
    SwitchOrSyncToDecodeLine(mode_spec_.line_sync.duration_ms);
  }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // State machine.

  enum class State {
    kWaitForSyncEvent,
    kDecodeLine,
  };

  State state_;

  // Number of samples decoded since the beginning of the line.
  //
  // NOTE: The line is measured since the beginning of the line synchronization.
  int num_line_samples_;

  // Offset in milliseconds of the line start.
  // This offset compensates for a non-integer number of samples per line,
  // avoiding clock drift.
  RealType line_start_offset_in_ms_;

  // Number of decoded lines of the transmission.
  //
  // Note that the number of image scanliens might be bigger than this because
  // some modes encode several image scanlines into a single SSTV transmission
  // line.
  int num_decoded_lines_;

  // Denotes whether pixels of the current line started to be decoded.
  bool pixels_decode_started_;

  // Number of pixels which were decoded from the current line.
  int num_decoded_pixels_in_line_;

  // Luminance of pixels of current line.
  std::array<RealType, ModeLimits::kMaxNumPixelsPerLine> line_pixels_luma_;

  // Average of frequency used for pixel luma sampling.
  RealType pixel_freq_average_;

  // Time (in milliseconds) since the line start at which pixel value is to be
  // sampled.
  RealType next_pixel_sample_time_;

  // Synchronization.

  inline void SwitchToPictureDecoding(const Mode mode,
                                      const RealType line_time_offset_ms) {
    SetMode(mode);

    state_ = State::kWaitForSyncEvent;
    SwitchOrSyncToDecodeLine(line_time_offset_ms);
  }

  // The `line_time_offset` is a time in milliseconds since the beginning of the
  // line synchronization which corresponds to this line (measured in
  // milliseconds).
  inline void SwitchOrSyncToDecodeLine(const RealType line_time_offset_ms) {
    // If we were not in the middle of decoding image reset number of of decoded
    // lines as we are starting to decode a new picture.
    if (state_ == State::kWaitForSyncEvent) {
      state_ = State::kDecodeLine;
      num_decoded_lines_ = 0;

      ClearImagePixels();
    }

    // TODO(sergey): Figure out an ideal strategy if the synchronization is
    // found in the middle of decoding.

    // Adjust time since the synchronization.
    line_start_offset_in_ms_ = line_time_offset_ms;

    // Reset line decoder.
    num_line_samples_ = 0;
    pixels_decode_started_ = false;
  }

  // Wait for synchronization.

  inline auto HandleWaitForSyncEvent(const RealType /*frequency*/) -> Result {
    // Nothing to do here. The synchronization is coming either from VIS decoder
    // or line decoder.
    return EmptyDecodeResult();
  }

  // Line sampling and decoding.

  inline auto HandleDecodeLine(const RealType frequency) -> Result {
    Result result = EmptyDecodeResult();

    ++num_line_samples_;

    const RealType current_line_time =
        line_start_offset_in_ms_ + time_ms_per_sample_ * num_line_samples_;

    pixel_freq_average_ = ExponentialMovingAverage(
        frequency, pixel_sample_average_weight_, pixel_freq_average_);

    if (pixels_decode_started_) {
      if (num_decoded_pixels_in_line_ < mode_spec_.num_pixels_per_line) {
        if (current_line_time >= next_pixel_sample_time_) {
          line_pixels_luma_[num_decoded_pixels_in_line_] =
              FrequencyToLuma(mode_spec_, pixel_freq_average_);

          ++num_decoded_pixels_in_line_;

          next_pixel_sample_time_ =
              total_synchronization_time_ +
              (num_decoded_pixels_in_line_ + 1) * mode_spec_.pixel_duration_ms;
        }
      }
    } else if (current_line_time >= total_synchronization_time_) {
      pixels_decode_started_ = true;
      num_decoded_pixels_in_line_ = 0;

      // Sample pixel at its trailing edge.
      // Due to averaging this shouldn't cause bleeding of the next pixel into
      // the current one.
      next_pixel_sample_time_ =
          total_synchronization_time_ + mode_spec_.pixel_duration_ms;

      // Reset the average accumulator.
      pixel_freq_average_ = frequency;
    }

    if (current_line_time > total_line_time_in_ms_) {
      // Indicate beginning of the new pixels data.
      if (num_decoded_lines_ == 0) {
        result.GetValue().emplace_back(ImagePixelsBegin{mode_spec_.mode});
      }

      const Result pixels_result = DecodePixelLuma();
      if (!pixels_result.Ok()) {
        Reset();
        return pixels_result;
      }

      result = Combine(result, pixels_result);
      ++num_decoded_lines_;

      if (num_decoded_lines_ == mode_spec_.num_lines) {
        // Indicate the end of the image.
        result.GetValue().emplace_back(ImagePixelsEnd{});
        Reset();
        return result;
      }

      num_line_samples_ = 0;
      line_start_offset_in_ms_ = current_line_time - total_line_time_in_ms_;

      pixels_decode_started_ = false;
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Line decoding into pixel values.

  inline auto DecodePixelLuma() -> Result {
    switch (mode_spec_.line_encoding) {
      case LineEncoding::kUnknown:
        // This situation is not supposed to happen, and there are checks and
        // validation during the configuration stage. Unless the bug is in some
        // other area of the code this case is unreachable.
        Unreachable();

      case LineEncoding::kYccAverageCrCb: return DecodeYCbCrAverageCrRb();
    }
    Unreachable();
  }

  auto DecodeYCbCrAverageCrRb() -> Result {
    Result result = EmptyDecodeResult();

    // Line 1.
    for (int x = 0; x < mode_spec_.image_width; ++x) {
      const Color3<RealType> ycc(
          line_pixels_luma_[x + mode_spec_.image_width * 0],
          line_pixels_luma_[x + mode_spec_.image_width * 2],
          line_pixels_luma_[x + mode_spec_.image_width * 1]);

      const Color3<RealType> rgb = Saturate(YCbCrToRGB(ycc));
      decoded_line1_[x] = rgb.template ConvertTo<Color3<uint8_t>>();
    }
    result.GetValue().emplace_back(ImagePixelsRow{
        std::span(decoded_line1_.data(), mode_spec_.image_width)});

    // Line 2.
    for (int x = 0; x < mode_spec_.image_width; ++x) {
      const Color3<RealType> ycc(
          line_pixels_luma_[x + mode_spec_.image_width * 3],
          line_pixels_luma_[x + mode_spec_.image_width * 2],
          line_pixels_luma_[x + mode_spec_.image_width * 1]);

      const Color3<RealType> rgb = Saturate(YCbCrToRGB(ycc));
      decoded_line2_[x] = rgb.template ConvertTo<Color3<uint8_t>>();
    }
    result.GetValue().emplace_back(ImagePixelsRow{
        std::span(decoded_line2_.data(), mode_spec_.image_width)});

    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output.

  inline void ClearImagePixels() {
    std::fill(decoded_line1_.begin(), decoded_line1_.end(), Color3ub(0, 0, 0));
    std::fill(decoded_line2_.begin(), decoded_line2_.end(), Color3ub(0, 0, 0));
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Specification of the currently decoding mode.
  ModeSpec<RealType> mode_spec_;

  // Number of milliseconds per one sample.
  RealType time_ms_per_sample_;

  // Weight for exponential averaging of frequency used for smoothing
  // frequency of pixels.
  RealType pixel_sample_average_weight_;

  // Total time of line synchronization (total line time without pixels, in
  // other words: line sync tone and porch).
  RealType total_synchronization_time_;

  // Total time of line in milliseconds (includes synchronization, porch,
  // pixels).
  RealType total_line_time_in_ms_;

  // Storage for decoded lines.
  std::array<Color3ub, ModeLimits::kMaxImageWidth> decoded_line1_;
  std::array<Color3ub, ModeLimits::kMaxImageWidth> decoded_line2_;
};

}  // namespace radio_core::picture::sstv
