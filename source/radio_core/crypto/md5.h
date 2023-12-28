// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Calculator of MD5 hash.
//
// Example:
//
//   crypto::md5::Hasher hasher;
//   hasher.append("Hello, World!");
//   hasher.finish();
//
//   std::cout << hasher.getDigest() << std::endl;

#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <span>
#include <string_view>
#include <utility>

#include "radio_core/base/algorithm.h"
#include "radio_core/base/build_config.h"
#include "radio_core/base/stream_util.h"

namespace radio_core::crypto::md5 {

namespace md5_internal {

//  Copyright (C) 1999, 2002 Aladdin Enterprises.  All rights reserved.
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
//  L. Peter Deutsch
//  ghost@aladdin.com

//  Independent implementation of MD5 (RFC 1321).
//
//  This code implements the MD5 Algorithm defined in RFC 1321, whose
//  text is available at
//        http://www.ietf.org/rfc/rfc1321.txt
//  The code is derived from the text of the RFC, including the test suite
//  (section A.5) but excluding the rest of Appendix A.  It does not include
//  any code or documentation that is identified in the RFC as being
//  copyrighted.
//
//  The original and principal author of md5.h is L. Peter Deutsch
//  <ghost@aladdin.com>.  Other authors are noted in the change history
//  that follows (in reverse chronological order):
//
//  2002-04-13 lpd Removed support for non-ANSI compilers; removed
//        references to Ghostscript; clarified derivation from RFC 1321;
//        now handles byte order either statically or dynamically.
//  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
//  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5);
//        added conditionalization for C++ compilation from Martin
//        Purschke <purschke@bnl.gov>.
//  1999-05-03 lpd Original version.

// Modifications:
// - Code style (following the project's clang-format rules).
// - Convert to more C++ style.
// - Inline header-only implementation.
// - Compile-time endian detection.
// - Resolve a lot of Clang-Tidy warnings.

/// Define the state of the MD5 Algorithm.
struct MD5State {
  uint32_t count[2];  // Message length in bits, lsw first.
  uint32_t abcd[4];   // Digest buffer.
  uint8_t buf[64];    // Accumulate block.
};

// Initialize the algorithm.
inline void Init(MD5State* pms);

// Append a string to the message.
inline void Append(MD5State* pms, const uint8_t* data, int nbytes);

// Finish the message and return the digest.
inline void Finish(MD5State* pms, uint8_t digest[16]);

static constexpr uint32_t kTMask = ((uint32_t)~0);
static constexpr uint32_t kT1 = /* 0xd76aa478 */ (kTMask ^ 0x28955b87);
static constexpr uint32_t kT2 = /* 0xe8c7b756 */ (kTMask ^ 0x173848a9);
static constexpr uint32_t kT3 = 0x242070db;
static constexpr uint32_t kT4 = /* 0xc1bdceee */ (kTMask ^ 0x3e423111);
static constexpr uint32_t kT5 = /* 0xf57c0faf */ (kTMask ^ 0x0a83f050);
static constexpr uint32_t kT6 = 0x4787c62a;
static constexpr uint32_t kT7 = /* 0xa8304613 */ (kTMask ^ 0x57cfb9ec);
static constexpr uint32_t kT8 = /* 0xfd469501 */ (kTMask ^ 0x02b96afe);
static constexpr uint32_t kT9 = 0x698098d8;
static constexpr uint32_t kT10 = /* 0x8b44f7af */ (kTMask ^ 0x74bb0850);
static constexpr uint32_t kT11 = /* 0xffff5bb1 */ (kTMask ^ 0x0000a44e);
static constexpr uint32_t kT12 = /* 0x895cd7be */ (kTMask ^ 0x76a32841);
static constexpr uint32_t kT13 = 0x6b901122;
static constexpr uint32_t kT14 = /* 0xfd987193 */ (kTMask ^ 0x02678e6c);
static constexpr uint32_t kT15 = /* 0xa679438e */ (kTMask ^ 0x5986bc71);
static constexpr uint32_t kT16 = 0x49b40821;
static constexpr uint32_t kT17 = /* 0xf61e2562 */ (kTMask ^ 0x09e1da9d);
static constexpr uint32_t kT18 = /* 0xc040b340 */ (kTMask ^ 0x3fbf4cbf);
static constexpr uint32_t kT19 = 0x265e5a51;
static constexpr uint32_t kT20 = /* 0xe9b6c7aa */ (kTMask ^ 0x16493855);
static constexpr uint32_t kT21 = /* 0xd62f105d */ (kTMask ^ 0x29d0efa2);
static constexpr uint32_t kT22 = 0x02441453;
static constexpr uint32_t kT23 = /* 0xd8a1e681 */ (kTMask ^ 0x275e197e);
static constexpr uint32_t kT24 = /* 0xe7d3fbc8 */ (kTMask ^ 0x182c0437);
static constexpr uint32_t kT25 = 0x21e1cde6;
static constexpr uint32_t kT26 = /* 0xc33707d6 */ (kTMask ^ 0x3cc8f829);
static constexpr uint32_t kT27 = /* 0xf4d50d87 */ (kTMask ^ 0x0b2af278);
static constexpr uint32_t kT28 = 0x455a14ed;
static constexpr uint32_t kT29 = /* 0xa9e3e905 */ (kTMask ^ 0x561c16fa);
static constexpr uint32_t kT30 = /* 0xfcefa3f8 */ (kTMask ^ 0x03105c07);
static constexpr uint32_t kT31 = 0x676f02d9;
static constexpr uint32_t kT32 = /* 0x8d2a4c8a */ (kTMask ^ 0x72d5b375);
static constexpr uint32_t kT33 = /* 0xfffa3942 */ (kTMask ^ 0x0005c6bd);
static constexpr uint32_t kT34 = /* 0x8771f681 */ (kTMask ^ 0x788e097e);
static constexpr uint32_t kT35 = 0x6d9d6122;
static constexpr uint32_t kT36 = /* 0xfde5380c */ (kTMask ^ 0x021ac7f3);
static constexpr uint32_t kT37 = /* 0xa4beea44 */ (kTMask ^ 0x5b4115bb);
static constexpr uint32_t kT38 = 0x4bdecfa9;
static constexpr uint32_t kT39 = /* 0xf6bb4b60 */ (kTMask ^ 0x0944b49f);
static constexpr uint32_t kT40 = /* 0xbebfbc70 */ (kTMask ^ 0x4140438f);
static constexpr uint32_t kT41 = 0x289b7ec6;
static constexpr uint32_t kT42 = /* 0xeaa127fa */ (kTMask ^ 0x155ed805);
static constexpr uint32_t kT43 = /* 0xd4ef3085 */ (kTMask ^ 0x2b10cf7a);
static constexpr uint32_t kT44 = 0x04881d05;
static constexpr uint32_t kT45 = /* 0xd9d4d039 */ (kTMask ^ 0x262b2fc6);
static constexpr uint32_t kT46 = /* 0xe6db99e5 */ (kTMask ^ 0x1924661a);
static constexpr uint32_t kT47 = 0x1fa27cf8;
static constexpr uint32_t kT48 = /* 0xc4ac5665 */ (kTMask ^ 0x3b53a99a);
static constexpr uint32_t kT49 = /* 0xf4292244 */ (kTMask ^ 0x0bd6ddbb);
static constexpr uint32_t kT50 = 0x432aff97;
static constexpr uint32_t kT51 = /* 0xab9423a7 */ (kTMask ^ 0x546bdc58);
static constexpr uint32_t kT52 = /* 0xfc93a039 */ (kTMask ^ 0x036c5fc6);
static constexpr uint32_t kT53 = 0x655b59c3;
static constexpr uint32_t kT54 = /* 0x8f0ccc92 */ (kTMask ^ 0x70f3336d);
static constexpr uint32_t kT55 = /* 0xffeff47d */ (kTMask ^ 0x00100b82);
static constexpr uint32_t kT56 = /* 0x85845dd1 */ (kTMask ^ 0x7a7ba22e);
static constexpr uint32_t kT57 = 0x6fa87e4f;
static constexpr uint32_t kT58 = /* 0xfe2ce6e0 */ (kTMask ^ 0x01d3191f);
static constexpr uint32_t kT59 = /* 0xa3014314 */ (kTMask ^ 0x5cfebceb);
static constexpr uint32_t kT60 = 0x4e0811a1;
static constexpr uint32_t kT61 = /* 0xf7537e82 */ (kTMask ^ 0x08ac817d);
static constexpr uint32_t kT62 = /* 0xbd3af235 */ (kTMask ^ 0x42c50dca);
static constexpr uint32_t kT63 = 0x2ad7d2bb;
static constexpr uint32_t kT64 = /* 0xeb86d391 */ (kTMask ^ 0x14792c6e);

inline void Process(MD5State& pms, const uint8_t* data /*[64]*/) {
  uint32_t a = pms.abcd[0];
  uint32_t b = pms.abcd[1];
  uint32_t c = pms.abcd[2];
  uint32_t d = pms.abcd[3];
  uint32_t t;

#if ARCH_CPU_BIG_ENDIAN
  uint32_t X[16];
#else
  uint32_t xbuf[16];
  const uint32_t* X;
#endif

  {
#if ARCH_CPU_LITTLE_ENDIAN
    // On little-endian machines, we can process properly aligned
    // data without copying it.
    if ((reinterpret_cast<uintptr_t>(data) & 3) == 0) {
      // Data are properly aligned.
      X = (const uint32_t*)data;
    } else {
      // Not aligned.
      memcpy(xbuf, data, 64);
      X = xbuf;
    }
#else
    // On big-endian machines, we must arrange the bytes in the
    // right order.
    const uint8_t* xp = data;
    int i;

    for (i = 0; i < 16; ++i, xp += 4) {
      X[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
    }
#endif
  }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

  /// Round 1.
  // Let [abcd k s i] denote the operation
  //   a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s).
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)                                              \
  (t) = (a) + F((b), (c), (d)) + (X)[k] + (Ti);                                \
  (a) = ROTATE_LEFT((t), (s)) + (b)
  // Do the following 16 operations.
  SET(a, b, c, d, 0, 7, kT1);
  SET(d, a, b, c, 1, 12, kT2);
  SET(c, d, a, b, 2, 17, kT3);
  SET(b, c, d, a, 3, 22, kT4);
  SET(a, b, c, d, 4, 7, kT5);
  SET(d, a, b, c, 5, 12, kT6);
  SET(c, d, a, b, 6, 17, kT7);
  SET(b, c, d, a, 7, 22, kT8);
  SET(a, b, c, d, 8, 7, kT9);
  SET(d, a, b, c, 9, 12, kT10);
  SET(c, d, a, b, 10, 17, kT11);
  SET(b, c, d, a, 11, 22, kT12);
  SET(a, b, c, d, 12, 7, kT13);
  SET(d, a, b, c, 13, 12, kT14);
  SET(c, d, a, b, 14, 17, kT15);
  SET(b, c, d, a, 15, 22, kT16);
#undef SET

  // Round 2.
  // Let [abcd k s i] denote the operation
  //   a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s).
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)                                              \
  (t) = (a) + G((b), (c), (d)) + (X)[k] + (Ti);                                \
  (a) = ROTATE_LEFT((t), (s)) + (b)
  // Do the following 16 operations.
  SET(a, b, c, d, 1, 5, kT17);
  SET(d, a, b, c, 6, 9, kT18);
  SET(c, d, a, b, 11, 14, kT19);
  SET(b, c, d, a, 0, 20, kT20);
  SET(a, b, c, d, 5, 5, kT21);
  SET(d, a, b, c, 10, 9, kT22);
  SET(c, d, a, b, 15, 14, kT23);
  SET(b, c, d, a, 4, 20, kT24);
  SET(a, b, c, d, 9, 5, kT25);
  SET(d, a, b, c, 14, 9, kT26);
  SET(c, d, a, b, 3, 14, kT27);
  SET(b, c, d, a, 8, 20, kT28);
  SET(a, b, c, d, 13, 5, kT29);
  SET(d, a, b, c, 2, 9, kT30);
  SET(c, d, a, b, 7, 14, kT31);
  SET(b, c, d, a, 12, 20, kT32);
#undef SET

