// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core::picture {

// Base class for read-only pixel accessor.
//
// Defines the API which allows to access the image information, and API to
// get pixel value in the desired type PixelType.
template <class PixelType>
struct ConstPixelAccessor {
  // Specification of the underlying image.
  struct Spec {
    int width{0};
    int height{0};
    int num_channels{0};
  };

  // Get the specification of the image this pixel accessor provides access to.
  virtual auto GetSpec() const -> Spec = 0;

  // Get value of a pixel at the given image position in the requested pixel
  // type.
  virtual auto GetPixel(int x, int y) const -> PixelType = 0;
};

}  // namespace radio_core::picture
