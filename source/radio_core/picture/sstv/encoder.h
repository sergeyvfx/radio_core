// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of images into a SSTV transmission.
//
// The input message is encoded according to the requested mode, converted to
// a sequence of frequency tones, ans is written to the output processor.

#pragma once

#include "radio_core/picture/sstv/message.h"
#include "radio_core/picture/sstv/mode_spec.h"
#include "radio_core/picture/sstv/picture_encoder.h"
#include "radio_core/picture/sstv/vis_encoder.h"
#include "radio_core/picture/sstv/vox_encoder.h"

namespace radio_core::picture::sstv {

template <class RealType>
class Encoder {
 public:
  struct Options {
    // Denotes whether VOX codes are to be generated prior to the picture
    // transmission.
    bool generate_vox = true;
  };

  void Configure(const Options& options) {
    generate_vox_ = options.generate_vox;
  }

  // Encode the picture into SSTV transmission.
  //
  // Encoding happens into a sequence of frequencies of specific duration.
  //
  // The frequency and duration is passed to the callback as an object of
  // FrequencyDuration<RealType>. The given list of args... is passed to the
  // callback after the frequency duration sample. This makes the required
  // callback signature to be:
  //
  //   callback(const FrequencyDuration<RealType>& sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const Message& message, F&& callback, Args&&... args) {
    if (message.mode == Mode::kUnknown || !message.pixel_accessor) {
      return;
    }

    const ModeSpec mode_spec = ModeSpec<RealType>::Get(message.mode);

    // VOX.
    if (generate_vox_) {
      vox_encoder_(std::forward<F>(callback), std::forward<Args>(args)...);
    }

    // VIS.
    vis_encoder_(mode_spec.vis_code,
                 std::forward<F>(callback),
                 std::forward<Args>(args)...);

    picture_encoder_(
        message, std::forward<F>(callback), std::forward<Args>(args)...);

    // TODO(sergey): EOF, FSKID, CWID
  }

 private:
  bool generate_vox_ = true;

  VOXEncoder<RealType> vox_encoder_;
  VISEncoder<RealType> vis_encoder_;
  PictureEncoder<RealType> picture_encoder_;
};

}  // namespace radio_core::picture::sstv
