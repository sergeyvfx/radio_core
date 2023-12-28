// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Processor which encodes VOX tones into a waveform.

#pragma once

#include "radio_core/picture/sstv/vox.h"

namespace radio_core::picture::sstv {

template <class RealType>
class VOXEncoder {
 public:
  // Encode the VOX into a sequence of frequencies of specific duration.
  //
  // The frequency and duration is passed to the callback as an object of
  // FrequencyDuration<RealType>. The given list of args... is passed to the
  // callback after the frequency duration sample. This makes the required
  // callback signature to be:
  //
  //   callback(const FrequencyDuration<RealType>& sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(F&& callback, Args&&... args) {
    for (const auto& tone : VOXTones<RealType>::kTones) {
      std::invoke(std::forward<F>(callback), tone, std::forward<Args>(args)...);
    }
  }
};

}  // namespace radio_core::picture::sstv
