// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/picture/sstv/result.h"

#include <sstream>

#include "radio_core/base/variant.h"
#include "radio_core/unittest/test.h"

namespace radio_core::picture::sstv {

TEST(sstv, DecodeResult) {
  DecodeResult result{DecodedData({
      DecodedVISCode{0x5f},
  })};

  EXPECT_TRUE(result.Ok());

  std::stringstream stream;

  for (const DecodedVariant& variant : result.GetValue()) {
    variant | match{
                  [&](const DecodedVISCode& decoded) {
                    stream << "VIS " << int(decoded.vis_code) << "\n";
                  },
                  [](auto& /*decoded*/) {},
              };
  }

  EXPECT_EQ(stream.str(), "VIS 95\n");
}

}  // namespace radio_core::picture::sstv
