// Copyright (c) 2025 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/signal/raised_cosine.h"

#include <array>

#include "radio_core/signal/filter_energy.h"
#include "radio_core/signal/filter_gain.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::signal {

using testing::FloatNear;
using testing::Pointwise;

TEST(raised_cosine, DesignRCFilter) {
  // gain=1.
  {
    std::array<float, 51> actual_kernel;
    DesignRCFilter<float>(actual_kernel, 1.0f, 2, 0.2f);
    EXPECT_NEAR(CalculateFilterGainAtDC<float>(actual_kernel), 1.0f, 1e-6f);

    // The values are obtained by carefully verifying the formula and running it
    // prior to any optimization.
    //
    // The test data has been generated using Matlab:
    //   rcosdesign(0.2,25,2,"normal")
    // and converted to have unity gain using
    // ScaleFilterToUnityGainAtFrequency().
    EXPECT_THAT(
        actual_kernel,
        Pointwise(
            FloatNear(1e-6f),
            std::to_array({
                -0.000000000000000000162396f, 0.000000000000000000273230f,
                0.000403438403736799955368f,  -0.000000000000000003123344f,
                -0.000866184942424297332764f, 0.000000000000000001299172f,
                0.001185305765829980373383f,  -0.000000000000000001318209f,
                -0.001042036106809973716736f, 0.000000000000000000651731f,
                -0.000000000000000000487189f, 0.000000000000000000880408f,
                0.002498214831575751304626f,  -0.000000000000000003312138f,
                -0.007165711838752031326294f, 0.000000000000000006495858f,
                0.015013872645795345306396f,  -0.000000000000000010106269f,
                -0.027837252244353294372559f, 0.000000000000000013686344f,
                0.049991574138402938842773f,  -0.000000000000000016727754f,
                -0.097430378198623657226562f, 0.000000000000000018768784f,
                0.315291315317153930664062f,  0.499915748834609985351562f,
                0.315291315317153930664062f,  0.000000000000000018768784f,
                -0.097430378198623657226562f, -0.000000000000000016727754f,
                0.049991574138402938842773f,  0.000000000000000013686344f,
                -0.027837252244353294372559f, -0.000000000000000010106269f,
                0.015013872645795345306396f,  0.000000000000000006495858f,
                -0.007165711838752031326294f, -0.000000000000000003312138f,
                0.002498214831575751304626f,  0.000000000000000000880408f,
                -0.000000000000000000487189f, 0.000000000000000000651731f,
                -0.001042036106809973716736f, -0.000000000000000001318209f,
                0.001185305765829980373383f,  0.000000000000000001299172f,
                -0.000866184942424297332764f, -0.000000000000000003123344f,
                0.000403438403736799955368f,  0.000000000000000000273230f,
                -0.000000000000000000162396f,
            })));
  }

  // gain=2.
  {
    std::array<float, 51> actual_kernel;
    DesignRCFilter<float>(actual_kernel, 2.0f, 2, 0.2f);
    EXPECT_NEAR(CalculateFilterGainAtDC<float>(actual_kernel), 2.0f, 1e-6f);
  }

  // beta=0.
  {
    std::array<float, 13> actual_kernel;
    DesignRCFilter<float>(actual_kernel, 1.0f, 3, 0.0f);
    EXPECT_NEAR(CalculateFilterGainAtDC<float>(actual_kernel), 1.0f, 1e-6f);
    ScaleFilterToUnityEnergy<float>(actual_kernel);

    // Verified against Matlab's rcosdesign:
    // >> rcosdesign(0, 4, 3, "normal")
    EXPECT_THAT(actual_kernel,
                Pointwise(FloatNear(1e-6f),
                          std::to_array<float>({0.0,
                                                -0.097973747338949,
                                                -0.122467184173686,
                                                0.0,
                                                0.244934368347373,
                                                0.489868736694746,
                                                0.592349068783433,
                                                0.489868736694746,
                                                0.244934368347373,
                                                0.0,
                                                -0.122467184173686,
                                                -0.097973747338949,
                                                0.0})));
  }

  // beta=1.
  {
    std::array<float, 13> actual_kernel;
    DesignRCFilter<float>(actual_kernel, 1.0f, 3, 1.0f);
    EXPECT_NEAR(CalculateFilterGainAtDC<float>(actual_kernel), 1.0f, 1e-6f);
    ScaleFilterToUnityEnergy<float>(actual_kernel);

    // Verified against Matlab's rcosdesign:
    // >> rcosdesign(1, 4, 3, "normal")
    EXPECT_THAT(actual_kernel,
                Pointwise(FloatNear(1e-6f),
                          std::to_array<float>({0.0,
                                                0.005452736062775,
                                                -0.011277249584375,
                                                0.0,
                                                0.177213922040185,
                                                0.496198981712519,
                                                0.666670664862098,
                                                0.496198981712519,
                                                0.177213922040185,
                                                0.0,
                                                -0.011277249584375,
                                                0.005452736062775,
                                                0.0})));
  }
}

}  // namespace radio_core::signal
