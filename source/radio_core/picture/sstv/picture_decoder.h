// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of scanlines of picture encoded using SSTV.
//
// The synchronization to the transmission is either done by an external trigger
// such as VIS decoder or by this decoder by looking for horizontal line
// synchronization pulse.

#pragma once

#include <span>

#include "radio_core/picture/sstv/line_decoder.h"
#include "radio_core/picture/sstv/line_sync.h"
#include "radio_core/picture/sstv/mode.h"

namespace radio_core::picture::sstv {

template <class RealType>
class PictureDecoder {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate{0};

    // Mode of the picture encoding.
    Mode mode{Mode::kUnknown};
  };

  using Error = sstv::Error;
  using Result = sstv::DecodeResult;

  inline void Configure(const Options& options) {
    ConfigureLineSync(options);
    ConfigureLineDecoder(options);
  }

  inline auto operator()(const RealType frequency) -> Result {
    Result result = EmptyDecodeResult();

    line_sync_(frequency, [&]() {
      line_decoder_.OnLineSync();

      result.GetValue().emplace_back(LineSynchronization{});
    });

    const Result line_result = line_decoder_(frequency);

    return Combine(result, line_result);
  }

  // Inform the machine that a vertical synchronization (VIS) has been decoded.
  // The `line_time_offset_ms` indicates how much the line decoder is into the
  // line synchronization pulse when the `onVerticalSync()` is called. This
  // time offset is caused by a delay in the processing time of the VIS decoder.
  inline void OnVerticalSync(const uint8_t vis_code,
                             const RealType line_time_offset_ms) {
    line_decoder_.OnVerticalSync(vis_code, line_time_offset_ms);
  }

 private:
  inline void ConfigureLineSync(const Options& options) {
    const typename LineSync<RealType>::Options sync_options = {
        .sample_rate = options.sample_rate,
        .mode = options.mode,
    };
    line_sync_.Configure(sync_options);
  }

  inline void ConfigureLineDecoder(const Options& options) {
    typename LineDecoder<RealType>::Options decoder_options = {
        .sample_rate = options.sample_rate,
        .mode = options.mode,
    };
    line_decoder_.Configure(decoder_options);
  }

  LineSync<RealType> line_sync_;
  LineDecoder<RealType> line_decoder_;
};

}  // namespace radio_core::picture::sstv
