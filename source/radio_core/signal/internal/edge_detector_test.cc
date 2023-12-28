// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/edge_detector.h"

#include "radio_core/unittest/test.h"

namespace radio_core::signal {

#define EXPECT_NO_EDGE(expr)                                                   \
  do {                                                                         \
    const auto edge = expr;                                                    \
    EXPECT_FALSE(edge) << #expr << " at " << __FILE__ << ":" << __LINE__;      \
  } while (false)

#define EXPECT_RISING_EDGE(expr)                                               \
  do {                                                                         \
    const auto edge = expr;                                                    \
    EXPECT_TRUE(edge.rising)                                                   \
        << #expr << " at " << __FILE__ << ":" << __LINE__;                     \
    EXPECT_FALSE(edge.falling)                                                 \
        << #expr << " at " << __FILE__ << ":" << __LINE__;                     \
  } while (false)

#define EXPECT_FALLING_EDGE(expr)                                              \
  do {                                                                         \
    const auto edge = expr;                                                    \
    EXPECT_FALSE(edge.rising)                                                  \
        << #expr << " at " << __FILE__ << ":" << __LINE__;                     \
    EXPECT_TRUE(edge.falling)                                                  \
        << #expr << " at " << __FILE__ << ":" << __LINE__;                     \
  } while (false)

TEST(EdgeDetector, RisingAndFalling) {
  EdgeDetector<float>::Options options;
  options.sample_weight = 0.8f;
  options.slow_sample_weight = 0.08f;
  options.rising_edge_threshold = 0.1f;
  options.falling_edge_threshold = 0.1f;

  EdgeDetector<float> detector(options);

  EXPECT_NO_EDGE(detector(1e-6f));
  EXPECT_NO_EDGE(detector(-1e-6f));
  EXPECT_RISING_EDGE(detector(0.99f));
  EXPECT_NO_EDGE(detector(1.0f));
  EXPECT_FALLING_EDGE(detector(-1.0f));
}

}  // namespace radio_core::signal