  // Round 3.
  // Let [abcd k s t] denote the operation
  //   a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s).
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)                                              \
  (t) = (a) + H((b), (c), (d)) + (X)[k] + (Ti);                                \
  (a) = ROTATE_LEFT((t), (s)) + (b)
  // Do the following 16 operations.
  SET(a, b, c, d, 5, 4, kT33);
  SET(d, a, b, c, 8, 11, kT34);
  SET(c, d, a, b, 11, 16, kT35);
  SET(b, c, d, a, 14, 23, kT36);
  SET(a, b, c, d, 1, 4, kT37);
  SET(d, a, b, c, 4, 11, kT38);
  SET(c, d, a, b, 7, 16, kT39);
  SET(b, c, d, a, 10, 23, kT40);
  SET(a, b, c, d, 13, 4, kT41);
  SET(d, a, b, c, 0, 11, kT42);
  SET(c, d, a, b, 3, 16, kT43);
  SET(b, c, d, a, 6, 23, kT44);
  SET(a, b, c, d, 9, 4, kT45);
  SET(d, a, b, c, 12, 11, kT46);
  SET(c, d, a, b, 15, 16, kT47);
  SET(b, c, d, a, 2, 23, kT48);
#undef SET

  // Round 4.
  // Let [abcd k s t] denote the operation
  //   a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s).
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)                                              \
  t = (a) + I((b), (c), (d)) + (X)[k] + (Ti);                                  \
  (a) = ROTATE_LEFT((t), (s)) + (b)
  // Do the following 16 operations.
  SET(a, b, c, d, 0, 6, kT49);
  SET(d, a, b, c, 7, 10, kT50);
  SET(c, d, a, b, 14, 15, kT51);
  SET(b, c, d, a, 5, 21, kT52);
  SET(a, b, c, d, 12, 6, kT53);
  SET(d, a, b, c, 3, 10, kT54);
  SET(c, d, a, b, 10, 15, kT55);
  SET(b, c, d, a, 1, 21, kT56);
  SET(a, b, c, d, 8, 6, kT57);
  SET(d, a, b, c, 15, 10, kT58);
  SET(c, d, a, b, 6, 15, kT59);
  SET(b, c, d, a, 13, 21, kT60);
  SET(a, b, c, d, 4, 6, kT61);
  SET(d, a, b, c, 11, 10, kT62);
  SET(c, d, a, b, 2, 15, kT63);
  SET(b, c, d, a, 9, 21, kT64);
