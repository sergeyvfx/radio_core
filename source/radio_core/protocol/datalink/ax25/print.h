// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

// Print/visualize utilities for AX.25 decoder.

#pragma once

#include <cstdarg>
#include <span>

#include "radio_core/base/string_util.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/protocol/datalink/ax25/control.h"
#include "radio_core/protocol/datalink/ax25/message.h"

namespace radio_core::protocol::datalink::ax25 {

namespace ax25_internal {

// Helper class which takes care of printing message information to the given
// buffer. It ensures that no data is written past the buffer, and that the part
// that is written has null-terminator.
class MessageInfoPrinter {
 public:
  explicit MessageInfoPrinter(const std::span<char> buffer)
      : buffer_(buffer), current_buffer_(buffer) {}

  // Append given string to the buffer, ensuring it is null-terminated.
  void Append(const char* str) {
    const size_t num_copied =
        StringCopy(current_buffer_.data(), str, current_buffer_.size());
    current_buffer_ = current_buffer_.subspan(num_copied);
  }

  // Append string field to the print buffer.
  // This function adds space separator from the previous character written to
  // the buffer.
  void AppendField(const char* str) {
    if (current_buffer_.data() != buffer_.data()) {
      Append(" ");
    }
    Append(str);
  }

  // Similar to the AppendField, but the field value is a format string.
  void AppendFormattedField(const char* format, ...) {
    if (current_buffer_.data() != buffer_.data()) {
      Append(" ");
    }
    va_list arg;
    va_start(arg, format);
    const size_t num_copied = StringPrintFormatList(
        current_buffer_.data(), current_buffer_.size(), format, arg);
    va_end(arg);
    current_buffer_ = current_buffer_.subspan(num_copied);
  }

 private:
  const std::span<char> buffer_;
  std::span<char> current_buffer_;
};

inline auto GetFrameFormatCode(const int control) -> const char* {
  switch (GetControlFormat(control)) {
    case ControlFormat::kUnknown: return ".";
    case ControlFormat::kInformation: return "I";
    case ControlFormat::kSupervisory: return "S";
    case ControlFormat::kUnnumbered: return "U";
  }
  Unreachable();
}

inline void EncodeInformationFrameInfo(const Message& /*message*/,
                                       MessageInfoPrinter& /*printer*/) {}

inline void EncodeSupervisoryFrameInfo(const Message& message,
                                       MessageInfoPrinter& printer) {
  switch (GetSupervisoryFrameControl(message.control)) {
    case SupervisoryFrameControl::kUnknown: break;

    case SupervisoryFrameControl::kReceiveReady:
      printer.AppendField("RR");
      break;
    case SupervisoryFrameControl::kReceiveNotReady:
      printer.AppendField("RNR");
      break;
    case SupervisoryFrameControl::kReject: printer.AppendField("REJ"); break;
    case SupervisoryFrameControl::kSelectiveReject:
      printer.AppendField("SREJ");
      break;
  }
}
}  // namespace ax25_internal

// Encode information about the message into a brief string representation.
// This representation contains frame type, and parsed information about the
// control and PID fields.
inline void EncodeMessageInfo(const Message& message,
                              const std::span<char> buffer) {
  if (buffer.empty()) {
    return;
  }

  ax25_internal::MessageInfoPrinter printer(buffer);

  // Frame format:
  // - Information: I
  // - Supervisory: S
  // - Unnumbered: U
  printer.AppendField(ax25_internal::GetFrameFormatCode(message.control));

  if (message.GetVersion() == 2) {
    if (message.IsCommand()) {
      printer.AppendField("C");
    }
    if (message.IsResponse()) {
      printer.AppendField("R");
    }
  }

  // Poll/Final.
  if (IsPollFrameControl(message.control)) {
    printer.AppendField("P");
  }
  if (IsFinalFrameControl(message.control)) {
    printer.AppendField("F");
  }

  switch (GetControlFormat(message.control)) {
    case ControlFormat::kUnknown: break;
    case ControlFormat::kInformation:
      ax25_internal::EncodeInformationFrameInfo(message, printer);
      break;
    case ControlFormat::kSupervisory:
      ax25_internal::EncodeSupervisoryFrameInfo(message, printer);
      break;
    case ControlFormat::kUnnumbered: break;
  }

  // PID.
  if (FrameControlUsesPID(message.control)) {
    printer.AppendFormattedField("PID=0x%02X", int(message.pid));
  }

  // Information.
  if (FrameControlUsesInfo(message.control)) {
    printer.AppendFormattedField(
        "Len=%d", int(message.information.GetCleanView().size()));
  }
}

}  // namespace radio_core::protocol::datalink::ax25
