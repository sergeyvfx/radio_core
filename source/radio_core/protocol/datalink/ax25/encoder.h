// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of AX.25 data link layer protocol.
//
// Encodes messages and passes them down the stream to a framing protocol, which
// typically is HDLC frame.
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

#include "radio_core/crypto/crc-16-ccitt.h"
#include "radio_core/protocol/datalink/ax25/message.h"
#include "radio_core/protocol/datalink/frame.h"

namespace radio_core::protocol::datalink::ax25 {

class Encoder {
 public:
  Encoder() = default;

  // Encode the message using AX.25 framing.
  //
  // The encoded frame begin, data bytes, and frame end are passed to the
  // callback as an object of FrameByte, one per invocation. The given list of
  // args... is passed to the callback after the frame byte. This makes the
  // required callback signature to be:
  //
  //   callback(const FrameByte& frame_byte, <optional arguments>)
  template <class F, class... Args>
  void operator()(const Message& message, F&& callback, Args&&... args) {
    // Push frame begin marker.
    std::invoke(std::forward<F>(callback),
                FrameByte(FrameMarker::kBegin),
                std::forward<Args>(args)...);

    InitCRC();

    PushAddressField(
        message, std::forward<F>(callback), std::forward<Args>(args)...);
    PushControlField(
        message, std::forward<F>(callback), std::forward<Args>(args)...);
    PushPIDField(
        message, std::forward<F>(callback), std::forward<Args>(args)...);
    PushInfoField(
        message, std::forward<F>(callback), std::forward<Args>(args)...);

    FinalizeCRC();

    PushFCSField(std::forward<F>(callback), std::forward<Args>(args)...);

    // Push frame begin marker.
    std::invoke(std::forward<F>(callback),
                FrameByte(FrameMarker::kEnd),
                std::forward<Args>(args)...);
  }

 private:
  template <class F, class... Args>
  inline void PushAddressField(const Message& message,
                               F&& callback,
                               Args&&... args) {
    PushAddress(message.address.destination,
                true,
                std::forward<F>(callback),
                std::forward<Args>(args)...);
    PushAddress(message.address.source,
                !message.address.repeaters.IsEmpty(),
                std::forward<F>(callback),
                std::forward<Args>(args)...);
    PushAddressRepeaters(
        message, std::forward<F>(callback), std::forward<Args>(args)...);
  }

  template <class F, class... Args>
  inline void PushControlField(const Message& message,
                               F&& callback,
                               Args&&... args) {
    PushByteToCRCAndOutput(message.control,
                           std::forward<F>(callback),
                           std::forward<Args>(args)...);
  }

  template <class F, class... Args>
  inline void PushPIDField(const Message& message,
                           F&& callback,
                           Args&&... args) {
    if (!FrameControlUsesPID(message.control)) {
      return;
    }
    PushByteToCRCAndOutput(
        message.pid, std::forward<F>(callback), std::forward<Args>(args)...);
  }

  template <class F, class... Args>
  inline void PushInfoField(const Message& message,
                            F&& callback,
                            Args&&... args) {
    if (!FrameControlUsesInfo(message.control)) {
      return;
    }

    for (const char ch : message.information) {
      if (ch == '\0') {
        break;
      }
      PushByteToCRCAndOutput(
          ch, std::forward<F>(callback), std::forward<Args>(args)...);
    }
  }

  template <class F, class... Args>
  inline void PushFCSField(F&& callback, Args&&... args) {
    std::invoke(std::forward<F>(callback),
                FrameByte(std::byte(crc_ & 0xff)),
                std::forward<Args>(args)...);
    std::invoke(std::forward<F>(callback),
                FrameByte(std::byte(crc_ >> 8)),
                std::forward<Args>(args)...);
  }

  template <class F, class... Args>
  inline void PushAddress(const Address& address,
                          bool has_repeater,
                          F&& callback,
                          Args&&... args) {
    // The characters of the call sign are standard seven-bit ASCII (upper case
    // only) placed in the left-most seven bits of the octet to make room for
    // the address extension bit. If the call sign contains fewer than six
    // characters, it is padded with ASCII spaces between the last call sign
    // character and the SSID octet.
    for (const char ch : address.callsign) {
      const uint8_t byte = ch;
      PushByteToCRCAndOutput(
          byte << 1, std::forward<F>(callback), std::forward<Args>(args)...);
    }

    // CRRSSSS0
    //
    // - Address extension bit, is set to “0” on all but the last address octet,
    //   where it is set to “1”.
    // - The bits marked “R” are reserved bits. They may be used in an
    //   agreed-upon manner in individual networks. When not implemented, they
    //   are set to one.
    // - The bit marked “C” is the command/response bit of an AX.25 frame.

    // Initialize R bits to 1.
    uint8_t ssid_byte = 0b01100000;

    ssid_byte |= (address.ssid << 1);

    if (address.has_been_repeated) {
      ssid_byte |= 0b10000000;
    }
    if (address.command_response_bit) {
      ssid_byte |= 0b10000000;
    }

    if (!has_repeater) {
      ssid_byte |= 1;
    }

    PushByteToCRCAndOutput(
        ssid_byte, std::forward<F>(callback), std::forward<Args>(args)...);
  }

  template <class F, class... Args>
  inline void PushAddressRepeaters(const Message& message,
                                   F&& callback,
                                   Args&&... args) {
    const size_t num_repeaters = message.address.repeaters.size();
    for (size_t i = 0; i < num_repeaters; ++i) {
      PushAddress(message.address.repeaters[i],
                  i < num_repeaters - 1,
                  std::forward<F>(callback),
                  std::forward<Args>(args)...);
    }
  }

  inline void InitCRC() {
    crc_ = crypto::crc16ccitt::Init<crypto::crc16ccitt::FCS>();
  }
  inline void FinalizeCRC() {
    crc_ = crypto::crc16ccitt::Finalize<crypto::crc16ccitt::FCS>(crc_);
  }
  inline void UpdateCRC(const uint8_t byte) {
    crc_ = crypto::crc16ccitt::Update<crypto::crc16ccitt::FCS>(crc_, byte);
  }

  template <class F, class... Args>
  inline void PushByteToCRCAndOutput(const uint8_t byte,
                                     F&& callback,
                                     Args&&... args) {
    std::invoke(std::forward<F>(callback),
                FrameByte(std::byte{byte}),
                std::forward<Args>(args)...);

    UpdateCRC(byte);
  }

  // CRC of the encoded message.
  // Is calculated incrementally while fields are being transmitted.
  uint16_t crc_ = 0;
};

}  // namespace radio_core::protocol::datalink::ax25