#undef SET

#undef ROTATE_LEFT

  // Then perform the following additions. (That is increment each
  // of the four registers by the value it had before this block
  // was started.)
  pms.abcd[0] += a;
  pms.abcd[1] += b;
  pms.abcd[2] += c;
  pms.abcd[3] += d;
}

inline void Init(MD5State& pms) {
  pms.count[0] = pms.count[1] = 0;
  pms.abcd[0] = 0x67452301;
  pms.abcd[1] = /*0xefcdab89*/ kTMask ^ 0x10325476;
  pms.abcd[2] = /*0x98badcfe*/ kTMask ^ 0x67452301;
  pms.abcd[3] = 0x10325476;
}

inline void Append(MD5State& pms, const uint8_t* data, const int nbytes) {
  const uint8_t* p = data;
  int left = nbytes;
  const auto offset = int((pms.count[0] >> 3) & 63);
  const auto nbits = uint32_t(nbytes << 3);

  if (nbytes <= 0) {
    return;
  }

  // Update the message length.
  pms.count[1] += nbytes >> 29;
  pms.count[0] += nbits;
  if (pms.count[0] < nbits) {
    pms.count[1]++;
  }

  // Process an initial partial block.
  if (offset) {
    int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

    memcpy(pms.buf + offset, p, copy);
    if (offset + copy < 64) {
      return;
    }
    p += copy;
    left -= copy;
    Process(pms, pms.buf);
  }

  // Process full blocks.
  for (; left >= 64; p += 64, left -= 64) {
    Process(pms, p);
  }

  // Process a final partial block.
  if (left) {
    memcpy(pms.buf, p, left);
  }
}

