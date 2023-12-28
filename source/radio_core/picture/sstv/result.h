// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Result of a decoding pipeline of SSTV transmission.

#pragma once

#include <cstdint>
#include <span>
#include <variant>

#include "radio_core/base/result.h"
#include "radio_core/base/static_vector.h"
#include "radio_core/math/color.h"
#include "radio_core/picture/sstv/mode.h"

namespace radio_core::picture::sstv {

enum class Error {
  // Generic error of an unknown origin.
  kError,
};

// Valid (in terms of checksum) VIS code has been detected in the transmission.
//
// Note that decoded VIS does not imply that the pixels will be provided. It is
// even possible that there will be no decoded pixels before the next VIS is
// decided.
struct DecodedVISCode {
  uint8_t vis_code{0};
};

// Horizontal line synchronization detected .
struct LineSynchronization {};

// Indicator of beginning of the new image pixels.
//
// This result is generated right before first scanline of the image is
// provided.
//
// Note that it is possible that the decode has been in the middle of decoding
// of other transmission, in which case there will be no explicit pixels end
// result generated.
struct ImagePixelsBegin {
  // Mode of the transmission which is followed to decode and provide pixel
  // values.
  // This defines the resolution of the image.
  Mode mode{Mode::kUnknown};
};

// Row of decoded image pixels.
struct ImagePixelsRow {
  // Pixel values of the row.
  //
  // The number of elements in the span matches the width of the currently
  // decoding picture mode.
  std::span<const Color3ub> pixels;
};

// Indicator that all image pixels have been decoded, and no pixels of the
// current picture will be provided.
//
// Note that this result is only returned when all pixels of the picture have
// been received. In a noisy environment with overlapping transmissions it is
// possible that a stronger SSTV transmission happens before the previous one
// was decoded. In this case there will be no explicit end result.
struct ImagePixelsEnd {};

// Variant of decoded data.
// A type-safe union of all known decoded data.
using DecodedVariant = std::variant<DecodedVISCode,
                                    LineSynchronization,
                                    ImagePixelsBegin,
                                    ImagePixelsRow,
                                    ImagePixelsEnd>;

// A collection of decoded data.
//
// Similar to `std::vector<DecodedVariant>` but the storage is allocated in the
// object itself and it can not contain more than MaxEvents events.
//
// The size allows for 2 decoded scanlines, transmission begin and end markers,
using DecodedData = StaticVector<DecodedVariant, 4>;

// Result of a decoder.
//
// If decoding has failed then it contains an error with the corresponding error
// code.
//
// Otherwise decoded data is stored in a container-like result value, each
// element of which is a variant of possible decoded elements (such as a
// scanline, VIS code, etc). Note that the container might be empty due to
// requirement to have more data to decode. This is not an error.
//
// The proposed way to traverse the decoded result is:
//
//   if (!result.Ok()) {
//     std::cout << "Error detected in SSTV transmission" << std::endl;
//     return;
//   }
//
//   // Iterate over decoded data.
//   for (const DecodedVariant& variant : result.GetValue()) {
//     variant | match{
//                   [](const DecodedVISCode& decoded) { ... },
//               };
//   }
using DecodeResult = radio_core::Result<DecodedData, Error>;

// Create new valid but empty result.
inline auto EmptyDecodeResult() -> DecodeResult {
  return DecodeResult{DecodedData{}};
}

// Combine two decode results.
// If either of the results is not Ok it is returned as-is.
// If both results are Ok their decoded variants are combined.
inline auto Combine(const DecodeResult& result_a, const DecodeResult& result_b)
    -> DecodeResult {
  if (!result_a.Ok()) {
    return result_a;
  }
  if (!result_b.Ok()) {
    return result_b;
  }

  DecodeResult result = result_a;
  result.GetValue().insert(result.GetValue().end(),
                           result_b.GetValue().begin(),
                           result_b.GetValue().end());

  return result;
}

}  // namespace radio_core::picture::sstv
