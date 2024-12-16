// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/tool/buffered_wav_writer.h"

#include <array>
#include <cstdint>
#include <vector>

#include "radio_core/unittest/memory_file_writer.h"
#include "radio_core/unittest/mock.h"
#include "radio_core/unittest/test.h"
#include "tl_audio_wav/tl_audio_wav_writer.h"

namespace radio_core::tool {

using testing::Eq;
using testing::FileWriterToMemory;
using testing::Pointwise;

template <class FileWriter>
using WAVWriter = tiny_lib::audio_wav_writer::Writer<FileWriter>;
using WAVFormatSpec = tiny_lib::audio_wav_writer::FormatSpec;

TEST(BufferedWAVWriter, SingleSample) {
  static constexpr auto kSamples = std::to_array({
      std::array<int16_t, 2>{3276, 13106},
      std::array<int16_t, 2>{6553, 16383},
      std::array<int16_t, 2>{9830, 19660},
  });

  // Generate expected buffer by using non-buffered writer implementation (there
  // might be internal buffers in the writer itself, but those are expected to
  // be tested elsewhere).
  std::vector<uint8_t> expected_buffer;
  {
    FileWriterToMemory file_writer(expected_buffer);
    WAVWriter<FileWriterToMemory> wav_writer;
    const WAVFormatSpec audio_format_spec = {
        .num_channels = 2,
        .sample_rate = 44100,
        .bit_depth = 16,
    };
    EXPECT_TRUE(wav_writer.Open(file_writer, audio_format_spec));
    for (const auto& sample : kSamples) {
      EXPECT_TRUE(wav_writer.WriteSingleSample<int16_t>(std::span(sample)));
    }
    EXPECT_TRUE(wav_writer.Close());
  }

  // Write samples using the buffered writer.
  std::vector<uint8_t> actual_buffer;
  {
    FileWriterToMemory file_writer(actual_buffer);
    WAVWriter<FileWriterToMemory> wav_writer;
    const WAVFormatSpec audio_format_spec = {
        .num_channels = 2,
        .sample_rate = 44100,
        .bit_depth = 16,
    };
    EXPECT_TRUE(wav_writer.Open(file_writer, audio_format_spec));

    BufferedWAVWriter<int16_t, 65536, FileWriterToMemory> buffered_wav_writer(
        wav_writer);
    for (const auto& sample : kSamples) {
      EXPECT_TRUE(buffered_wav_writer.WriteSingleSample(std::span(sample)));
    }
    EXPECT_TRUE(buffered_wav_writer.Flush());

    EXPECT_TRUE(wav_writer.Close());
  }

  // Compare actual and expected buffers.
  EXPECT_THAT(actual_buffer, Pointwise(Eq(), expected_buffer));
}

TEST(BufferedWAVWriter, WriteMultipleSamples) {
  static constexpr auto kSamples = std::to_array({
      std::array<int16_t, 2>{3276, 13106},
      std::array<int16_t, 2>{6553, 16383},
      std::array<int16_t, 2>{9830, 19660},
  });

  // Generate expected buffer by using non-buffered writer implementation (there
  // might be internal buffers in the writer itself, but those are expected to
  // be tested elsewhere).
  std::vector<uint8_t> expected_buffer;
  {
    FileWriterToMemory file_writer(expected_buffer);
    WAVWriter<FileWriterToMemory> wav_writer;
    const WAVFormatSpec audio_format_spec = {
        .num_channels = 2,
        .sample_rate = 44100,
        .bit_depth = 16,
    };
    EXPECT_TRUE(wav_writer.Open(file_writer, audio_format_spec));
    for (int i = 0; i < 2; ++i) {
      for (const auto& sample : kSamples) {
        EXPECT_TRUE(wav_writer.WriteSingleSample<int16_t>(std::span(sample)));
      }
    }
    EXPECT_TRUE(wav_writer.Close());
  }

  // Write samples in a way that they are first buffered.
  {
    // Write samples using the buffered writer.
    std::vector<uint8_t> actual_buffer;
    {
      FileWriterToMemory file_writer(actual_buffer);
      WAVWriter<FileWriterToMemory> wav_writer;
      const WAVFormatSpec audio_format_spec = {
          .num_channels = 2,
          .sample_rate = 44100,
          .bit_depth = 16,
      };
      EXPECT_TRUE(wav_writer.Open(file_writer, audio_format_spec));

      BufferedWAVWriter<int16_t, 8, FileWriterToMemory> buffered_wav_writer(
          wav_writer);
      EXPECT_TRUE(
          buffered_wav_writer.WriteMultipleSamples(std::span<const int16_t>(
              kSamples[0].data(), kSamples.size() * kSamples[0].size())));
      EXPECT_TRUE(
          buffered_wav_writer.WriteMultipleSamples(std::span<const int16_t>(
              kSamples[0].data(), kSamples.size() * kSamples[0].size())));
      EXPECT_TRUE(buffered_wav_writer.Flush());

      EXPECT_TRUE(wav_writer.Close());
    }

    // Compare actual and expected buffers.
    EXPECT_THAT(actual_buffer, Pointwise(Eq(), expected_buffer));
  }

  // Write samples directly from the input span.
  {
    // Write samples using the buffered writer.
    std::vector<uint8_t> actual_buffer;
    {
      FileWriterToMemory file_writer(actual_buffer);
      WAVWriter<FileWriterToMemory> wav_writer;
      const WAVFormatSpec audio_format_spec = {
          .num_channels = 2,
          .sample_rate = 44100,
          .bit_depth = 16,
      };
      EXPECT_TRUE(wav_writer.Open(file_writer, audio_format_spec));

      BufferedWAVWriter<int16_t, 2, FileWriterToMemory> buffered_wav_writer(
          wav_writer);
      EXPECT_TRUE(
          buffered_wav_writer.WriteMultipleSamples(std::span<const int16_t>(
              kSamples[0].data(), kSamples.size() * kSamples[0].size())));
      EXPECT_TRUE(
          buffered_wav_writer.WriteMultipleSamples(std::span<const int16_t>(
              kSamples[0].data(), kSamples.size() * kSamples[0].size())));
      EXPECT_TRUE(buffered_wav_writer.Flush());

      EXPECT_TRUE(wav_writer.Close());
    }

    // Compare actual and expected buffers.
    EXPECT_THAT(actual_buffer, Pointwise(Eq(), expected_buffer));
  }
}

}  // namespace radio_core::tool
