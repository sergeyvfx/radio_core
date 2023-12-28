// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/frequency.h"

#include <array>
#include <cmath>
#include <cstdint>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core {

// Matcher for the frequencies equality.
//
// It used rather small floating point epsilon because of the mathematical
// operations for units loosing quite some bits of accuracy. It is still very
// low frequency deviation, perhaps not worse than real-life radio frequency
// accuracy.
MATCHER_P(FrequencyNear, expected, "") {
  const Frequency delta_frequency = arg - expected;
  const double delta = std::abs(delta_frequency.ToFloatingPoint<double>());
  if (delta < 1e-6) {
    return true;
  }
  *result_listener << "whose absolute difference is " << delta;
  return true;
}

TEST(Frequency, Construct) {
  EXPECT_FLOAT_EQ(Frequency(10).ToFloatingPoint<double>(), 10.0);
  EXPECT_FLOAT_EQ(Frequency(10.1).ToFloatingPoint<double>(), 10.1);
  EXPECT_FLOAT_EQ(Frequency(10.1f).ToFloatingPoint<double>(), 10.1);

  EXPECT_EQ(Frequency(-100).ToInteger<int>(), -100);
}

TEST(Frequency, Assign) {
  {
    Frequency bar{456};

    {
      const Frequency foo{123};
      bar = foo;
    }

    EXPECT_EQ(bar, 123);
  }

  {
    Frequency bar{456};
    bar = 123;

    EXPECT_EQ(bar, 123);
  }
}

TEST(Frequency, Compare) {
  // Explicit.
  {
    EXPECT_TRUE(Frequency(10) == Frequency(10));
    EXPECT_FALSE(Frequency(10) != Frequency(10));
    EXPECT_TRUE(Frequency(10) < Frequency(20));
    EXPECT_FALSE(Frequency(10) > Frequency(20));
  }

  // Implicit.
  {
    EXPECT_TRUE(Frequency(10) == 10);
    EXPECT_FALSE(Frequency(10) != 10);
    EXPECT_TRUE(Frequency(10) < 20);
    EXPECT_FALSE(Frequency(10) > 20);

    EXPECT_TRUE(Frequency(10) == 10.0);
    EXPECT_FALSE(Frequency(10) != 10.0);
    EXPECT_TRUE(Frequency(10) < 20.0);
    EXPECT_FALSE(Frequency(10) > 20.0);
  }
}

TEST(Frequency, Conversion) {
  const Frequency frequency(123.4f);

  EXPECT_FLOAT_EQ(double(frequency), 123.4);
  EXPECT_FLOAT_EQ(float(frequency), 123.4f);

  EXPECT_FLOAT_EQ(int(frequency), 123);
}

TEST(Frequency, ToInteger) {
  // 1000000 GHz
  EXPECT_EQ(Frequency(1000000000000001LL).ToInteger<int64_t>(),
            1000000000000001LL);
}

TEST(Frequency, ToFloatingPoint) {
  // 1000000 GHz
  EXPECT_EQ(Frequency(1000000000000001LL).ToFloatingPoint<double>(),
            1000000000000001.0);
}

TEST(Frequency, Negation) {
  EXPECT_EQ((-Frequency(100)).ToInteger<int>(), -100);
}

TEST(Frequency, Math) {
  EXPECT_EQ(Frequency(123) + 2, Frequency(125));
  EXPECT_EQ(Frequency(123) - 2, Frequency(121));

  EXPECT_EQ(Frequency(123) * 2, Frequency(246));
  EXPECT_EQ(Frequency(123) / 2, Frequency(61.5));
}

TEST(Frequency, Round) {
  EXPECT_EQ(Round(Frequency(123.3)), Frequency(123));
  EXPECT_EQ(Round(Frequency(123.6)), Frequency(124));
}

