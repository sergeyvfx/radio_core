// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/datalink/ax25/print.h"

#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink::ax25 {

TEST(EncodeMessageInfo, Information) {
  {
    Message message;
    char buffer[1024];
    message.control = 0b11111110;
    message.pid = 123;
    message.information = "Hello, world!";
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "I F PID=0x7B Len=13");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11111110;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 0;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "I R F PID=0x00 Len=0");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11111110;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 1;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "I C F PID=0x00 Len=0");
  }
}

TEST(EncodeMessageInfo, Supervisory) {
  {
    Message message;
    char buffer[1024];
    message.control = 0b11110001;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "S F RR");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11110001;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 0;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "S R F RR");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11110001;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 1;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "S C F RR");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11110101;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "S F RNR");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11111001;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "S F REJ");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11111101;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "S F SREJ");
  }
}

TEST(EncodeMessageInfo, Unnumbered) {
  {
    Message message;
    char buffer[1024];
    message.control = 0b11111111;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "U F");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11111111;
    message.address.source.command_response_bit = 1;
    message.address.destination.command_response_bit = 0;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "U R F");
  }

  {
    Message message;
    char buffer[1024];
    message.control = 0b11111111;
    message.address.source.command_response_bit = 0;
    message.address.destination.command_response_bit = 1;
    EncodeMessageInfo(message, buffer);
    EXPECT_STREQ(buffer, "U C F");
  }
}

}  // namespace radio_core::protocol::datalink::ax25
