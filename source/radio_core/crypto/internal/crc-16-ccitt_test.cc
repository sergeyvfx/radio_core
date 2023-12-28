// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/crypto/crc-16-ccitt.h"

#include <string_view>

#include "radio_core/unittest/test.h"

namespace radio_core::crypto {

namespace {

template <class Parametrization>
auto CalculateCRC(const std::string_view str) -> uint16_t {
  uint16_t crc = crc16ccitt::Init<Parametrization>();
  for (const char ch : str) {
    crc = crc16ccitt::Update<Parametrization>(crc, static_cast<uint8_t>(ch));
  }
  crc = crc16ccitt::Finalize<Parametrization>(crc);

  return crc;
}

}  // namespace

TEST(crc16ccitt, FCS) {
  // The following sources were used to validate correctness of the CRC:
  //
  //  - Decoding off-the-air APRS messages
  //
  //  - Online calculator https://crccalc.com/
  //
  //    Use the CRC-16/X-25 algorithm.
  //
  //    NOTE: Spaces are not handled correctly so use Hex input type when
  //    calculating CRC of a string with spaces.
  //
  // - Online calculator
  //
  //     http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
  //
  //   using CRC16_X_25 parametrization preset.

  EXPECT_EQ(CalculateCRC<crc16ccitt::FCS>("123456789"), 0x906E);
  EXPECT_EQ(CalculateCRC<crc16ccitt::FCS>("Hello, World!"), 0x9BD5);
}

}  // namespace radio_core::crypto
