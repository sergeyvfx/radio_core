// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/datalink/ax25/message.h"

#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink::ax25 {

TEST(FixedString, Construct) {
  using ax25_internal::FixedString;

  // Default initialization.
  {
    FixedString<6, ' '> str;
    EXPECT_EQ(str, std::string_view("      "));
  }

  // Initialization with string of an exact length.
  {
    FixedString<6, ' '> str("123456");
    EXPECT_EQ(str, std::string_view("123456"));
  }

  // Initialization with string of a shorter length.
  {
    FixedString<6, ' '> str("1234  ");
    EXPECT_EQ(str, std::string_view("1234  "));
  }

  // Initialization with string of an longer length.
  {
    FixedString<6, ' '> str("1234567890");
    EXPECT_EQ(str, std::string_view("123456"));
  }
}

TEST(FixedString, Assign) {
  using ax25_internal::FixedString;

  // Assign string of an exact length.
  {
    FixedString<6, ' '> str;
    str = "123456";
    EXPECT_EQ(str, std::string_view("123456"));
  }

  // Assign string of a shorter length.
  {
    FixedString<6, ' '> str;
    str = "1234";
    EXPECT_EQ(str, std::string_view("1234  "));
  }

  // Assign string of a longer length.
  {
    FixedString<6, ' '> str;
    str = "1234567890";
    EXPECT_EQ(str, std::string_view("123456"));
  }
}

TEST(FixedString, GetCleanView) {
  using ax25_internal::FixedString;

  {
    FixedString<6, ' '> str("123");
    EXPECT_EQ(str.GetCleanView(), std::string_view("123"));
  }

  {
    FixedString<6, ' '> str("123456");
    EXPECT_EQ(str.GetCleanView(), std::string_view("123456"));
  }

  {
    FixedString<6, '\0'> str("123");
    EXPECT_EQ(str.GetCleanView(), std::string_view("123"));
    EXPECT_EQ(str.GetCleanView().size(), 3);
  }
}

TEST(Information, Assign) {
  {
    Information info;
    info = "Hello, World!";
    EXPECT_EQ(info, std::string_view("Hello, World!"));
  }
}

TEST(Message, GetVersion) {
  // Frame version 1.
  {
    Message message;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 0;
    EXPECT_EQ(message.GetVersion(), 1);
  }
  {
    Message message;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 1;
    EXPECT_EQ(message.GetVersion(), 1);
  }

  // Frame version 2.
  {
    Message message;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 0;
    EXPECT_EQ(message.GetVersion(), 2);
  }
  {
    Message message;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 1;
    EXPECT_EQ(message.GetVersion(), 2);
  }
}

TEST(Message, IsCommand) {
  // Frame version 1.
  {
    Message message;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 0;
    EXPECT_FALSE(message.IsCommand());
  }
  {
    Message message;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 1;
    EXPECT_FALSE(message.IsCommand());
  }

  // Frame version 2.
  {
    Message message;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 0;
    EXPECT_FALSE(message.IsCommand());
  }
  {
    Message message;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 1;
    EXPECT_TRUE(message.IsCommand());
  }
}

TEST(Message, IsResponse) {
  // Frame version 1.
  {
    Message message;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 0;
    EXPECT_FALSE(message.IsResponse());
  }
  {
    Message message;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 1;
    EXPECT_FALSE(message.IsResponse());
  }

  // Frame version 2.
  {
    Message message;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 0;
    EXPECT_TRUE(message.IsResponse());
  }
  {
    Message message;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 1;
    EXPECT_FALSE(message.IsResponse());
  }
}

}  // namespace radio_core::protocol::datalink::ax25