TEST(Frequency, Modulo) {
  // Test case os based on https://stackoverflow.com/a/67098028
  const auto kArguments = std::to_array<double>({-10.9,
                                                 -10.5,
                                                 -10.4,
                                                 -0.9,
                                                 -0.5,
                                                 -0.1,
                                                 0.0,
                                                 0.1,
                                                 0.5,
                                                 0.9,
                                                 10.4,
                                                 10.5,
                                                 10.9});

  for (const double a : kArguments) {
    EXPECT_NEAR(double(Modulo(Frequency(a), 3.2_Hz)), std::fmod(a, 3.2), 1e-12);
    EXPECT_NEAR(
        double(Modulo(Frequency(a), -3.2_Hz)), std::fmod(a, -3.2), 1e-12);
  }
}

TEST(Frequency, Unit) {
  // kHz
  {
    EXPECT_THAT(0_kHz, FrequencyNear(Frequency(0LL)));
    EXPECT_THAT(1_kHz, FrequencyNear(Frequency(1000LL)));
    EXPECT_THAT(10_kHz, FrequencyNear(Frequency(10000LL)));
    EXPECT_THAT(100_kHz, FrequencyNear(Frequency(100000LL)));
    EXPECT_THAT(1000_kHz, FrequencyNear(Frequency(1000000LL)));
    EXPECT_THAT(10000_kHz, FrequencyNear(Frequency(10000000LL)));
    EXPECT_THAT(100000_kHz, FrequencyNear(Frequency(100000000LL)));
    EXPECT_THAT(1000000_kHz, FrequencyNear(Frequency(1000000000LL)));

    EXPECT_THAT(1.0_kHz, FrequencyNear(Frequency(1000LL)));
    EXPECT_THAT(10.0_kHz, FrequencyNear(Frequency(10000LL)));
    EXPECT_THAT(100.0_kHz, FrequencyNear(Frequency(100000LL)));
    EXPECT_THAT(1000.0_kHz, FrequencyNear(Frequency(1000000LL)));
    EXPECT_THAT(10000.0_kHz, FrequencyNear(Frequency(10000000LL)));
    EXPECT_THAT(100000.0_kHz, FrequencyNear(Frequency(100000000LL)));
    EXPECT_THAT(1000000.0_kHz, FrequencyNear(Frequency(1000000000LL)));

    EXPECT_THAT(0.001_kHz, FrequencyNear(Frequency(1LL)));
    EXPECT_THAT(1.001_kHz, FrequencyNear(Frequency(1001LL)));
    EXPECT_THAT(10.001_kHz, FrequencyNear(Frequency(10001LL)));
    EXPECT_THAT(100.001_kHz, FrequencyNear(Frequency(100001LL)));
    EXPECT_THAT(1000.001_kHz, FrequencyNear(Frequency(1000001LL)));
    EXPECT_THAT(10000.001_kHz, FrequencyNear(Frequency(10000001LL)));
    EXPECT_THAT(100000.001_kHz, FrequencyNear(Frequency(100000001LL)));
    EXPECT_THAT(1000000.001_kHz, FrequencyNear(Frequency(1000000001LL)));
  }

  // MHz.
  {
    EXPECT_THAT(0_MHz, FrequencyNear(Frequency(0LL)));
    EXPECT_THAT(1_MHz, FrequencyNear(Frequency(1000000LL)));
    EXPECT_THAT(10_MHz, FrequencyNear(Frequency(10000000LL)));
    EXPECT_THAT(100_MHz, FrequencyNear(Frequency(100000000LL)));
    EXPECT_THAT(1000_MHz, FrequencyNear(Frequency(1000000000LL)));
    EXPECT_THAT(10000_MHz, FrequencyNear(Frequency(10000000000LL)));
    EXPECT_THAT(100000_MHz, FrequencyNear(Frequency(100000000000LL)));
    EXPECT_THAT(1000000_MHz, FrequencyNear(Frequency(1000000000000LL)));

    EXPECT_THAT(1.0_MHz, FrequencyNear(Frequency(1000000LL)));
    EXPECT_THAT(10.0_MHz, FrequencyNear(Frequency(10000000LL)));
    EXPECT_THAT(100.0_MHz, FrequencyNear(Frequency(100000000LL)));
    EXPECT_THAT(1000.0_MHz, FrequencyNear(Frequency(1000000000LL)));
    EXPECT_THAT(10000.0_MHz, FrequencyNear(Frequency(10000000000LL)));
    EXPECT_THAT(100000.0_MHz, FrequencyNear(Frequency(100000000000LL)));
    EXPECT_THAT(1000000.0_MHz, FrequencyNear(Frequency(1000000000000LL)));

    EXPECT_THAT(0.000001_MHz, FrequencyNear(Frequency(1LL)));
    EXPECT_THAT(1.000001_MHz, FrequencyNear(Frequency(1000001LL)));
    EXPECT_THAT(10.000001_MHz, FrequencyNear(Frequency(10000001LL)));
    EXPECT_THAT(100.000001_MHz, FrequencyNear(Frequency(100000001LL)));
    EXPECT_THAT(1000.000001_MHz, FrequencyNear(Frequency(1000000001LL)));
    EXPECT_THAT(10000.000001_MHz, FrequencyNear(Frequency(10000000001LL)));
    EXPECT_THAT(100000.000001_MHz, FrequencyNear(Frequency(100000000001LL)));
    EXPECT_THAT(1000000.000001_MHz, FrequencyNear(Frequency(1000000000001LL)));
  }

  // GHz.
  {
    EXPECT_THAT(0_GHz, FrequencyNear(Frequency(0LL)));
    EXPECT_THAT(1_GHz, FrequencyNear(Frequency(1000000000LL)));
    EXPECT_THAT(10_GHz, FrequencyNear(Frequency(10000000000LL)));
    EXPECT_THAT(100_GHz, FrequencyNear(Frequency(100000000000LL)));
    EXPECT_THAT(1000_GHz, FrequencyNear(Frequency(1000000000000LL)));
    EXPECT_THAT(10000_GHz, FrequencyNear(Frequency(10000000000000LL)));
    EXPECT_THAT(100000_GHz, FrequencyNear(Frequency(100000000000000LL)));
    EXPECT_THAT(1000000_GHz, FrequencyNear(Frequency(1000000000000000LL)));

    EXPECT_THAT(1.0_GHz, FrequencyNear(Frequency(1000000000LL)));
    EXPECT_THAT(10.0_GHz, FrequencyNear(Frequency(10000000000LL)));
    EXPECT_THAT(100.0_GHz, FrequencyNear(Frequency(100000000000LL)));
    EXPECT_THAT(1000.0_GHz, FrequencyNear(Frequency(1000000000000LL)));
    EXPECT_THAT(10000.0_GHz, FrequencyNear(Frequency(10000000000000LL)));
    EXPECT_THAT(100000.0_GHz, FrequencyNear(Frequency(100000000000000LL)));
    EXPECT_THAT(1000000.0_GHz, FrequencyNear(Frequency(1000000000000000LL)));

    EXPECT_THAT(0.000000001_GHz, FrequencyNear(Frequency(1LL)));
    EXPECT_THAT(1.000000001_GHz, FrequencyNear(Frequency(1000000001LL)));
    EXPECT_THAT(10.000000001_GHz, FrequencyNear(Frequency(10000000001LL)));
    EXPECT_THAT(100.000000001_GHz, FrequencyNear(Frequency(100000000001LL)));
    EXPECT_THAT(1000.000000001_GHz, FrequencyNear(Frequency(1000000000001LL)));
    EXPECT_THAT(10000.000000001_GHz,
                FrequencyNear(Frequency(10000000000001LL)));
    EXPECT_THAT(100000.000000001_GHz,
                FrequencyNear(Frequency(100000000000001LL)));
    EXPECT_THAT(1000000.000000001_GHz,
                FrequencyNear(Frequency(1000000000000001LL)));
  }
}

}  // namespace radio_core
