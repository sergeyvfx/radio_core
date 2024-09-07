// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <ostream>

#include "radio_core/base/unreachable.h"

namespace radio_core::picture::sstv {

enum class Mode {
  // Mode was not specified or specified binary representation does not
  // correspond to any known mode.
  kUnknown = -1,

  kPD90 = 0x63,
  kPD120 = 0x5f,
  kPD160 = 0x62,
  kPD180 = 0x60,
  kPD240 = 0x61,
  kPD290 = 0x5e,

  // TODO(sergey): Extend to all other modes. Need to find a reliable source!
};

// The canonical abbreviated name of the mode.
// For example, "PD90".
constexpr inline auto GetName(const Mode mode) -> const char* {
  switch (mode) {
    case Mode::kUnknown: return "Unknown";
    case Mode::kPD90: return "PD90";
    case Mode::kPD120: return "PD120";
    case Mode::kPD160: return "PD160";
    case Mode::kPD180: return "PD180";
    case Mode::kPD240: return "PD240";
    case Mode::kPD290: return "PD290";
  }
  Unreachable();
}

inline auto operator<<(std::ostream& os, const Mode mode) -> std::ostream& {
  os << GetName(mode);
  return os;
}

// Convert integer representation of VIS code to the Mode enumerator.
// If the mode is not known then Mode::kUnknown is returned.
inline auto GetModeFromVISCode(const uint8_t vis_code) -> Mode {
  switch (static_cast<Mode>(vis_code)) {
    case Mode::kUnknown: return Mode::kUnknown;
    case Mode::kPD90: return Mode::kPD90;
    case Mode::kPD120: return Mode::kPD120;
    case Mode::kPD160: return Mode::kPD160;
    case Mode::kPD180: return Mode::kPD180;
    case Mode::kPD240: return Mode::kPD240;
    case Mode::kPD290: return Mode::kPD290;
  }
  return Mode::kUnknown;
}

// Convert Mode enumerator to an integer VIS code.
inline auto GetVISCodeFromMode(const Mode mode) -> uint8_t {
  return static_cast<uint8_t>(mode);
}

}  // namespace radio_core::picture::sstv
