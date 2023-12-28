// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core::picture::sstv {

enum class LineEncoding {
  // Unknown or unspecified line encoding.
  kUnknown,

  // YCbCr color space. Lines are encoded in the following manner:
  //
  //   Y0, R-Y, B-Y, Y1
  //
  // where:
  //   Y0   - luminosity of the line N.
  //   R-Y  - Average Cr component of same pixel in lines N and N+1.
  //   B-Y  - Average Cb component of same pixel in lines N and N+1.
  //   Y1   - luminosity of the line N+1.
  kYccAverageCrCb,
};

}  // namespace radio_core::picture::sstv
