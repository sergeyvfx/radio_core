// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Result of a decoding pipeline of APT transmission.

#pragma once

#include <cstdint>
#include <span>
#include <variant>

#include "radio_core/base/result.h"
#include "radio_core/base/static_vector.h"
#include "radio_core/math/color.h"

namespace radio_core::picture::apt {

enum class Error {
  // Generic error of an unknown origin.
  kError,
};

// Horizontal line synchronization: the decoder detected Sync A marker in the
// transmission. The next decoded line will pixels of the picture starting from
// this synchronization marker.
struct LineSynchronization {};

// AN entire line of APT transmission, which consists of the following fields:
//   - Sync A
//   - Space A
//   - Image A
//   - Telemetry A
//   - Sync B
//   - Space B
//   - Image B
//   - Telemetry B
struct Line {
  // Pixel values of the row.
  //
  // The number of elements in the span matches the width of the currently
  // decoding picture mode.
  std::span<const Color1ub> pixels;

  // TODO(sergey): Provide a convenient way to access the individual parts of
  // the line.
};

// Variant of decoded data.
// A type-safe union of all known decoded data.
using DecodedVariant = std::variant<Line, LineSynchronization>;

// A collection of decoded data.
//
// Similar to `std::vector<DecodedVariant>` but the storage is allocated in the
// object itself and it can not contain more than MaxEvents events.
//
// The size allows for decoded line, and synchronization marker.
using DecodedData = StaticVector<DecodedVariant, 2>;

// Result of a decoder.
//
// If decoding has failed then it contains an error with the corresponding error
// code.
//
// Otherwise decoded data is stored in a container-like result value, each
// element of which is a variant of possible decoded elements (such as a line,
// synchronization marker, etc). Note that the container might be empty due to
// requirement to have more data to decode. This is not an error.
//
// The proposed way to traverse the decoded result is:
//
//   if (!result.Ok()) {
//     std::cout << "Error detected in APT transmission" << std::endl;
//     return;
//   }
//
//   // Iterate over decoded data.
//   for (const DecodedVariant& variant : result.GetValue()) {
//     variant | match{
//                   [](const Line& decoded) { ... },
//               };
//   }
using DecodeResult = radio_core::Result<DecodedData, Error>;

// Create new valid but empty result.
inline auto EmptyDecodeResult() -> DecodeResult {
  return DecodeResult{DecodedData{}};
}

}  // namespace radio_core::picture::apt
