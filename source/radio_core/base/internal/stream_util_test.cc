// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/stream_util.h"

#include <ios>
#include <sstream>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(stream_util, StreamState) {
  {
    std::ostringstream stream;

    {
      const StreamState state(stream);
      stream << std::hex << 100;
    }

    { stream << 100; }

    EXPECT_EQ(stream.str(), "64100");
  }
}

}  // namespace radio_core
