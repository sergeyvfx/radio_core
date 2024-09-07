// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <ios>
#include <optional>
#include <ostream>

#include "radio_core/base/static_vector.h"
#include "radio_core/base/unreachable.h"

namespace radio_core::protocol::datalink {

// Representation of a frame delimeter.
enum class FrameMarker {
  kBegin,
  kEnd,
};

inline auto operator<<(std::ostream& os,
                       const FrameMarker marker) -> std::ostream& {
  switch (marker) {
    case FrameMarker::kBegin: os << "FrameMarker::kBegin"; return os;
    case FrameMarker::kEnd: os << "FrameMarker::kEnd"; return os;
  }

  Unreachable();
}

// Representation of a decoded byte from frame transmission.
//
// Can either be a frame delimeter marker indicating beginning/ending of a frame
// or a byte of a data.
//
// In the actual representation of a marker in the protocol might be different
// from a byte, which does not really make a different here other than possibly
// a bit confusing naming.
//
// The FrameByte is always either a data byte or a marker, never both, and never
// neither of them.
class FrameByte {
 public:
  FrameByte() = default;
  explicit FrameByte(const std::byte byte) : data_(byte) {}
  explicit FrameByte(const FrameMarker marker) : marker_(marker) {}

  // Returns true when this object represents data byte of a frame.
  constexpr auto IsData() const -> bool { return data_.has_value(); }

  // For data type returns the value of the byte.
  // When is called for marker type throws a std::bad_optional_access exception.
  constexpr auto GetData() const -> std::byte { return data_.value(); }

  // Returns true when this object represents frame marker.
  constexpr auto IsMarker() const -> bool { return marker_.has_value(); }

  // When is called for data type throws a std::bad_optional_access exception.
  constexpr auto GetMarker() const -> FrameMarker { return marker_.value(); }

  constexpr auto operator==(const FrameByte& other) const -> bool {
    return data_ == other.data_ && marker_ == other.marker_;
  }
  constexpr auto operator!=(const FrameByte& other) const -> bool {
    return !(*this == other);
  }

 private:
  std::optional<std::byte> data_;
  std::optional<FrameMarker> marker_;
};

inline auto operator<<(std::ostream& os,
                       const FrameByte& frame_byte) -> std::ostream& {
  if (frame_byte.IsMarker()) {
    os << "FrameByte(" << frame_byte.GetMarker() << ")";
  } else if (frame_byte.IsData()) {
    os << "FrameByte(std::byte{0b"
       << std::bitset<8>(std::to_integer<int>(frame_byte.GetData())) << "})";
  } else {
    Unreachable();
  }

  return os;
}

// A collection of frame bytes with a fixed maximum capacity.
//
// Similar to `std::vector<FrameByte>` but the storage is allocated in the
// object itself and it can not contain more than MaxBytes frame bytes.
template <int MaxBytes>
using FrameBytes = StaticVector<FrameByte, MaxBytes>;

template <int MaxBytes>
inline auto operator<<(std::ostream& os,
                       const FrameBytes<MaxBytes>& frame_bytes)
    -> std::ostream& {
  bool is_first = true;
  os << "FrameBytes<" << MaxBytes << ">(";
  for (const FrameByte& frame_byte : frame_bytes) {
    if (!is_first) {
      os << ", ";
    }

    if (frame_byte.IsMarker()) {
      os << frame_byte.GetMarker();
    } else if (frame_byte.IsData()) {
      os << "std::byte{0b"
         << std::bitset<8>(std::to_integer<int>(frame_byte.GetData())) << "}";
    } else {
      Unreachable();
    }

    is_first = false;
  }
  os << ")";
  return os;
}

}  // namespace radio_core::protocol::datalink
