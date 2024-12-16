// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/frequency.h"

#include "radio_core/base/constants.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

TEST(NormalizedAngularFrequency, Basic) {
  EXPECT_NEAR(NormalizedAngularFrequency<float>(1000, 44100),
              0.14247585730565954f,
              1e-6f);
}

TEST(RationalFrequency, Basic) {
  EXPECT_NEAR(
      RationalFrequency<float>(0.14247585730565954f, 44100), 1000.0f, 1e-6f);
}

TEST(UnwrapInstantFrequency, Basic) {
  const double pi = constants::pi;

  EXPECT_NEAR(UnwrapInstantFrequency(0.), 0, 1e-6);
  EXPECT_NEAR(UnwrapInstantFrequency(pi), pi, 1e-6);
  EXPECT_NEAR(UnwrapInstantFrequency(2 * pi), 0, 1e-6);
  EXPECT_NEAR(UnwrapInstantFrequency(pi * 1.5), -pi / 2, 1e-6);
  EXPECT_NEAR(UnwrapInstantFrequency(-pi), -pi, 1e-6);
  EXPECT_NEAR(UnwrapInstantFrequency(-2 * pi), 0, 1e-6);
  EXPECT_NEAR(UnwrapInstantFrequency(-pi * 1.5), pi / 2, 1e-6);
}

}  // namespace radio_core::signal
