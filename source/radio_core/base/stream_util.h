// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>
#include <type_traits>

namespace radio_core {

// An utility class which provides RAII stream state storing and restoring.
//
// Example:
//
//   void PrintTo(const int value, std::ostream& os) {
//     const StreamState stream_state(os);
//     os << std::hex << value;
//   }
template <class CharT, class Traits = typename std::char_traits<CharT>>
class StreamState {
 public:
  explicit StreamState(std::basic_ios<CharT, Traits>& stream)
      : stream_{stream},
        flags_{stream.flags()},
        locale_{stream.getloc()},
        precision_{stream.precision()},
        width_{stream.width()},
        fill_{stream.fill()} {}

  StreamState(const StreamState& other) = delete;
  StreamState(StreamState&& other) noexcept = delete;

  ~StreamState() {
    stream_.flags(flags_);
    stream_.imbue(locale_);
    stream_.precision(precision_);
    stream_.width(width_);
    stream_.fill(fill_);
  }

  auto operator=(const StreamState& other) -> StreamState& = delete;
  auto operator=(StreamState&& other) -> StreamState& = delete;

 private:
  std::basic_ios<CharT, Traits>& stream_;
  std::ios_base::fmtflags flags_;
  std::locale locale_;
  std::streamsize precision_;
  std::streamsize width_;
  CharT fill_;
};

}  // namespace radio_core
