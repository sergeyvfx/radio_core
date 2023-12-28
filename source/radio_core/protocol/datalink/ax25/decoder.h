// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of AX.25 data link layer protocol.
//
// This decoder takes care of parsing a decoded frame into fields of an AX.25
// message, performing checksum checks. The raw data stream is expected to be
// split into frames using a corresponding frame decoder (such as HDLC decoder).
//
// The decoder supports streamed per-byte decoding as well as decoding of an
// entire frame provided as a span. The latter one is merely a wrapper around
// per-byte decoder API.
//
// Protocol specification:
//
//   https://www.tapr.org/pdf/AX25.2.2.pdf
//
//
// Online AX.25 Frame Generator:
//
//   https://notblackmagic.com/bitsnpieces/ax.25/
//
// PPP in HDLC Framing RFC. Explains CRC/FCS field:
//
//   https://www.ietf.org/rfc/rfc1549.txt

#pragma once

#include <functional>
#include <optional>
#include <span>

#include "radio_core/base/result.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/crypto/crc-16-ccitt.h"
#include "radio_core/protocol/datalink/ax25/message.h"
#include "radio_core/protocol/datalink/frame.h"

namespace radio_core::protocol::datalink::ax25 {

class Decoder {
 public:
  enum class Error {
    // Given data has been processed but the message is not complete yet and
    // hence is not available for access.
    //
    // This code does not indicate a processing or protocol error, it just
    // indicates that more data is needed to fully decode message from the
    // transmission.
    kUnavailable,

    // A message was captured, but its actual checksum did not match the one
    // provided in the FCS.
    // The received state of the message is present in the result so that the
    // caller might attempt to perform bit correction.
    kChecksumMismatch,

    // The decoding frame is too large to fit into the message.
    kResourceExhausted,
  };

  using Result = radio_core::Result<std::reference_wrapper<Message>, Error>;

  Decoder() { ResetIfNeeded(); }

  // Process frame marker.
  //
  // When processing new frame marker the currently decoding message is
  // discarded. In practice this means couple of things:
  //
  //  - If framing protocol uses implicit frame-end marker when a new-frame
  //    marker is uses it is up to the user of this decoder needs to make the
  //    flags explicit.
  //
  //  - The currently decoding message is discarded. This means that even if it
  //    is only a frame-end flag has been missed from a transmission the message
  //    is not decoded at all.
  //
  // Processing `FrameMarker::kBegin` marker returns Error::kUnavailable code.
  //
  // Processing `FrameMarker::kEnd` marker returns a reference to the decoded
  // message if is it fully decoded and the CRC matched. If all the message
  // fields are decoded but the CRC did not match then Error::CHECKSUM_MISMATCH
  // code is returned with the reference to the current state of the message.
  // This gives caller a possibility to perform bit correction.
  //
  // The message is invalidated by the next processing function call.
  inline auto operator()(const FrameMarker marker) -> Result {
    switch (marker) {
      case FrameMarker::kBegin: {
        ResetIfNeeded();
        return Result(Error::kUnavailable);
      }

      case FrameMarker::kEnd: {
        // Make sure all the "delayed" bytes from the information buffer are
        // taken into account.
        if (field_state_ == FieldState::kInformation) {
          FinalizeFCS();
          return FlushInformationBytes();
        }

        return Result(Error::kUnavailable);
      }
    }

    UNREACHABLE();
  }

  // Process single byte of an AX.25 frame.
  auto operator()(const std::byte new_byte) -> Result {
    // New frame byte has been provided, the decoding has started.
    is_reset_ = false;

    // Convert to an integral type to simplify bit magic (make it less verbose)
    // and to simplify cast to boolean and wide integer types (such as uint16_t
    // and and uint32_t).
    const auto byte_value = std::to_integer<uint8_t>(new_byte);

    // Central place to handle CRC calculation.
    // It is needed for all stages except to when the data stream is out of the
    // frame and hence all the data bytes are ignored.
    if (field_state_ != FieldState::kFrameSkip) {
      ProcessFCSByte(byte_value);
    }

    switch (field_state_) {
      case FieldState::kAddressDestination:
        return ProcessAddressDestinationByte(byte_value);
      case FieldState::kAddressSource:
        return ProcessAddressSourceByte(byte_value);
      case FieldState::kAddressRepeater:
        return ProcessAddressRepeaterByte(byte_value);

      case FieldState::kControl: return ProcessControlByte(byte_value);

      case FieldState::kPID: return ProcessPIDByte(byte_value);

      case FieldState::kInformation: return ProcessInformationByte(byte_value);

      case FieldState::kFrameSkip: return Result(Error::kUnavailable);
    }

    UNREACHABLE();
  }

