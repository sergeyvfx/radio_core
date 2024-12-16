// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/tool/log_util.h"

#include <sstream>

#include "radio_core/unittest/test.h"

namespace radio_core::tool {

TEST(LogUtil, LogTimeWithRealtimeComparison) {
  std::stringstream stream;
  stream << LogTimeWithRealtimeComparison(0.5f, 1.5f);

  EXPECT_EQ(stream.str(), "0.5 seconds (3x realtime)");
}

}  // namespace radio_core::tool
