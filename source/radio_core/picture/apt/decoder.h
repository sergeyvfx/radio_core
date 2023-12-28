// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder which encapsulates an entire pipeline of decoding APT images from
// an audio stream.
//
// The decoder is basically an AM demodulator with the APT sub-carrier frequency
// as the center AM frequency, and bandwidth matching the baud rate of the APT.

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

#include "radio_core/base/ring_buffer.h"
#include "radio_core/math/color.h"
#include "radio_core/math/math.h"
#include "radio_core/picture/apt/info.h"
#include "radio_core/picture/apt/result.h"
#include "radio_core/signal/analytical_signal.h"
#include "radio_core/signal/digital_hysteresis.h"
#include "radio_core/signal/ema_agc.h"
#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/filter_window_heuristic.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::picture::apt {

template <class RealType, template <class> class Allocator = std::allocator>
class Decoder {
  // Time which takes to transmit one pixel in the line.
  static constexpr RealType kTimePerPixel = RealType(1) / Info::kBaudRate;

 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate;

    // Configuration of the pre-filter which filters input samples.
    //
    // The pre-filter is filtering frequencies around the sub-carrier using the
    // baud rate as the filter bandwidth.
    //
    // The transition bandwidth is provided in Hz and defines the order of the
    // filter. It is measured in hertz.
    RealType prefilter_transition_bandwidth{70};

    // Configuration of the hilbert transformer of the analytical signal
    // calculator.
    // The attenuation and transition frequency is used to calculate alpha and
    // dw parameters of the Kaiser window used by the transformer. The
    // transition frequency is measured in hertz.
    RealType hilbert_attenuation{90};
    RealType hilbert_transition_frequency{Info::kBaudRate};

    // Configuration of AGC which ensures the intensity of pixels is close to
    // [0 .. 1] range.
    // Measured in the multiples of lines.
    // The default is to have fast charge response and slow discharge which
    // covers the full wedge calibration area.
    RealType agc_charge_num_lines{0.0005};
    RealType agc_discharge_num_lines{64};

