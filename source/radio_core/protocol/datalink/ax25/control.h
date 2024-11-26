// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

// Control field information and manipulation.
//
// Follows description of the AX.25 Link Access Protocol:
//
//  http://www.ax25.net/AX25.2.2-Jul%2098-2.pdf

#pragma once

#include <ostream>

#include "radio_core/base/unreachable.h"

namespace radio_core::protocol::datalink::ax25 {

// Constants which brings semantic meaning to different bits of the control
// field.
class ControlBits {
 public:
  // Control field type.
  //
  // The mask denotes which bits of the control field are responsible for
  // defining corresponding type (different types are indicated by the different
  // set of bits).
  //
  //   ┌─────────────────────────┬───────────────────────────┐
  //   │                         │    Control-Field Bits     │
  //   │   Control Field Type    ├─────────┬─────┬───────┬───┤
  //   │                         │ 7  6  5 │  4  │ 3 2 1 │ 0 │
  //   ├─────────────────────────┼─────────┼─────┼───────┼───┤
  //   │         I Frame         │  N(R)   │  P  │ N(S)  │ 0 │
  //   ├─────────────────────────┼─────────┼─────┼───────┴───┤
  //   │         S Frame         │  N(R)   │ P/F │ S S 0   1 │
  //   ├─────────────────────────┼─────────┼─────┼───────────┤
  //   │         U Frame         │  M M M  │ P/F │ M M 1   1 │
  //   └─────────────────────────┴─────────┴─────┴───────────┘
  struct Type {
    inline static constexpr int kInformation = 0b00000000;
    inline static constexpr int kInformationMask = 0b00000001;

    inline static constexpr int kSupervisory = 0b00000001;
    inline static constexpr int kSupervisoryMask = 0b00000011;

    inline static constexpr int kUnnumbered = 0b00000011;
    inline static constexpr int kUnnumberedMask = 0b00000011;
  };

  // Format of the INFORMATION (I) frame type.
  //
  // The information (I) command transfers sequentially-numbered frames
  // containing an information field across a data link.
  //
  //   ┌─────────────────────────┬───────────────────────────┐
  //   │                         │    Control-Field Bits     │
  //   │   Control Field Type    ├─────────┬─────┬───────┬───┤
  //   │                         │ 7  6  5 │  4  │ 3 2 1 │ 0 │
  //   ├─────────────────────────┼─────────┼─────┼───────┼───┤
  //   │         I Frame         │  N(R)   │  P  │ N(S)  │ 0 │
  //   └─────────────────────────┴─────────┴─────┴───────┴───┘
  struct Information {
    // Bit corresponding to the P/Flag.
    // When the bit is set the frame is Final, otherwise its Poll.
    inline static constexpr int kFinalBit = 0b00010000;
  };

  // Format of the SUPERVISORY (S) frame type.
  //   ┌─────────────────────────┬───────────────────────────┐
  //   │                         │    Control-Field Bits     │
  //   │   Control Field Type    ├─────────┬─────┬─────┬─────┤
  //   │                         │ 7  6  5 │  4  │ 3 2 │ 1 0 │
  //   ├─────────────────────────┼─────────┼─────┼─────┼─────┤
  //   │ Receive Ready (RR)      │  N(R)   │ P/F │ 0 0 │ 0 1 │
  //   ├─────────────────────────┼─────────┼─────┼─────┼─────┤
  //   │ Receive Not Ready (RNR) │  N(R)   │ P/F │ 0 1 │ 0 1 │
  //   ├─────────────────────────┼─────────┼─────┼─────┼─────┤
  //   │ Reject (REJ)            │  N(R)   │ P/F │ 1 0 │ 0 1 │
  //   ├─────────────────────────┼─────────┼─────┼─────┼─────┤
  //   │ Selective Reject (SREJ) │  N(R)   │ P/F │ 1 1 │ 0 1 │
  //   └─────────────────────────┴─────────┴─────┴─────┴─────┘
  struct Supervisory {
    // Bit corresponding to the P/Flag.
    // When the bit is set the frame is Final, otherwise its Poll.
    inline static constexpr int kFinalBit = 0b00010000;

