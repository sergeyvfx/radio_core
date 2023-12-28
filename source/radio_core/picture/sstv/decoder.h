// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder which encapsulates an entire pipeline of decoding SSTV images from
// an audio stream.

#pragma once

#include <span>

#include "radio_core/picture/sstv/mode.h"
#include "radio_core/picture/sstv/picture_decoder.h"
#include "radio_core/picture/sstv/prefilter.h"
#include "radio_core/picture/sstv/result.h"
#include "radio_core/picture/sstv/vis_decoder.h"

namespace radio_core::picture::sstv {

template <class RealType, template <class> class Allocator = std::allocator>
class Decoder {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate;

    // Mode of encoded picture.
    // Used by default, when mode is not known.
    Mode mode;
  };

  using Error = sstv::Error;
  using Result = sstv::DecodeResult;

  inline void Configure(const Options& options) {
    // Configure prefilter.
    const typename Prefilter::Options prefilter_options = {
        .sample_rate = options.sample_rate,
    };
    prefilter_.Configure(prefilter_options);

    // Configure picture decoder.
    const typename PictureDecoder::Options picture_options = {
        .sample_rate = options.sample_rate,
        .mode = options.mode,
    };
    picture_decoder_.Configure(picture_options);

    // Configure VIS decoder.
    const typename VISDecoder::Options vis_options = {
        .sample_rate = options.sample_rate,
    };
    vis_decoder_.Configure(vis_options);
  }

  inline auto operator()(const RealType audio_sample) -> Result {
    Result result = EmptyDecodeResult();

    const RealType frequency = prefilter_(audio_sample);

    const typename VISDecoder::Result vis_result = vis_decoder_(frequency);
    if (vis_result.Ok()) {
      const uint8_t vis_code = vis_result.GetValue();
      picture_decoder_.OnVerticalSync(vis_code,
                                      vis_decoder_.GetDelayInMilliseconds());

      // Append the VIS code to the result.
      result.GetValue().emplace_back(DecodedVISCode{vis_code});
    }

    const Result picture_result = picture_decoder_(frequency);
    if (!picture_result.Ok()) {
      return picture_result;
    }

    return Combine(result, picture_result);
  }

 private:
  using PictureDecoder = sstv::PictureDecoder<RealType>;
  PictureDecoder picture_decoder_;

  using VISDecoder = sstv::VISDecoder<RealType, Allocator>;
  VISDecoder vis_decoder_;

  using Prefilter = sstv::Prefilter<RealType, Allocator>;
  Prefilter prefilter_;
};

}  // namespace radio_core::picture::sstv
