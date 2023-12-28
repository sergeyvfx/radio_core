// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/crypto/md5.h"

#include <array>
#include <cstdint>

#include "radio_core/unittest/test.h"

namespace radio_core::crypto {

TEST(MD5, Basic) {
  // echo -n 'Hello, World!' | md5
  const md5::Digest expected_digest("65a8e27d8879283831b664bd8b7f0ad4");

  md5::Hasher hasher;
  hasher.Append(std::to_array<uint8_t>(
      {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'}));
  hasher.Finish();

  EXPECT_EQ(hasher.GetDigest(), expected_digest);
}

}  // namespace radio_core::crypto
