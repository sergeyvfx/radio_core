// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/memory_file_writer.h"

#include <vector>

#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"

namespace radio_core::testing {

using testing::Eq;
using testing::Pointwise;

TEST(FileWriterToMemory, Simple) {
  std::vector<uint8_t> data;
  FileWriterToMemory writer(data);

  EXPECT_EQ(writer.Write("Hello, ", 7), 7);
  EXPECT_EQ(writer.Write("World!", 6), 6);
  EXPECT_THAT(data, Pointwise(Eq(), std::string_view("Hello, World!")));
}

TEST(FileWriterToMemory, Rewind) {
  std::vector<uint8_t> data;
  FileWriterToMemory writer(data);

  EXPECT_EQ(writer.Write("Hello, World!", 13), 13);
  EXPECT_TRUE(writer.Rewind());
  EXPECT_EQ(writer.Write("123", 3), 3);

  EXPECT_THAT(data, Pointwise(Eq(), std::string_view("123lo, World!")));
}

TEST(SimpleFileWriterToMemory, Simple) {
  SimpleFileWriterToMemory writer;

  EXPECT_EQ(writer.Write("Hello, ", 7), 7);
  EXPECT_EQ(writer.Write("World!", 6), 6);
  EXPECT_THAT(writer.buffer,
              Pointwise(Eq(), std::string_view("Hello, World!")));
}

}  // namespace radio_core::testing
