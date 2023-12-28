// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/picture/memory_pixel_accessor.h"

#include <array>
#include <cstdint>

#include "radio_core/math/color.h"
#include "radio_core/unittest/test.h"

namespace radio_core::picture {

TEST(ConstMemoryPixelAccessor, Read_Grayscale_From_Grayscale) {
  constexpr std::array<uint8_t, 1> pixels = {32};

  const ConstMemoryPixelAccessor<Color1ub> accessor(
      pixels, {.width = 1, .height = 1, .num_channels = 1});

  EXPECT_EQ(accessor.GetPixel(0, 0), Color1ub(32));
}

TEST(ConstMemoryPixelAccessor, Read_Grayscale_From_RGB) {
  constexpr std::array<uint8_t, 3> pixels = {32, 64, 128};

  const ConstMemoryPixelAccessor<Color1ub> accessor(
      pixels, {.width = 1, .height = 1, .num_channels = 3});

  EXPECT_EQ(accessor.GetPixel(0, 0), Color1ub(61));
}

TEST(ConstMemoryPixelAccessor, Read_RGB_From_Grayscale) {
  constexpr std::array<uint8_t, 1> pixels = {32};

  const ConstMemoryPixelAccessor<Color3ub> accessor(
      pixels, {.width = 1, .height = 1, .num_channels = 1});

  EXPECT_EQ(accessor.GetPixel(0, 0), Color3ub(32, 32, 32));
}

TEST(ConstMemoryPixelAccessor, Read_RGB_From_RGB) {
  constexpr std::array<uint8_t, 3> pixels = {32, 64, 128};

  const ConstMemoryPixelAccessor<Color3ub> accessor(
      pixels, {.width = 1, .height = 1, .num_channels = 3});

  EXPECT_EQ(accessor.GetPixel(0, 0), Color3ub(32, 64, 128));
}

}  // namespace radio_core::picture
