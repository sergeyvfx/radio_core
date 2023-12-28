// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Message of SSTV transmission.
// Specifies mode, image pixels, optional FSKID.

#pragma once

#include "radio_core/math/color.h"
#include "radio_core/picture/pixel_accessor.h"
#include "radio_core/picture/sstv/mode.h"

namespace radio_core::picture::sstv {

class Message {
 public:
  // Mode in which the image will be transmitted.
  //
  // The mode also defines the requirements to the image in terms of the image
  // resolution: the image must be the same resolution as the mode expects it
  // to be.
  Mode mode{Mode::kUnknown};

  // Accessor to the pixels of the transmitting image.
  //
  // It accesses 3 channel 8bit images because this is the most commonly used
  // format for the input images. In the context of the SSTV it does not make
  // much sense to go higher bit depths.
  using PixelAccessor = ConstPixelAccessor<Color3ub>;
  PixelAccessor* pixel_accessor{nullptr};

  // TODO(sergey): FSKID, CWID
};

}  // namespace radio_core::picture::sstv