inline void Finish(MD5State& pms, uint8_t digest[16]) {
  static const uint8_t kPad[64] = {
      0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint8_t data[8];
  int i;

  // Save the length before padding.
  for (i = 0; i < 8; ++i) {
    data[i] = uint8_t(pms.count[i >> 2] >> ((i & 3) << 3));
  }

  // Pad to 56 bytes mod 64.
  Append(pms, kPad, int((55 - (pms.count[0] >> 3)) & 63) + 1);

  // Append the length.
  Append(pms, data, 8);

  for (i = 0; i < 16; ++i) {
    digest[i] = uint8_t(pms.abcd[i >> 2] >> ((i & 3) << 3));
  }
}

// Convert character of a number of a hex value in stirng repsernation to
// integer.
// Invalid characters are considered to be 0.
inline auto HexDigitToInt(const char ch) -> int {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }

  if (ch >= 'a' && ch <= 'f') {
    return 10 + ch - 'a';
  }
  if (ch >= 'A' && ch <= 'F') {
    return 10 + ch - 'A';
  }

  return 0;
}

}  // namespace md5_internal

class Digest {
 public:
  Digest() = default;
  Digest(const Digest& other) = default;
  Digest(Digest&& other) noexcept = default;

  // Construct digest from a sequence of bytes.
  // The order is the same as during calculation, and matches byte presentation
  // of hash returned by tools like `md5sum`.
  explicit Digest(std::array<uint8_t, 16> bytes) : data_(std::move(bytes)) {}

