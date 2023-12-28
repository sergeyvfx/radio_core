// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/math/bessel.h"

#include "radio_core/unittest/test.h"

#include <cmath>

namespace radio_core {

TEST(Bessel, ModifiedBesselI0) {
  EXPECT_NEAR(ModifiedBesselI0(1.2345), 1.418860751277091, 1e-14);

  EXPECT_NEAR(ModifiedBesselI0(-3.1), 5.294491489675604, 1e-9);
  EXPECT_NEAR(ModifiedBesselI0(-3.0), 4.880792585865025, 1e-9);
  EXPECT_NEAR(ModifiedBesselI0(0.0), 1.0, 1e-9);
  EXPECT_NEAR(ModifiedBesselI0(3.0), 4.880792585865025, 1e-9);
  EXPECT_NEAR(ModifiedBesselI0(3.1), 5.294491489675604, 1e-9);
}

}  // namespace radio_core