    inline static constexpr int kMask = 0b00001111;

    // Receive Ready (RR) Command and Response.
    inline static constexpr int kReceiveReady = 0b00000001;
    // Receive Not Ready (RNR) Command and Response.
    inline static constexpr int kReceiveNotReady = 0b00000101;
    // Reject (REJ) Command and Response.
    inline static constexpr int kReject = 0b00001001;
    // Selective Reject (SREJ) Command and Response.
    inline static constexpr int kSelectiveReject = 0b00001101;
  };

  // Format of the UNNUMBERED (U) frame type.
  //
  // The mask denotes bits which are set by the command or a response (as in,
  // the bit 4 denotes pull/final bit, the rest bits are denotes the command or
  // response).
  //
  // The values include the frame type bits.
  //
  //   ┌───────────────────────┬──────────────┬───────────────────────────┐
  //   │                       │              │    Control-Field Bits     │
  //   │   Control Field Type  │     Type     ├─────────┬─────┬─────┬─────┤
  //   │                       │              │ 7  6  5 │  4  │ 3 2 │ 1 0 │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Set Asynchronous     │              │         │     │     │     │
  //   │  Balanced Mode        │   Command    │  0 1 1  │  P  │ 1 1 │ 1 1 │
  //   │  Extended (SABME)     │              │         │     │     │     │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Set Asynchronous     │              │         │     │     │     │
  //   │  Balanced Mode        │   Command    │  0 0 1  │  P  │ 1 1 │ 1 1 │
  //   │  (SABM)               │              │         │     │     │     │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Disconnect (DISC)    │   Command    │  0 1 0  │  P  │ 0 0 │ 1 1 │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Disconnect Mode (DM) │   Response   │  0 0 0  │  F  │ 1 1 │ 1 1 │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Unnumbered           │   Response   │  0 1 1  │  F  │ 0 0 │ 1 1 │
  //   │  Acknowledge (UA)     │              │         │     │     │     │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Frame Reject (FRMR)  │   Response   │  1 0 0  │  F  │ 0 1 │ 1 1 │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Unnumbered           │   Either     │  0 0 0  │ P/F │ 0 0 │ 1 1 │
  //   │  Information (UI)     │              │         │     │     │     │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Exchange             │   Either     │  1 0 1  │ P/F │ 1 1 │ 1 1 │
  //   │  Identification (XID) │              │         │     │     │     │
  //   ├───────────────────────┼──────────────┼─────────┼─────┼─────┼─────┤
  //   │  Test (TEST)          │   Either     │  1 1 1  │ P/F │ 0 0 │ 1 1 │
  //   └───────────────────────┴──────────────┴─────────┴─────┴─────┴─────┘
  struct Unnumbered {
    // Bit corresponding to the P/Flag.
    // When the bit is set the frame is Final, otherwise its Poll.
    inline static constexpr int kFinalBit = 0b00010000;

    inline static constexpr int kMASK = 0b11101111;

    // Set Asynchronous Balanced Mode Extended (SABME), command.
    inline static constexpr int kSABME = 0b01101111;
    // Set Asynchronous Balanced Mode (SABM), command.
    inline static constexpr int kSABM = 0b00101111;
    // Disconnect (DISC), command
    inline static constexpr int kDISC = 0b01000011;
    // Disconnect Mode (DM), response
    inline static constexpr int kDM = 0b00001111;
    // Unnumbered Acknowledge (UA), response
    inline static constexpr int kUA = 0b01100011;
    // Frame Reject (FRMR) Response
    inline static constexpr int kFRMR = 0b10000111;
    // Unnumbered Information (UI), either
    inline static constexpr int kUI = 0b00000011;
    // Exchange Identification (XID), either.
    inline static constexpr int kXID = 0b10101111;
    // Test (TEST), either.
    inline static constexpr int kTEST = 0b11100011;
  };
};

// Formats of control fields used in AX.25.
enum class ControlFormat {
  kUnknown,

