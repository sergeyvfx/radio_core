// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/format.h"

#include <array>
#include <string>

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(IntFormat, OverflowAlignToLeft) {
  const IntFormat::Options options;

  {
    std::array<char, 0> buffer;
    EXPECT_FALSE(IntFormat::FormatToBuffer(options, 123, buffer));
  }

  {
    std::array<char, 1> buffer;
    EXPECT_FALSE(IntFormat::FormatToBuffer(options, 123, buffer));
  }

  {
    std::array<char, 2> buffer;
    EXPECT_FALSE(IntFormat::FormatToBuffer(options, 123, buffer));
  }

  {
    std::array<char, 3> buffer;

    EXPECT_FALSE(IntFormat::FormatToBuffer(options, 123, buffer));
  }

  {
    std::array<char, 4> buffer;

    EXPECT_TRUE(IntFormat::FormatToBuffer(options, 123, buffer));
    EXPECT_EQ(std::string_view(buffer.data()), "123");
  }
}

TEST(IntFormat, OverflowAlignToRight) {
  const IntFormat::Options options = {
      .align_to_right = true,
  };

  {
    std::array<char, 0> buffer;
    EXPECT_EQ(IntFormat::FormatToBuffer(options, 123, buffer), 0);
  }

  {
    std::array<char, 1> buffer;
    EXPECT_EQ(IntFormat::FormatToBuffer(options, 123, buffer), 0);
  }

  {
    std::array<char, 2> buffer;
    EXPECT_EQ(IntFormat::FormatToBuffer(options, 123, buffer), 0);
  }

  {
    std::array<char, 3> buffer;

    EXPECT_EQ(IntFormat::FormatToBuffer(options, 123, buffer), 0);
  }

  {
    std::array<char, 4> buffer;

    EXPECT_EQ(IntFormat::FormatToBuffer(options, 123, buffer), 4);
    EXPECT_EQ(std::string_view(buffer.data()), "123");
  }
}

TEST(IntFormat, Default) {
  // Simple positive value.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(IntFormat::Options{}, 123, buffer), 4);
    EXPECT_EQ(std::string_view(buffer.data()), "123");
  }

  // Simple negative value.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(IntFormat::Options{}, -123, buffer), 5);
    EXPECT_EQ(std::string_view(buffer.data()), "-123");
  }

  // Big negative value without thousands separator.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(IntFormat::Options{}, -12345, buffer),
              7);
    EXPECT_EQ(std::string_view(buffer.data()), "-12345");
  }
}

TEST(IntFormat, UsePositivePrefix) {
  // Positive value with explicit "+" prefix.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_positive_prefix = true}, 123, buffer),
              5);
    EXPECT_EQ(std::string_view(buffer.data()), "+123");
  }

  // Zero value.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_positive_prefix = true}, 0, buffer),
              3);
    EXPECT_EQ(std::string_view(buffer.data()), "+0");
  }

  // Zero value, but no prefix.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{
                      .use_positive_prefix = true,
                      .use_positive_zero = false,
                  },
                  0,
                  buffer),
              2);
    EXPECT_EQ(std::string_view(buffer.data()), "0");
  }
}

TEST(IntFormat, ThousandSeparator) {
  // Positive value which does not have any separators.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.thousand_separator = ' '}, 123, buffer),
              4);
    EXPECT_EQ(std::string_view(buffer.data()), "123");
  }

  // Big negative value with thousands separator.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(
        IntFormat::FormatToBuffer(
            IntFormat::Options{.thousand_separator = ' '}, -12345, buffer),
        8);
    EXPECT_EQ(std::string_view(buffer.data()), "-12 345");
  }
}

TEST(IntFormat, AlignToRight) {
  {
    std::array<char, 8> buffer;
    std::fill(buffer.begin(), buffer.end(), 'w');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.align_to_right = true}, 123, buffer),
              4);
    EXPECT_EQ(std::string_view(buffer.data()), "wwww123");
  }
}

TEST(IntFormat, DecimalUnitPrefix) {
  // Small value.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  123,
                  buffer),
              4);
    EXPECT_EQ(std::string_view(buffer.data()), "123");
  }

  // Exact 1k.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  1000,
                  buffer),
              3);
    EXPECT_EQ(std::string_view(buffer.data()), "1k");
  }

  // Exact 2k.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  2000,
                  buffer),
              3);
    EXPECT_EQ(std::string_view(buffer.data()), "2k");
  }

  // Big value without decimal part.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  123000,
                  buffer),
              5);
    EXPECT_EQ(std::string_view(buffer.data()), "123k");
  }

  // Big value with decimal part.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  123004,
                  buffer),
              9);
    EXPECT_EQ(std::string_view(buffer.data()), "123.004k");
  }
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  123045,
                  buffer),
              9);
    EXPECT_EQ(std::string_view(buffer.data()), "123.045k");
  }
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  123456,
                  buffer),
              9);
    EXPECT_EQ(std::string_view(buffer.data()), "123.456k");
  }

  // Space before the unit.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{
                      .use_highest_decimal_unit_prefix = true,
                      .use_space_before_unit = true,
                  },
                  12500,
                  buffer),
              7);
    EXPECT_EQ(std::string_view(buffer.data()), "12.5 k");
  }

  // Ensure trailing zeros of the decimal part are ignored.
  {
    std::array<char, 64> buffer;
    std::fill(buffer.begin(), buffer.end(), 'x');

    EXPECT_EQ(IntFormat::FormatToBuffer(
                  IntFormat::Options{.use_highest_decimal_unit_prefix = true},
                  12500,
                  buffer),
              6);
    EXPECT_EQ(std::string_view(buffer.data()), "12.5k");
  }
}

TEST(IntFormat, FormatToString) {
  {
    std::string str(128, 'x');
    EXPECT_TRUE(IntFormat::FormatToString(
        IntFormat::Options{.use_highest_decimal_unit_prefix = true},
        12500,
        str));
    EXPECT_EQ(str, "12.5k");
  }

  {
    std::string str(8, 'w');
    EXPECT_TRUE(IntFormat::FormatToString(
        IntFormat::Options{.align_to_right = true}, 123, str));
    EXPECT_EQ(str, "wwwww123");
  }
}

}  // namespace radio_core