  // Construct digest from its hex string representation.
  // Up to 16 characters of the stirng is used. If the string terminates too
  // soon the remaining bits are considered to be 0.
  // If the string contains invalid for base-16 system characters they are
  // treated as 0.
  explicit Digest(std::string_view str) {
    for (int i = 0; i < 16 && !str.empty(); ++i) {
      int byte = 0;

      if (!str.empty()) {
        byte = (byte << 4) | md5_internal::HexDigitToInt(str[0]);
        str.remove_prefix(1);
      }

      if (!str.empty()) {
        byte = (byte << 4) | md5_internal::HexDigitToInt(str[0]);
        str.remove_prefix(1);
      }

      data_[i] = byte;
    }
  }

  ~Digest() = default;

  inline auto Data() -> uint8_t* { return data_.data(); }
  inline auto Data() const -> const uint8_t* { return data_.data(); }

  inline auto Size() const -> size_t { return data_.size(); }

  auto operator=(const Digest& other) -> Digest& = default;
  auto operator=(Digest&& other) -> Digest& = default;

  inline auto operator==(const Digest& other) const -> bool {
    return data_ == other.data_;
  }
  inline auto operator!=(const Digest& other) const -> bool {
    return !(*this == other);
  }

  inline auto operator[](size_t index) -> uint8_t& { return data_[index]; }
  inline auto operator[](size_t index) const -> uint8_t { return data_[index]; }

 private:
  std::array<uint8_t, 16> data_ = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
};

inline auto operator<<(std::ostream& os, const Digest digest) -> std::ostream& {
  const StreamState stream_state(os);

  for (size_t i = 0; i < digest.Size(); ++i) {
    const unsigned int byte = digest[i];
    os << std::hex << std::setfill('0') << std::setw(2) << byte;
  }

  return os;
}

class Hasher {
 public:
  inline Hasher() { md5_internal::Init(state_); }

  // Append a sequence of bytes to the hash. The bytes are added in their
  // order in the span.
  inline void Append(const std::span<const uint8_t> buffer) {
    // NOTE: The append is internally split into buffers of size which fits into
    // integer data type. This is because the buffer might be bigger, but the
    // underlying implementation is based on integer type.

    constexpr size_t kMaxBytesPerAppend = std::numeric_limits<int>::max();

    std::span<const uint8_t> remaining_buffer = buffer;
    while (!remaining_buffer.empty()) {
      const size_t num_bytes = Min(kMaxBytesPerAppend, remaining_buffer.size());
      md5_internal::Append(state_, remaining_buffer.data(), int(num_bytes));
      remaining_buffer = remaining_buffer.subspan(num_bytes);
    }
  }

  inline void Finish() { md5_internal::Finish(state_, digest_.Data()); }

  inline auto GetDigest() const -> const Digest& { return digest_; }

 private:
  md5_internal::MD5State state_;
  Digest digest_;
};

}  // namespace radio_core::crypto::md5