  kInformation,
  kSupervisory,
  kUnnumbered,
};
inline auto operator<<(std::ostream& os, const ControlFormat format)
    -> std::ostream& {
  switch (format) {
    case ControlFormat::kUnknown: os << "Unknown"; return os;
    case ControlFormat::kInformation: os << "Information"; return os;
    case ControlFormat::kSupervisory: os << "Supervisory"; return os;
    case ControlFormat::kUnnumbered: os << "Unnumbered"; return os;
  }
  Unreachable();
}

enum class SupervisoryFrameControl {
  kUnknown,

  kReceiveReady,
  kReceiveNotReady,
  kReject,
  kSelectiveReject,
};

// Get control format from an encoded control field.
inline auto GetControlFormat(const int control) -> ControlFormat {
  // Information.
  if ((control & ControlBits::Type::kInformationMask) ==
      ControlBits::Type::kInformation) {
    return ControlFormat::kInformation;
  }

  // Supervisory.
  if ((control & ControlBits::Type::kSupervisoryMask) ==
      ControlBits::Type::kSupervisory) {
    return ControlFormat::kSupervisory;
  }

  // Unnumbered.
  if ((control & ControlBits::Type::kUnnumberedMask) ==
      ControlBits::Type::kUnnumbered) {
    return ControlFormat::kUnnumbered;
  }

  return ControlFormat::kUnknown;
}

// Get supervisory control field from the encoded control field.
inline auto GetSupervisoryFrameControl(const int control)
    -> SupervisoryFrameControl {
  if (GetControlFormat(control) != ControlFormat::kSupervisory) {
    return SupervisoryFrameControl::kUnknown;
  }

  switch (control & ControlBits::Supervisory::kMask) {
    case ControlBits::Supervisory::kReceiveReady:
      return SupervisoryFrameControl::kReceiveReady;
    case ControlBits::Supervisory::kReceiveNotReady:
      return SupervisoryFrameControl::kReceiveNotReady;
    case ControlBits::Supervisory::kReject:
      return SupervisoryFrameControl::kReject;
    case ControlBits::Supervisory::kSelectiveReject:
      return SupervisoryFrameControl::kSelectiveReject;
  }

  return SupervisoryFrameControl::kUnknown;
}

// Returns true if the frame has P/F bit set to Final.
inline auto IsFinalFrameControl(const int control) -> bool {
  switch (GetControlFormat(control)) {
    case ControlFormat::kUnknown: return false;
    case ControlFormat::kInformation:
      return control & ControlBits::Information::kFinalBit;
    case ControlFormat::kSupervisory:
      return control & ControlBits::Supervisory::kFinalBit;
    case ControlFormat::kUnnumbered:
      return control & ControlBits::Unnumbered::kFinalBit;
  }
  Unreachable();
}

// Returns true if the frame has P/F bit set to Poll.
inline auto IsPollFrameControl(const int control) -> bool {
  return !IsFinalFrameControl(control);
}

// Check whether PID field is used by the message with the format denoted by the
// given control.
//
// The Protocol Identifier (PID) field appears in information frames (I and
// UI) only.
inline auto FrameControlUsesPID(const int control) -> bool {
  if ((control & ControlBits::Unnumbered::kMASK) ==
      ControlBits::Unnumbered::kUI) {
    return true;
  }

  if ((control & ControlBits::Type::kInformationMask) ==
      ControlBits::Type::kInformation) {
    return true;
  }

  return false;
}

// Check whether Info field is used by the message with the format denoted by
// the given control.
inline auto FrameControlUsesInfo(const int control) -> bool {
  if ((control & ControlBits::Unnumbered::kMASK) ==
      ControlBits::Unnumbered::kUI) {
    return true;
  }

  if ((control & ControlBits::Type::kInformationMask) ==
      ControlBits::Type::kInformation) {
    return true;
  }

  return false;
}

}  // namespace radio_core::protocol::datalink::ax25
