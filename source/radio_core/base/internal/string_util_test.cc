// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/string_util.h"

#include <string>
#include <string_view>

#include "radio_core/unittest/test.h"

namespace radio_core {

using String = std::string;
using StringView = std::string_view;

TEST(StringUtil, TrimWhitespaceASCII) {
  EXPECT_EQ(TrimWhitespaceASCII<String>(""), "");
  EXPECT_EQ(TrimWhitespaceASCII<String>("   \t  \t\t  "), "");
  EXPECT_EQ(TrimWhitespaceASCII<String>("   \t IMPORTANT  \t\t  "),
            "IMPORTANT");
  EXPECT_EQ(TrimWhitespaceASCII<String>("   \t FOO BAR  \t\t  "), "FOO BAR");
}

TEST(StringUtil, StringReplaceAll) {
  EXPECT_EQ(StringReplaceAll<String>("", "", "bar"), "");
  EXPECT_EQ(StringReplaceAll<String>("", "foo", "bar"), "");
  EXPECT_EQ(StringReplaceAll<String>("foo", "", "bar"), "foo");
  EXPECT_EQ(StringReplaceAll<String>("foo", "bar", "zoo"), "foo");
  EXPECT_EQ(StringReplaceAll<String>("foo bar zoo", "bar", "zoo"),
            "foo zoo zoo");
  EXPECT_EQ(StringReplaceAll<String>("foo bar bar zoo", "bar", "zoo"),
            "foo zoo zoo zoo");
  EXPECT_EQ(StringReplaceAll<String>("aaa", "a", "ya"), "yayaya");
}

TEST(StringUtil, StringCopy) {
  char buffer[64] = {0};

  StringCopy(buffer, "", 1);
  EXPECT_EQ(StringView(buffer), "");

  StringCopy(buffer, "foo", 1);
  EXPECT_EQ(StringView(buffer), "");

  StringCopy(buffer, "foobar", 3);
  EXPECT_EQ(StringView(buffer), "fo");

  StringCopy(buffer, "foobar", 0);
  EXPECT_EQ(StringView(buffer), "fo");
}

TEST(StringUtil, StringPrintFormat) {
  {
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    char buffer[8] = {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};

    // Small string, fits into the buffer.
    EXPECT_EQ(StringPrintFormat(buffer, sizeof(buffer), "%d", 10), 2);
    EXPECT_EQ(StringView(buffer), "10");
    EXPECT_EQ(buffer[2], '\0');
  }

  // Big destination, will clip.
  {
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    char buffer[8] = {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};

    EXPECT_EQ(StringPrintFormat(
                  buffer, sizeof(buffer), "%d%d%d%d%d", 12, 34, 56, 78, 90),
              10);
    EXPECT_EQ(StringView(buffer), "1234567");
    EXPECT_EQ(buffer[7], '\0');
  }

  // Make sure no write past the given buffer size happens.
  {
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    char buffer[9] = {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};

    EXPECT_EQ(StringPrintFormat(
                  buffer, sizeof(buffer) - 1, "%d%d%d%d%d", 12, 34, 56, 78, 90),
              10);
    EXPECT_EQ(StringView(buffer), "1234567");
    EXPECT_EQ(buffer[7], '\0');
    EXPECT_EQ(buffer[8], 0x7f);
  }
}

}  // namespace radio_core
