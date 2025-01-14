// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/complex_wav_file_reader.h"

#include <functional>
#include <vector>

#include "radio_core/unittest/test.h"

namespace radio_core::testing {

using Path = std::filesystem::path;

TEST(ComplexWAVFileReader, GetFormatSpec) {
  const Path filename = testing::TestFileAbsolutePath(
      Path("modulation") / "bpsk" / "bpsk_pn23_665.4ksps_offset.wav");

  ComplexWAVFileReader reader;
  EXPECT_TRUE(reader.Open(filename));

  const ComplexWAVFileReader::FormatSpec format_spec = reader.GetFormatSpec();
  EXPECT_EQ(format_spec.num_channels, 2);
}

TEST(ComplexWAVFileReader, Basic) {
  const Path filename = testing::TestFileAbsolutePath(
      Path("modulation") / "bpsk" / "bpsk_pn23_665.4ksps_offset.wav");

  ComplexWAVFileReader reader;
  EXPECT_TRUE(reader.Open(filename));

  std::vector<Complex> all_samples;
  EXPECT_TRUE(reader.ForeachSample(
      [&](const Complex& sample) { all_samples.push_back(sample); }));
  EXPECT_EQ(all_samples.size(), 65536 * 4);
}

TEST(ComplexWAVFileReader, Buferred) {
  const Path filename = testing::TestFileAbsolutePath(
      Path("modulation") / "bpsk" / "bpsk_pn23_665.4ksps_offset.wav");

  ComplexWAVFileReader reader;
  EXPECT_TRUE(reader.Open(filename));

  std::vector<Complex> all_samples;
  // Read in the buffer of size that is not an integer multiple of the overall
  // number of samples.
  EXPECT_TRUE(reader.ForeachSampleBuffered<101>(
      [&](const std::span<const Complex> new_samples) {
        all_samples.insert(
            all_samples.end(), new_samples.begin(), new_samples.end());
      }));
  EXPECT_EQ(all_samples.size(), 65536 * 4);
}

}  // namespace radio_core::testing
