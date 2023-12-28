// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Message of an Automatic Picture Transmission (APT).

#pragma once

#include "radio_core/math/color.h"
#include "radio_core/picture/pixel_accessor.h"

namespace radio_core::picture::apt {

class Message {
 public:
  // Accessor to the pixels of the transmitting image.
  // Provides access to a single-channel image.
  using PixelAccessor = ConstPixelAccessor<Color1ub>;

  // Accessors for the Image A and Image B of the transmission.
  PixelAccessor* pixel_accessor_a{nullptr};
  PixelAccessor* pixel_accessor_b{nullptr};
};

}  // namespace radio_core::picture::apt