    // Hysteresis is used to avoid digitizer ringing when the signal bounces
    // around the value which corresponds to the digital 1.
    //
    // Threshold is the transition level from 0 to 1, and the window defines the
    // "dead" zone around the threshold where no transition happens until the
    // signal goes outside of the window.
    RealType hysteresis_threshold{0.5};
    RealType hysteresis_window{0.1};
  };

  using Error = apt::Error;
  using Result = apt::DecodeResult;

  inline void Configure(const Options& options) {
    sync_bit_buffer_.resize(Info::kSyncA.size());
    sync_pixel_buffer_.resize(Info::kSyncA.size());

    time_per_sample_ = RealType(1) / options.sample_rate;

    ConfigurePrefilter(options);
    ConfigureAnalyticalSignal(options);
    ConfigureAGC(options);
    ConfigureHysteresis(options);
  }

  inline auto operator()(const RealType audio_sample) -> Result {
    Result result = EmptyDecodeResult();

    // Always push the audio sample to the processors, so they maintain their
    // state.
    const RealType prefiltered_sample = prefilter_(audio_sample);
    const typename AnalyticalSignal::Complex analytical_sample =
        analytical_signal_(agc_(prefiltered_sample));

    // Early output if the pixel is not to be sampled yet.
    //
    // TODO(sergey): Look into possibly averaging the value, to help dealing
    // with noisy signals. The downside is that it could introduce more blur
    // and make it harder to lock on the synchronization.
    current_time_within_pixel += time_per_sample_;
    if (current_time_within_pixel < kTimePerPixel) {
      return result;
    }

    // Convert amplitude to a pixel value.
    const RealType amplitude = Abs(analytical_sample);
    const RealType pixel_float = Saturate(amplitude);
    const uint8_t pixel_int = pixel_float * 255;

    // Append pixel to the decoded line.
    line_pixels_[num_line_pixels_++] = pixel_int;

    const bool bit = hysteresis_(pixel_float);
    sync_bit_buffer_.push_back(bit);
    sync_pixel_buffer_.push_back(pixel_int);

    // Synchronize to the line.
    if (IsLineSyncDetected()) {
      ResynchronizeCurrentLine();
      result.GetValue().emplace_back(LineSynchronization{});
    }

    // Emit line when it is fully decoded.
    if (num_line_pixels_ == Info::kNumPixelsPerLine) {
      result.GetValue().emplace_back(Line{line_pixels_});
      num_line_pixels_ = 0;
    }

    current_time_within_pixel -= kTimePerPixel;

    return result;
  }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // Configuration of various stages of the signal processing.

  void ConfigurePrefilter(const Options& options) {
    const int prefilter_num_taps =
        signal::EstimateFilterSizeForTransitionBandwidth(
            options.prefilter_transition_bandwidth, options.sample_rate) |
        1;

    prefilter_.SetKernelSize(prefilter_num_taps);

    signal::DesignBandPassFilter(
        prefilter_.GetKernel(),
        signal::WindowEquation<RealType, signal::Window::kHamming>(),
        RealType(Info::kSubCarrierFrequency) - Info::kBaudRate / 2,
        RealType(Info::kSubCarrierFrequency) + Info::kBaudRate / 2,
        options.sample_rate);
  }

  void ConfigureAnalyticalSignal(const Options& options) {
    // Configure analytical signal.
    const RealType alpha = options.hilbert_attenuation;
    const RealType beta = signal::CalculateKaiserBeta(alpha);
    const RealType dw = signal::NormalizedAngularFrequency(
        options.hilbert_transition_frequency, options.sample_rate);

    const int kaiser_size = signal::CalculateKaiserSize(alpha, dw) | 1;

    analytical_signal_.Design(
        kaiser_size,
        signal::WindowEquation<RealType, signal::Window::kKaiser>(beta));
  }

  void ConfigureAGC(const Options& options) {
    const RealType time_per_line = kTimePerPixel * Info::kNumPixelsPerLine;
    const int num_samples_per_line = options.sample_rate * time_per_line;

    const RealType agc_charge_rate =
        RealType(2) / (num_samples_per_line * options.agc_charge_num_lines + 1);
    const RealType agc_discharge_rate =
        RealType(2) /
        (num_samples_per_line * options.agc_discharge_num_lines + 1);

    agc_.Configure(agc_charge_rate, agc_discharge_rate);
  }

  void ConfigureHysteresis(const Options& options) {
    hysteresis_.SetThreshold(options.hysteresis_threshold,
                             options.hysteresis_window);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Line synchronization.

  // Returns true when the latest demodulated pixel form a line synchronization
  // marker.
  // The line synchronization is the Sync A since it is what the line of pixels
  // starts with.
  auto IsLineSyncDetected() -> bool {
    return std::equal(
        sync_bit_buffer_.begin(), sync_bit_buffer_.end(), Info::kSyncA.begin());
  }

  // Shuffle pixels of the currently decoding line after line synchronization
  // was detected.
  // makes it so that the line starts with the synchronization sequence.
  void ResynchronizeCurrentLine() {
    constexpr int kNumSyncPixels = Info::kSyncA.size();

    // Copy raw pixels from the synchronization buffer.
    std::copy(sync_pixel_buffer_.begin(),
              sync_pixel_buffer_.end(),
              line_pixels_.begin());

    num_line_pixels_ = kNumSyncPixels;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Local processor types for easier access.
  using SimpleFIRFilter =
      signal::SimpleFIRFilter<RealType, RealType, Allocator>;
  using AnalyticalSignal = signal::AnalyticalSignal<RealType, Allocator>;
  using EMAAGC = signal::EMAAGC<RealType>;
  using DigitalHysteresis = signal::DigitalHysteresis<RealType>;

  // Processors of the processing pipeline.,
  SimpleFIRFilter prefilter_;
  AnalyticalSignal analytical_signal_;
  EMAAGC agc_;
  DigitalHysteresis hysteresis_;

  // Pre-calculated time per input audio sample, measured in seconds.
  // This is effectively an inverse of the sample rate.
  RealType time_per_sample_{0};

  // Time elapsed since the beginning of the current pixel, in seconds.
  RealType current_time_within_pixel{0};

  // Demodulated pixels of the current line.
  std::array<Color1ub, Info::kNumPixelsPerLine> line_pixels_;
  int num_line_pixels_{0};

  // The last number of pixels (where the number matches the duration of the
  // synchronization in pixels).
  // This buffer is used to detect the line synchronization.
  RingBuffer<uint8_t, Allocator<uint8_t>> sync_bit_buffer_;
  RingBuffer<uint8_t, Allocator<uint8_t>> sync_pixel_buffer_;
};

}  // namespace radio_core::picture::apt