  // Process the entire frame.
  //
  // Upon a successfully finished decoding the result contains a reference to
  // the decoded message. The message is invalidated by the next processing
  // function call.
  //
  // This is a shortcut of a "streamed" processing which affects the current
  // state of the decoder.
  auto operator()(const std::span<const std::byte> frame) -> Result {
    const Result begin_result = (*this)(FrameMarker::kBegin);
    if (!begin_result.Ok() && begin_result.GetError() != Error::kUnavailable) {
      return begin_result;
    }

    for (const std::byte byte : frame) {
      const Result result = (*this)(byte);
      if (!result.Ok() && result.GetError() != Error::kUnavailable) {
        return result;
      }
    }

    return (*this)(FrameMarker::kEnd);
  }

  // Process frame byte which is either a marker of a data.
  //
  // Upon a successfully finished decoding the result contains a reference to
  // the decoded message. The message is invalidated by the next processing
  // function call.
  auto operator()(const FrameByte& frame_byte) -> Result {
    if (frame_byte.IsData()) {
      return (*this)(frame_byte.GetData());
    }

    if (frame_byte.IsMarker()) {
      return (*this)(frame_byte.GetMarker());
    }

    UNREACHABLE();
  }

 private:
  inline void ResetIfNeeded() {
    if (is_reset_) {
      return;
    }
    is_reset_ = true;

    field_state_ = FieldState::kAddressDestination;

    address_state_.Clear();
    information_state_.Clear();
    fcs_state_.Clear();

    message_.Clear();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Address field.

  auto ProcessAddressDestinationByte(const uint8_t byte) -> Result {
    if (ProcessAddressCallsignByte(byte)) {
      return Result(Error::kUnavailable);
    }

    // SSID Encoding: CRRSSSS0
    //
    // - The first (low-order or bit 0) bit of each octet is the HDLC address
    //   extension bit, set to zero on all but the last octet in the address
    //   field, where it is set to one.
    //
    // - The bits marked “R” are reserved bits. They may be used in an
    //   agreed-upon manner in individual networks. When not implemented, they
    //   are set to one.
    //
    // - The bit marked “C” is the command/response bit of an AX.25 frame, as
    //   outlined in Section 6.1.2 of the specification.
    //
    // - The characters of the call sign are standard seven-bit ASCII (upper
    //   case only) placed in the left-most seven bits of the octet to make room
    //   for the address extension bit. If the call sign contains fewer than six
    //   characters, it is padded with ASCII spaces between the last call sign
    //   character and the SSID octet.
    //
    // - The 0000 SSID is reserved for the first personal AX.25 station. This
    //   provision establishes one standard SSID for “normal” stations to use
    //   for the first station.

    // Finalize address decoding.
    address_state_.address.ssid = (byte >> 1) & 0xf;
    address_state_.address.has_been_repeated = false;
    address_state_.extension_bit = (byte & 1);

    // Assign the address to the message.
    message_.address.destination = address_state_.address;

    // Move to the next state of decoding.
    address_state_.Clear();
    field_state_ = FieldState::kAddressSource;

    return Result(Error::kUnavailable);
  }

  auto ProcessAddressSourceByte(const uint8_t byte) -> Result {
    if (ProcessAddressCallsignByte(byte)) {
      return Result(Error::kUnavailable);
    }

    // SSID Encoding: CRRSSSS0
    //
    // - The first (low-order or bit 0) bit of each octet is the HDLC address
    //   extension bit, set to zero on all but the last octet in the address
    //   field, where it is set to one.
    //
    //  - The bits marked “R” are reserved bits. They may be used in an
    //    agreed-upon manner in individual networks. When not implemented, they
    //    are set to one.
    //
    //  - The bit marked “C” is the command/response bit of an LA PA frame, as
    //    outlined in Section 6.1.2 of the specification.
    //
    // - The characters of the call sign are standard seven-bit ASCII (upper
    //   case only) placed in the leftmost seven bits of the octet to make room
    //   for the address extension bit. If the call sign contains fewer than six
    //   characters, it is padded with ASCII spaces between the last call sign
    //   character and the SSID octet.
    //
    // - The 0000 SSID is reserved for the first personal AX.25 station. This
    //   provision establishes one standard SSID for “normal” stations to use
    //   for the first station.

    // Finalize address decoding.
    address_state_.address.ssid = (byte >> 1) & 0xf;
    address_state_.address.has_been_repeated = false;
    address_state_.extension_bit = (byte & 1);

    // Assign the address to the message.
    message_.address.source = address_state_.address;

    // Move to the next state of decoding.
    if (address_state_.extension_bit == 0) {
      address_state_.Clear();
      field_state_ = FieldState::kAddressRepeater;
    } else {
      field_state_ = FieldState::kControl;
    }

    return Result(Error::kUnavailable);
  }

  auto ProcessAddressRepeaterByte(const uint8_t byte) -> Result {
    if (ProcessAddressCallsignByte(byte)) {
      return Result(Error::kUnavailable);
    }

    // SSID Encoding: HRRSSSS1
    //
    // - As with the source and destination address subfields discussed above,
    //   bit 0 of each octet is the HDLC
    //
    // - address extension bit, is set to “0” on all but the last address octet,
    //   where it is set to “1”.
    //
    // - The “R” bits are reserved in the same manner as in the source and
    //   destination subfields.
    //
    // - The “H” bit is the has-been-repeated bit. It is set to “0” when a frame
    //   has not been repeated, and set to “1” by the repeating station when
    //   repeated.

    // Finalize address decoding.
    address_state_.address.ssid = (byte >> 1) & 0xf;
    address_state_.address.has_been_repeated = (byte & 0b10000000);
    address_state_.extension_bit = (byte & 1);

    // Assign the address to the message.
    if (!message_.address.repeaters.TryAppend(address_state_.address)) {
      return Result(Error::kResourceExhausted);
    }

    // Move to the next state of decoding.
    if (address_state_.extension_bit == 0) {
      address_state_.Clear();
    } else {
      field_state_ = FieldState::kControl;
    }

    return Result(Error::kUnavailable);
  }

  // Push incoming byte to the callsign field of the current address.
  // If the byte still firs into the callsign it is appended to the callsign and
  // true is returned.
  // If the callsign is fully received then false is returned and the address
  // state is left unchanged.
  auto ProcessAddressCallsignByte(const uint8_t byte) -> bool {
    Callsign& callsign = address_state_.address.callsign;

    if (address_state_.num_received_bytes < callsign.GetCapacity()) {
      // The characters of the call sign are standard seven-bit ASCII (upper
      // case only) placed in the left-most seven bits of the octet to make room
      // for the address extension bit. If the call sign contains fewer than six
      // characters, it is padded with ASCII spaces between the last call sign
      // character and the SSID octet.

      const char ch = char(byte >> 1);
      callsign[address_state_.num_received_bytes++] = ch;
      return true;
    }

    // NOTE: This is not an error, is just the callsign has been fully processed
    // and other parts of the address are to be parsed.

    return false;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Control field.

  auto ProcessControlByte(const uint8_t byte) -> Result {
    message_.control = byte;

    if (message_.UsesPIDField()) {
      field_state_ = FieldState::kPID;
    } else {
      // TODO(sergey): Support message types which do not use PID field.
      field_state_ = FieldState::kFrameSkip;
    }

    return Result(Error::kUnavailable);
  }

  //////////////////////////////////////////////////////////////////////////////
  // PID field.

  auto ProcessPIDByte(const uint8_t byte) -> Result {
    message_.pid = byte;

    if (message_.UsesInfoField()) {
      field_state_ = FieldState::kInformation;
    } else {
      // TODO(sergey): Support message types which do not use Info field.
      field_state_ = FieldState::kFrameSkip;
    }

    return Result(Error::kUnavailable);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Information field.

  auto ProcessInformationByte(uint8_t byte) -> Result {
    if (information_state_.num_data_bytes == 4) {
      const char ch = char(information_state_.data & 0xff);

      const Result result = ProcessInformationChar(ch);
      if (!result.Ok() && result.GetError() != Error::kUnavailable) {
        return result;
      }

      information_state_.data >>= 8;
      information_state_.data |= byte << 24;
      return Result(Error::kUnavailable);
    }

    information_state_.data |= byte << (information_state_.num_data_bytes * 8);
    ++information_state_.num_data_bytes;

    return Result(Error::kUnavailable);
  }

  auto FlushInformationBytes() -> Result {
    if (information_state_.num_data_bytes < 2) {
      // We expect at least 2 bytes from FCS field.
      return Result(Error::kUnavailable);
    }

    while (information_state_.num_data_bytes > 2) {
      const char ch = char(information_state_.data & 0xff);

      const Result result = ProcessInformationChar(ch);
      if (!result.Ok() && result.GetError() != Error::kUnavailable) {
        return result;
      }

      information_state_.data >>= 8;
      --information_state_.num_data_bytes;
    }

    // At this point the delayed buffer in the information state contains the
    // last 2 bytes of the frame which correspond to the FCS field.
    const uint16_t received_fcs = information_state_.data;

    if (received_fcs != fcs_state_.actual_frame_fcs) {
      return {message_, Error::kChecksumMismatch};
    }

    return Result(message_);
  }

  inline auto ProcessInformationChar(const char ch) -> Result {
    if (information_state_.length >= message_.information.GetCapacity()) {
      return Result(Error::kResourceExhausted);
    }
    message_.information[information_state_.length++] = ch;
    return Result(Error::kUnavailable);
  }

  //////////////////////////////////////////////////////////////////////////////
  // FCS field.

  inline void ProcessFCSByte(const uint8_t byte) {
    if (fcs_state_.num_data_bytes == 2) {
      fcs_state_.actual_frame_fcs = crypto::crc16ccitt::Update<FCSSpec>(
          fcs_state_.actual_frame_fcs, fcs_state_.data & 0xff);
    } else {
      ++fcs_state_.num_data_bytes;
    }

    fcs_state_.data = (fcs_state_.data >> 8) | (byte << 8);
  }

  inline void FinalizeFCS() {
    fcs_state_.actual_frame_fcs =
        crypto::crc16ccitt::Finalize<FCSSpec>(fcs_state_.actual_frame_fcs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Denotes whether the state is in reset state.
  // Used to avoid unneeded redundant resets.
  bool is_reset_{false};

  // State of the decoder: denotes which field is currently being decoded.
  enum class FieldState {
    // Parts of the Address field.
    kAddressDestination,
    kAddressSource,
    kAddressRepeater,

    kControl,

    kPID,

    kInformation,

    // Ignore the rest of the frame.
    // Used for unknown or unsupported messages.
    kFrameSkip,
  };
  FieldState field_state_;

  struct {
    inline void Clear() {
      num_received_bytes = 0;

      address.Clear();

      extension_bit = false;
    }

    // Number of received bytes in the address field.
    int num_received_bytes;

    // Partially decoded address.
    Address address;

    // HDLC address extension bit. Set to zero on all but the last octet in the
    // address field, where it is set to one.
    bool extension_bit;
  } address_state_;

  // State of information field decoding.
  struct {
    inline void Clear() {
      data = 0;
      num_data_bytes = 0;
      length = 0;
    }

    // The information field decoding uses a sliding window to tell bytes of the
    // information field from the FCS field apart (this is because the length of
    // the information field is not known in advance).
    //
    // This data field contains `num_data_bytes` bytes of information.
    uint32_t data;
    int num_data_bytes;

    // Current length of the information field.
    int length;
  } information_state_;

  using FCSSpec = crypto::crc16ccitt::FCS;

  struct {
    inline void Clear() {
      actual_frame_fcs = crypto::crc16ccitt::Init<FCSSpec>();
      data = 0;
      num_data_bytes = 0;
    }

    // CRC of the frame calculated from the provided data bytes.
    uint16_t actual_frame_fcs;

    // The FCS calculation ignores two last bytes of the frame (as those belong
    // to FCS transmitted with the message).
    // This is a cyclic buffer used to ensure that the transmitted FCS is
    // ignoed: the actual FCS is calculated once this buffer goes above 2 bytes.
    uint32_t data;
    int num_data_bytes;
  } fcs_state_;

  // Partially decoded message.
  Message message_;
};

}  // namespace radio_core::protocol::datalink::ax25
