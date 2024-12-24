// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/complex_wav_file_reader.h"

#include <functional>

#include "radio_core/unittest/test.h"

namespace radio_core::testing {

TEST(ComplexWAVFileReader, Basic) {
  using Path = std::filesystem::path;

  ComplexWAVFileReader reader;

  const Path filename = testing::TestFileAbsolutePath(
      Path("modulation") / "bpsk" / "bpsk_pn23_665.4ksps_offset.wav");

  EXPECT_TRUE(reader.Open(filename));

  const ComplexWAVFileReader::FormatSpec format_spec = reader.GetFormatSpec();
  EXPECT_EQ(format_spec.num_channels, 2);

  EXPECT_TRUE(
      reader.ForeachSample([](const Complex& sample) { (void)sample; }));
}

}  // namespace radio_core::testing
