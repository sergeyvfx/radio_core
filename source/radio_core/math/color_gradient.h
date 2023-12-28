// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Functions to operate on color gradient: rasterization and interpolation.

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <span>

#include "radio_core/math/color.h"
#include "radio_core/math/math.h"

namespace radio_core {

// Definition of a color of gradient at the specific coordinate u.
// The coordinate is in the normalized space: 0 means the very first color,
// 1 means the very last color.
template <class ColorType>
class GradientPoint {
 public:
  float u;
  ColorType color;
};

// Rasterize gradient points into a continuous array of colors.
//
// The gradient points are supposed to be ordered ascending by their coordinate.
// They also are expected to cover [0, 1] range.
template <class ColorType>
void RasterizeGradient(const std::span<const GradientPoint<ColorType>> points,
                       std::span<ColorType> pixels) {
  if (points.empty() || pixels.empty()) {
    return;
  }

  using GradientPoint = GradientPoint<ColorType>;

  const size_t num_pixels = pixels.size();
  const float advance_per_pixel = 1.0f / (num_pixels - 1);
  for (size_t i = 0; i < num_pixels; ++i) {
    // Normalized coordinate along the pixels of the entire gradient.
    const float u = advance_per_pixel * float(i);

    // Find gradient point which coordinate is not less than the current
    // coordinate along the gradient pixels.
    const typename std::span<const GradientPoint>::iterator it =
        std::lower_bound(
            points.begin(),
            points.end(),
            u,
            [](const GradientPoint& point, const float upper_u) -> bool {
              return point.u < upper_u;
            });

    // TODO(sergey): Support extreme gradient definitions where gradient points
    // might not cover an entire range of [0, 1].
    assert(it != points.end());

    if (it == points.begin()) {
      // No next point to interpolate with: use the last color.
      pixels[i] = points[0].color;
      continue;
    }

    // Interpolate between two adjacent gradient points.
    const GradientPoint& a = *(it - 1);
    const GradientPoint& b = *(it);
    const float fac = (u - a.u) / (b.u - a.u);
    pixels[i] = Lerp(a.color, b.color, fac);
  }
}

// Sample pixel value of gradient which is closest to the given normalized
// coordinate.
// The coordinate is expected to be within [0, 1] range.
template <class ColorType>
inline auto GradientSampleNearest(const std::span<const ColorType> pixels,
                                  const float u) -> const ColorType& {
  assert(u >= 0.0f);
  assert(u <= 1.0f);

  return pixels[RoundToInt(u * (pixels.size() - 1))];
}

}  // namespace radio_core
