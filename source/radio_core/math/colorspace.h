// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Operations to convert colors between different color spaces.

#pragma once

#include "radio_core/math/color.h"

namespace radio_core {

// Formula from Python Imaging Library:
//
//   R  = Y +                       + (Cr - 128) *  1.40200
//   G  = Y + (Cb - 128) * -0.34414 + (Cr - 128) * -0.71414
//   B  = Y + (Cb - 128) *  1.77200
//
// Adopted to work with floating point values in the range of [0 .. 1].
template <class T>
inline constexpr auto YCbCrToRGB(const Color3<T>& ycc) -> Color3<T> {
  Color3f rgb;

  rgb.rgb.r = ycc.ycc.y + (ycc.ycc.cr - T(0.5)) * T(1.40200);
  rgb.rgb.g = ycc.ycc.y + (ycc.ycc.cb - T(0.5)) * T(-0.34414) +
              (ycc.ycc.cr - T(0.5)) * T(-0.71414);
  rgb.rgb.b = ycc.ycc.y + (ycc.ycc.cb - T(0.5)) * T(1.77200);

  return rgb;
}

// Formula from Python Imaging Library:
//
//   Y  = R *  0.29900 + G *  0.58700 + B *  0.11400
//   Cb = R * -0.16874 + G * -0.33126 + B *  0.50000 + 128
//   Cr = R *  0.50000 + G * -0.41869 + B * -0.08131 + 128
//
// Adopted to work with floating point values in the range of [0 .. 1].
template <class T>
inline constexpr auto RGBToYCbCr(const Color3<T>& rgb) -> Color3<T> {
  Color3f ycc;

  ycc.ycc.y =
      rgb.rgb.r * T(0.29900) + rgb.rgb.g * T(0.58700) + rgb.rgb.b * T(0.11400);
  ycc.ycc.cb = rgb.rgb.r * T(-0.16874) + rgb.rgb.g * T(-0.33126) +
               rgb.rgb.b * T(0.50000) + T(0.5);
  ycc.ycc.cr = rgb.rgb.r * T(0.50000) + rgb.rgb.g * T(-0.41869) +
               rgb.rgb.b * T(-0.08131) + T(0.5);

  return ycc;
}

}  // namespace radio_core
