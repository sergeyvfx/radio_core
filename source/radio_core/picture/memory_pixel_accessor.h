// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cassert>
#include <span>
#include <type_traits>

#include "radio_core/picture/pixel_accessor.h"

namespace radio_core::picture {

namespace picture_internal {

template <class PixelType, class EnableTag = void>
struct PixelMaker;

// Creation of 1-channel pixel values.
template <class PixelType>
struct PixelMaker<PixelType, std::enable_if_t<PixelType::N == 1>> {
  using value_type = typename PixelType::value_type;

  static auto Make(const value_type& value) { return PixelType(value); }

  static auto Make(const value_type& v1,
                   const value_type& v2,
                   const value_type& v3) {
    return PixelType(0.2126f * v1 + 0.7152f * v2 + 0.0722f * v3);
  }
};

// Creation of 3-channel pixel values.
template <class PixelType>
struct PixelMaker<PixelType, std::enable_if_t<PixelType::N == 3>> {
  using value_type = typename PixelType::value_type;

  static auto Make(const value_type& value) {
    return PixelType(value, value, value);
  }

  static auto Make(const value_type& v1,
                   const value_type& v2,
                   const value_type& v3) {
    return PixelType(v1, v2, v3);
  }
};

}  // namespace picture_internal

// Implementation of the pixel accessor which reads pixel values from an
// in-memory buffer.
//
// The output pixel type is denoted by the PixelType. The accessor will perform
// needed conversion from the underlying pixel storage to the PixelType.
template <class PixelType>
struct ConstMemoryPixelAccessor : public ConstPixelAccessor<PixelType> {
  using Spec = typename ConstPixelAccessor<PixelType>::Spec;

  struct Parameters {
    // Width, height, and the number of channels in the image.
    int width{0};
    int height{0};
    int num_channels{0};

    // Number of single channel elements between first elements of two adjacent
    // pixels.
    //
    // When is set to 0 matches the number of channels in the image.
    int pixel_stride{0};

    // Number of single channel elements between first pixels of two adjacent
    // rows.
    //
    // When 0 the value is calculated from the image width and the resolved
    // pixel stride.
    int row_stride{0};

    inline auto Resolved() const -> Parameters {
      Parameters resolved = *this;

      if (resolved.pixel_stride == 0) {
        resolved.pixel_stride = resolved.num_channels;
      }

      if (resolved.row_stride == 0) {
        resolved.row_stride = resolved.pixel_stride * resolved.width;
      }

      return resolved;
    }
  };

  ConstMemoryPixelAccessor() = default;

  ConstMemoryPixelAccessor(
      std::span<const typename PixelType::value_type> pixels,
      const Parameters& parameters)
      : pixels_(pixels), parameters_(parameters.Resolved()) {
    assert(pixels_.size() >=
           size_t(parameters_.row_stride) * (parameters_.height - 1) +
               parameters_.pixel_stride * parameters_.width);
  }

  virtual auto GetSpec() const -> Spec {
    return {
        .width = parameters_.width,
        .height = parameters_.height,
        .num_channels = parameters_.num_channels,
    };
  }

  virtual auto GetPixel(int x, int y) const -> PixelType {
    const size_t element_index =
        size_t(y) * parameters_.row_stride + x * parameters_.pixel_stride;

    assert(parameters_.num_channels != 0);
    assert(parameters_.num_channels != 2);

    if (parameters_.num_channels == 1) {
      return picture_internal::PixelMaker<PixelType>::Make(
          pixels_[element_index]);
    }

    return picture_internal::PixelMaker<PixelType>::Make(
        pixels_[element_index + 0],
        pixels_[element_index + 1],
        pixels_[element_index + 2]);
  }

 private:
  std::span<const typename PixelType::value_type> pixels_;
  Parameters parameters_;
};

}  // namespace radio_core::picture
