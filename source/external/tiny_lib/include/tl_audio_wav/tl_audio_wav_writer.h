// Copyright (c) 2022 tiny lib authors
//
// SPDX-License-Identifier: MIT-0

// Writer of WAVE files.
//
// The writer implements streamed and bulk writer of WAVE files.
//
// In the bulk mode the file is written using given set of samples, and file is
// written in one go.
//
// In a streamed mode samples are written incrementally to the file. In this
// mode the underlying file needs to support rewind to the beginning of the
// file as the WAVE header needs to be adjusted after the final number of
// samples is known.
//
// Example
// =======
//
//   auto my_samples = std::to_array({
//       std::array<float, 2>{0.1f, 0.4f},
//       std::array<float, 2>{0.2f, 0.5f},
//       std::array<float, 2>{0.3f, 0.6f},
//   });
//
//   // Streamed writer.
//
//   const FormatSpec format_spec = {
//       .num_channels = 2,
//       .sample_rate = 44100,
//       .bit_depth = 16,
//   };
//
//   MyFileWriter my_file_writer;
//   Writer<MyFileWriter> wav_writer;
//
//   wav_writer.Open(my_file_writer, format_spec);
//
//   for (const auto& sample : my_samples) {
//     wav_writer.WriteSingleSample(sample);
//   }
//
//   wav_writer.Close();
//
//   // Streamed buffered writer.
//
//   const FormatSpec format_spec = {
//       .num_channels = 2,
//       .sample_rate = 44100,
//       .bit_depth = 16,
//   };
//
//   MyFileWriter my_file_writer;
//   Writer<MyFileWriter> wav_writer;
//
//   wav_writer.Open(my_file_writer, format_spec);
//   wav_writer.WriteMultipleSamples(samples_buffer);
//   wav_writer.Close();
//
//   // Bulked writer.
//
//   const FormatSpec format_spec = {
//       .num_channels = 2,
//       .sample_rate = 44100,
//       .bit_depth = 16,
//   };
//
//   Writer<MyFileWriter>::Write(MyFileWriter(), format_spec, my_samples);
//
// File Writer
// ===========
//
// The file writer is to implement a Write() method which can be invoked with
// the following signature:
//
//   auto Write(const void* ptr, SizeType num_bytes_to_write) -> IntType;
//
// It is possible to have trailing arguments with default values in the
// method if they do not change the expected invoke arguments.
//
// The num_bytes_to_write denotes the number of bytes which the WAV writer
// expects to be written to the file.
//
// Upon success the Write() is to return the number of bytes actually written.
//
// The WAV writer expects the number of actually written bytes to match the
// number of requested bytes to be written. This gives flexibility to the error
// handling in the file writer implementation: both write() and fwrite() style
// of the return value in the case of error or reading past the EOF will work
// for the WAV writer.
//
// When using a streamed API then additionally the following method is to be
// implemented:
//
//   auto Rewind() -> bool;
//
// which rewinds current position in the file to its beginning, allowing to
// override content of file.
//
// The return value is defined as following:
//
//   - Upon success true is returned
//   - Otherwise false is returned.
//
// It is possible to have trailing arguments with default values in those
// methods if they fo not change the expected semantic.
//
//
// Limitations
// ===========
//
// - Only writing WAVE as uncompressed PCM 16-bit signed integer data type is
//   supported.
//
// - Streaming is not optimized: it can take substantial time to write float
//   samples to int16 file.
//
//
// Version history
// ===============
//
//   0.0.2-alpha    (13 Dec 2024)    Various improvements with the goal to
//                                   support buffered writing:
//                                   - Implement buffered writing.
//                                   - Added WriteMultipleSamples().
//                                   - Fix incorrect check for the number of
//                                     written bytes.
//                                   - Added access to file specification:
//                                     writer.GetFileSpec().
//                                   - [[nodiscard]] is used for internal
//                                     functions.
//   0.0.1-alpha    (28 Dec 2023)    First public release.

#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <span>

// Semantic version of the tl_audio_wav_writer library.
#define TL_AUDIO_WAV_WRITER_VERSION_MAJOR 0
#define TL_AUDIO_WAV_WRITER_VERSION_MINOR 0
#define TL_AUDIO_WAV_WRITER_VERSION_REVISION 1

// Namespace of the module.
// The outer name spaces which surrounds the ABI-version namespace.
#ifndef TL_AUDIO_WAV_WRITER_NAMESPACE
#  define TL_AUDIO_WAV_WRITER_NAMESPACE tiny_lib::audio_wav_writer
#endif

// The size of the buffer used for single frame-sample.
//
// It is used, for example, in WriteSingleSample() to optimize disk and memory
// access pattern. Measured in the number of per-channel samples. The default
// value covers the case of surround files with center channel.
//
// Must be non-negative.
// Value of 0 or 1 disables buffered writing, which leads to more poor
// performance but uses less stack memory.
#ifndef TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE
#  define TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE 5
#endif

// The size of the buffer used for storing multiple frame-samples.
//
// It is used, for example, in WriteMultipleSamples() to optimize disk and
// memory access pattern. Measured in the number of per-channel samples.
//
// Must be non-negative.
// Value of 0 or 1 disables buffered writing, which leads to more poor
// performance but uses less stack memory.
#ifndef TL_AUDIO_WAV_WRITER_BUFFER_SIZE
#  define TL_AUDIO_WAV_WRITER_BUFFER_SIZE                                      \
    (TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE * 32)
#endif

// Helpers for TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE.
//
// Typical extra indirection for such conversion to allow macro to be expanded
// before it is converted to string.
#define TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE_CONCAT_HELPER(id1, id2, id3)     \
  v_##id1##_##id2##_##id3
#define TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE_CONCAT(id1, id2, id3)            \
  TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE_CONCAT_HELPER(id1, id2, id3)

// Constructs identifier suitable for namespace denoting the current library
// version.
//
// For example: TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE -> v_0_1_9
#define TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE                                  \
  TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE_CONCAT(                                \
      TL_AUDIO_WAV_WRITER_VERSION_MAJOR,                                       \
      TL_AUDIO_WAV_WRITER_VERSION_MINOR,                                       \
      TL_AUDIO_WAV_WRITER_VERSION_REVISION)

// NOLINTNEXTLINE(modernize-concat-nested-namespaces)
namespace TL_AUDIO_WAV_WRITER_NAMESPACE {
inline namespace TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE {

////////////////////////////////////////////////////////////////////////////////
// Public declarations.

// Information about format in which the audio data is stored in a WAV file.
struct FormatSpec {
  // Number of channels.
  // Rather self-explanatory: 1 is mono, 2 is stereo, 5 is surround.
  int num_channels{-1};

  // Sample rate in samples per second.
  int sample_rate{-1};

  // Bit depth for per-channel sample values.
  // For example: 16 means values are stored as 16-bit signed integers.
  int bit_depth{-1};
};

template <class FileWriter>
class Writer {
 public:
  // Constructs a new WAV file writer.
  // The writer is not ready for writing and is to be opened first.
  Writer() = default;

  Writer(Writer&& other) noexcept = default;
  auto operator=(Writer&& other) -> Writer& = default;

  // Delete copy constructor and assignment operator.
  //
  // Simplifies the FileWriter API by avoiding requirement to implement the
  // copy file descriptor API call.
  Writer(const Writer& other) noexcept = delete;
  auto operator=(const Writer& other) -> Writer& = delete;

  ~Writer() = default;

  // Calculate maximum number of samples which can be stored in a WAV file with
  // the given format specification.
  // The sample include values of all channels.
  static inline auto MaxNumSamples(const FormatSpec& format_spec) -> uint32_t;

  // Open file for writing samples with the given specification into the file
  // writer.
  //
  // The file writer is referenced by the WAV writer and caller is to guarantee
  // it is valid throughout the WAV file writing.
  //
  // Writes a temporary head to the file, so that if the application execution
  // is aborted it is still possible to read samples which were written prior to
  // the failure.
  //
  // Multiple calls of Open() on the same file is undefined.
  //
  // Returns true upon success.
  auto Open(FileWriter& file_writer, const FormatSpec& format_spec) -> bool;

  // Get format specification of the file.
  //
  // Requires WAV file to be successfully open for write first and will have an
  // undefined behavior if the Open() returned false.
  inline auto GetFormatSpec() const -> const FormatSpec&;

  // Write single sample which consists of values for all channels.
  //
  // The number of values must match the number of channels in the format
  // specification otherwise an error is returned.
  //
  // Returns true upon successful write.
  template <class ValueType>
  auto WriteSingleSample(std::span<const ValueType> sample) -> bool;

  // Write multiple samples to the file.
  //
  // The samples are provided as a flattened memory of audio frame samples:
  //  {
  //    sample 1 ch 1, sample 1 ch 2  ...  sample 1 ch N,
  //    sample 2 ch 1, sample 2 ch 2  ...  sample 2 ch N,
  //    ...
  //    sample M ch 1, sample M ch 2  ...  sample M ch N,
  //  }
  //
  // The number of channels in every ampel must match the number of channels in
  // the format specification, and the number of frames should be integer.
  // Otherwise an error is returned.
  template <class ValueType>
  auto WriteMultipleSamples(std::span<const ValueType> samples) -> bool;

  // Close the file writer by writing the final header.
  //
  // Calling on a writer which is not opened is undefined.
  //
  // The WAV writer is to be closed explicitly, otherwise a data loss might
  // occur.
  auto Close() -> bool;

  // Create WAV file from the given format specification and samples.
  //
  // This call writes the complete file, without ability to append samples to
  // the file.
  //
  // The samples storage is expected to be a container of sample, and every
  // sample is a container of per-channel values of that sample. The number of
  // channels in all samples must match the number of channels specified in the
  // format.
  //
  // Returns true if the file is fully written.
  template <class FileWriterType, class ValueType>
  static auto Write(FileWriterType&& file_writer,
                    const FormatSpec& format_spec,
                    std::span<const ValueType> samples) -> bool;

 private:
  // Write placeholder header upon the file Open().
  //
  // Will write all the known information from the current format specification,
  // but will keep the chunk sizes at a high value allowing to read partially
  // saved file.
  [[nodiscard]] auto WritePlaceholderHeader() -> bool;

  // Write the final header.
  //
  // Is supposed to be called after rewinding the file writer back to the
  // beginning.
  [[nodiscard]] auto WriteFinalHeader() -> bool;

  // Internal implementation of the public WriteSingleSample().
  //
  // Uses a value type used for storage as a template argument.
  template <class ValueTypeInFile, class ValueTypeInBuffer>
  [[nodiscard]] auto WriteSingleSample(
      std::span<const ValueTypeInBuffer> sample) -> bool;

  // Internal implementation of the public WriteMultipleSamples().
  //
  // Uses a value type used for storage as a template argument.
  template <class ValueTypeInFile, class ValueTypeInBuffer>
  [[nodiscard]] auto WriteMultipleSamples(
      std::span<const ValueTypeInBuffer> samples) -> bool;

  // Internal implementation of the public Write().
  //
  // Uses a value type stored in the file as a templated argument.
  template <class ValueTypeInFile,
            class FileWriterType,
            class ValueTypeInBuffer>
  [[nodiscard]] static auto Write(FileWriterType&& file_writer,
                                  const FormatSpec& format_spec,
                                  std::span<const ValueTypeInBuffer> samples)
      -> bool;

  FileWriter* file_writer_{nullptr};

  bool is_open_attempted_{false};  // True if Open() has been called.
  bool is_open_{false};            // True if the Open() returned true.

  FormatSpec format_spec_;

  uint32_t num_samples_written_ = 0;
};

#if defined(_MSC_VER)
#  pragma warning(push)
// 4702 - Unreachable code.
//        MSVC might consider some fall-back code paths consider unreachable.
//        For example, assert() and default return value as a recovery.
//        Another example, is having unbuffered code after `if constexpr ()`
//        which evaluates to true for the buffered reading.
#  pragma warning(disable : 4702)
#endif

////////////////////////////////////////////////////////////////////////////////
// WAV data types declaration.

namespace internal {

// Data structures used by a WAVE file are summarized in:
//
//   WAVE PCM soundfile format
//   http://soundfile.sapp.org/doc/WaveFormat/
//
// Some deviations are done to make chunk ID and size a more reusable entity
// other than operating on Chunk and Subchunk notation.
//
// In our terms the WAVE PCM file consists of chunks. Every chunk consists of
// its header and data. The required chunks are RIFF, FMT, and DATA.
//
// The RIFF chunk defines the top-level file information: the format used in the
// file. The RIFF chunk is identified as RIFF in files stored in the little
// endian and as RIFX for files stored in the big endian.
//
// The size stored in the RIFF chunk includes the RIFF data AND the rest of the
// file. In other words, the RIFF chunk acts as a container for the entire audio
// file.
//
// The FMT chunk contains channels and sampling specification (number of
// channels, sample rate, etc.).
//
// The DATA chunk contains samples.
//
// The file might have more chunks than the listed above. Although the ones from
// above are the required minimum for a proper audio file.

// Convert 4 character string representation of an ID to an unsigned 32-bit
// integer value in a way that it can be compared to a value directly read from
// file without runtime endian conversion.
constexpr auto IDStringToUInt32(const char a,
                                const char b,
                                const char c,
                                const char d) -> uint32_t {
  if constexpr (std::endian::native == std::endian::little) {
    return (uint8_t(a) << 0) | (uint8_t(b) << 8) | (uint8_t(c) << 16) |
           (uint8_t(d) << 24);
  }

  if constexpr (std::endian::native == std::endian::big) {
    return (uint8_t(a) << 24) | (uint8_t(b) << 16) | (uint8_t(c) << 8) |
           (uint8_t(d) << 0);
  }
}

// Mnemonics for known and supported chunk identifiers.
//
// Derived from the ASCII string representation stored as 32-bit integer.
//
// The value of ID matches the value when it is read from a binary file as
// uint32 without endian conversion.
enum class ChunkID : uint32_t {
  // RIFF corresponds to a WAVE files stored in little endian.
  kRIFF = IDStringToUInt32('R', 'I', 'F', 'F'),

  // RIFX is RIFF header but for a file using big endian.
  kRIFX = IDStringToUInt32('R', 'I', 'F', 'X'),

  kFMT = IDStringToUInt32('f', 'm', 't', ' '),
  kDATA = IDStringToUInt32('d', 'a', 't', 'a'),
};

// Mnemonics for known and supported WAVE formats.
//
// Derived from the ASCII representation in the file stored as a hexadecimal
enum class Format : uint32_t {
  kWAVE = IDStringToUInt32('W', 'A', 'V', 'E'),
};

// Audio format stored in the FMT chunk.
enum class AudioFormat : uint16_t {
  kPCM = 1,
};

// Header of a chunk.
// Identifies what the coming chunk is and what its size is.
struct ChunkHeader {
  ChunkID id;     // Identifier of this chunk.
  uint32_t size;  // Size of the chunk in bytes.
};
static_assert(sizeof(ChunkHeader) == 8);

// Data of a RIFF chunk (`ChunkID::RIFF` for files stored in little endian and
// `ChunkID::RIFX` for files stored in big endian).
struct RIFFData {
  Format format;  // Format of the file.
};
static_assert(sizeof(RIFFData) == 4);

struct FormatData {
  // Format in which audio data is stored.
  // Mnemonic representation is in `AudioFormat` enumerator.
  AudioFormat audio_format;

  // Number of channels.
  // Rather self-explanatory: 1 is mono, 2 is stereo, 5 is surround.
  uint16_t num_channels;

  // Sample rate in samples per second.
  uint32_t sample_rate;

  // sample_rate * num_channels * bit_depth/8
  uint32_t byte_rate;
  // num_channels * bit_depth/8
  uint16_t block_align;

  // Bits per sample of a single channel: 16 means samples are stored as 16-bit
  // integers.
  uint16_t bit_depth;
};
static_assert(sizeof(FormatData) == 16);

}  // namespace internal

////////////////////////////////////////////////////////////////////////////////
// Public API implementation.

template <class FileWriter>
inline auto Writer<FileWriter>::MaxNumSamples(const FormatSpec& format_spec)
    -> uint32_t {
  constexpr uint32_t kMaxDataBytes =
      0xffffffff - sizeof(internal::RIFFData) -
      // Header and data of the FMT chunk.
      sizeof(internal::ChunkHeader) - sizeof(internal::FormatData) -
      // Header and data of the DATA chunk (samples).
      sizeof(internal::ChunkHeader);

  const uint32_t byte_depth = format_spec.bit_depth / 8;
  return kMaxDataBytes / byte_depth / format_spec.num_channels;
}

template <class FileWriter>
auto Writer<FileWriter>::Open(FileWriter& file_writer,
                              const FormatSpec& format_spec) -> bool {
  assert(!is_open_attempted_);
  is_open_attempted_ = true;

  file_writer_ = &file_writer;
  format_spec_ = format_spec;

  is_open_ = true;

  if (!WritePlaceholderHeader()) {
    is_open_ = false;
    return false;
  }

  return true;
}

template <class FileWriter>
inline auto Writer<FileWriter>::GetFormatSpec() const -> const FormatSpec& {
  assert(is_open_);

  return format_spec_;
}

template <class FileWriter>
template <class ValueType>
auto Writer<FileWriter>::WriteSingleSample(
    const std::span<const ValueType> sample) -> bool {
  switch (format_spec_.bit_depth) {
    case 16: return WriteSingleSample<int16_t, ValueType>(sample);
  }
  return false;
}

template <class FileWriter>
template <class ValueType>
auto Writer<FileWriter>::WriteMultipleSamples(
    const std::span<const ValueType> samples) -> bool {
  switch (format_spec_.bit_depth) {
    case 16: return WriteMultipleSamples<int16_t, ValueType>(samples);
  }
  return false;
}

template <class FileWriter>
auto Writer<FileWriter>::Close() -> bool {
  assert(is_open_);

  if (!file_writer_->Rewind()) {
    return false;
  }

  if (!WriteFinalHeader()) {
    return false;
  }

  is_open_ = false;
  file_writer_ = nullptr;

  return true;
}

template <class FileWriter>
template <class FileWriterType, class ValueType>
auto Writer<FileWriter>::Write(FileWriterType&& file_writer,
                               const FormatSpec& format_spec,
                               const std::span<const ValueType> samples)
    -> bool {
  switch (format_spec.bit_depth) {
    case 16: return Write<int16_t>(file_writer, format_spec, samples);
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Private implementation.

namespace internal {

template <class T>
inline auto Byteswap(T value) -> T;

// Swap endianess of 16 bit unsigned int value.
template <>
[[nodiscard]] inline auto Byteswap(const uint16_t value) -> uint16_t {
#if defined(__GNUC__)
  return __builtin_bswap16(value);
#elif defined(_MSC_VER)
  static_assert(sizeof(unsigned short) == 2, "Size of short should be 2");
  return _byteswap_ushort(value);
#else
  return (value >> 8) | (value << 8);
#endif
}

// Swap endianess of 32 bit unsigned int value.
template <>
[[nodiscard]] inline auto Byteswap(const uint32_t value) -> uint32_t {
#if defined(__GNUC__)
  return __builtin_bswap32(value);
#elif defined(_MSC_VER)
  static_assert(sizeof(unsigned long) == 4, "Size of long should be 4");
  return _byteswap_ulong(value);
#else
  return ((value >> 24)) | ((value << 8) & 0x00ff0000) |
         ((value >> 8) & 0x0000ff00) | ((value << 24));
#endif
}

template <class T>
[[nodiscard]] inline auto Byteswap(const T value) -> T {
  static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4);

  if constexpr (sizeof(T) == 1) {
    return value;
  }

  if constexpr (sizeof(T) == 2) {
    uint16_t result = Byteswap(*reinterpret_cast<const uint16_t*>(&value));
    return *reinterpret_cast<T*>(&result);
  }

  if constexpr (sizeof(T) == 4) {
    uint32_t result = Byteswap(*reinterpret_cast<const uint32_t*>(&value));
    return *reinterpret_cast<T*>(&result);
  }

  assert(!"Unhandled type for Byteswap");

  return {};
}

// Calculate size which is to be specified in the RIFF/RIFX chunk header.
// This size is the size of the entire WAV file minus the header of the
// RIFF/RIFX chunk header.
[[nodiscard]] inline auto CalculateRIFFContainerSize(
    const uint32_t num_data_bytes) -> uint32_t {
  return
      // Data of the RIFF/RIFX chunk.
      sizeof(RIFFData) +
      // Header and data of the FMT chunk.
      sizeof(ChunkHeader) + sizeof(FormatData) +
      // Header and data of the DATA chunk (samples).
      sizeof(ChunkHeader) + num_data_bytes;
}

[[nodiscard]] inline auto FormatSpecToFormatData(const FormatSpec& format_spec)
    -> FormatData {
  FormatData format_data;

  const uint32_t byte_depth = format_spec.bit_depth / 8;

  format_data.audio_format = AudioFormat::kPCM;
  format_data.num_channels = format_spec.num_channels;
  format_data.sample_rate = format_spec.sample_rate;
  format_data.bit_depth = format_spec.bit_depth;

  format_data.byte_rate =
      format_data.sample_rate * format_data.num_channels * byte_depth;
  format_data.block_align = format_data.num_channels * byte_depth;

  return format_data;
}

template <class FileWriter, class T>
[[nodiscard]] inline auto WriteObjectToFile(FileWriter& file_writer,
                                            const T& object) -> bool {
  const void* object_ptr = static_cast<const void*>(&object);
  constexpr size_t kObjectSize = sizeof(T);

  return file_writer.Write(object_ptr, kObjectSize) == kObjectSize;
}

// Write WAVE header to the file, starting at the current position in the file.
//
// Writes all sections of header up to and including the DATA chunk header.
template <class FileWriter>
[[nodiscard]] inline auto WriteHeader(FileWriter& file_writer,
                                      const FormatSpec& format_spec,
                                      const uint32_t num_samples) -> bool {
  const uint32_t byte_depth = format_spec.bit_depth / 8;
  const uint32_t num_data_bytes =
      num_samples * byte_depth * format_spec.num_channels;

  // RIFF header and data.

  constexpr ChunkID riff_id = (std::endian::native == std::endian::little)
                                  ? ChunkID::kRIFF
                                  : ChunkID::kRIFX;

  const uint32_t riff_container_size =
      CalculateRIFFContainerSize(num_data_bytes);

  if (!WriteObjectToFile(
          file_writer,
          ChunkHeader{.id = riff_id, .size = riff_container_size})) {
    return false;
  }

  if (!WriteObjectToFile(file_writer, RIFFData{.format = Format::kWAVE})) {
    return false;
  }

  // Format.

  if (!WriteObjectToFile(
          file_writer,
          ChunkHeader{.id = ChunkID::kFMT, .size = sizeof(FormatData)})) {
    return false;
  }

  if (!WriteObjectToFile(file_writer, FormatSpecToFormatData(format_spec))) {
    return false;
  }

  // Data header.

  if (!WriteObjectToFile(
          file_writer,
          ChunkHeader{.id = ChunkID::kDATA, .size = num_data_bytes})) {
    return false;
  }

  return true;
}

template <class FromType, class ToType>
auto ConvertValue(FromType sample) -> ToType;

template <>
[[nodiscard]] inline auto ConvertValue(const float sample) -> int16_t {
  // There does not seem to be the standard and different implementations are
  // using different quantization rules.
  //
  // Follow the following
  //
  //   How to handle asymmetry of WAV data?
  //   https://gist.github.com/endolith/e8597a58bcd11a6462f33fa8eb75c43d
  //
  // which states that AES17 defines full-scale in a way that the most negative
  // value of a fixed integer type is unused.

  if (sample <= -1.0f) {
    return -32767;
  }
  if (sample >= 1.0f) {
    return 32767;
  }

  return int16_t(sample * 32767.0f);
}

template <>
[[nodiscard]] inline auto ConvertValue(const int16_t sample) -> int16_t {
  return sample;
}

// Write single sample without using any buffering.
//
// Writes the entire sample, without performing any checks about validity.
//
// Returns true on success, false otherwise.
template <class ValueTypeInFile, class FileWriter, class ValueTypeInBuffer>
[[nodiscard]] inline auto WriteSingleSampleUnbuffered(
    FileWriter& file_writer, const std::span<const ValueTypeInBuffer> sample)
    -> bool {
  for (auto value_in_buffer : sample) {
    const ValueTypeInFile value_in_file =
        ConvertValue<ValueTypeInBuffer, ValueTypeInFile>(value_in_buffer);

    if (file_writer.Write(&value_in_file, sizeof(ValueTypeInFile)) !=
        sizeof(ValueTypeInFile)) {
      return false;
    }
  }

  return true;
}

// Write single sample using buffer to optimize disk and memory access.
//
// Writes the entire sample, without performing any checks about validity.
//
// Returns true on success, false otherwise.
template <class ValueTypeInFile, class FileWriter, class ValueTypeInBuffer>
[[nodiscard]] inline auto WriteSingleSampleBuffered(
    FileWriter& file_writer, const std::span<const ValueTypeInBuffer> sample)
    -> bool {
  static_assert(TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE >= 0);

  // This is more of provisionary check, to avoid possible warnings about zero
  // sized buffer.
  if constexpr (TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE > 1) {
    const size_t num_channels_to_write = sample.size();
    size_t num_channels_written = 0;

    std::array<ValueTypeInFile, TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE>
        buffer;

    if (num_channels_to_write <= TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE) {
      // An optimized version with less branching for cases when the
      // frame-sample fits into the buffer.

      const size_t num_bytes_to_write =
          num_channels_to_write * sizeof(ValueTypeInFile);

      // Convert type from how it's stored in the file to the return sample
      // type.
      for (size_t i = 0; i < num_channels_to_write; ++i) {
        buffer[i] = ConvertValue<ValueTypeInBuffer, ValueTypeInFile>(
            sample[num_channels_written++]);
      }

      // Write channels.
      if (file_writer.Write(buffer.data(), num_bytes_to_write) !=
          num_bytes_to_write) {
        return false;
      }

      return true;
    }

    // A non-optimized version which handles an arbitrary number of channels.

    while (num_channels_written != num_channels_to_write) {
      const size_t n =
          std::min(size_t(TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE),
                   num_channels_to_write - num_channels_written);
      const size_t num_bytes_to_write = n * sizeof(ValueTypeInFile);

      // Convert type from how it's stored in the file to the return sample
      // type.
      for (size_t i = 0; i < n; ++i) {
        buffer[i] = ConvertValue<ValueTypeInBuffer, ValueTypeInFile>(
            sample[num_channels_written++]);
      }
      // Write channels.
      if (file_writer.Write(buffer.data(), num_bytes_to_write) !=
          num_bytes_to_write) {
        return false;
      }
    }

    return true;
  }

  return false;
}

// Write single sample. Will choose the best strategy to use based on the
// current configuration.
//
// Writes the entire sample, without performing any checks about validity.
//
// Returns true on success, false otherwise.
template <class ValueTypeInFile, class FileWriter, class ValueTypeInBuffer>
[[nodiscard]] inline auto WriteSingleSample(
    FileWriter& file_writer, const std::span<const ValueTypeInBuffer> sample)
    -> bool {
  static_assert(TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE >= 0);

  if constexpr (TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE > 1) {
    return WriteSingleSampleBuffered<ValueTypeInFile>(file_writer, sample);
  }

  return WriteSingleSampleUnbuffered<ValueTypeInFile>(file_writer, sample);
}

// Write multiple samples without using any buffering.
//
// Writes the entire samples buffer, without performing any checks about
// validity.
//
// Returns true on success, false otherwise.
template <class ValueTypeInFile, class FileWriter, class ValueTypeInBuffer>
[[nodiscard]] inline auto WriteMultipleSamplesUnbuffered(
    FileWriter& file_writer,
    const size_t num_frames,
    const size_t num_channels,
    const std::span<const ValueTypeInBuffer> samples) -> bool {
  const ValueTypeInBuffer* current_frame_data = samples.data();
  for (size_t frame = 0; frame < num_frames; ++frame) {
    if (!WriteSingleSample<ValueTypeInFile>(
            file_writer, std::span(current_frame_data, num_channels))) {
      return false;
    }
    current_frame_data += num_channels;
  }
  return true;
}

// Write multiple samples using buffer to optimize disk and memory access.
//
// Writes the entire samples buffer, without performing any checks about
// validity.
//
// Returns true on success, false otherwise.
template <class ValueTypeInFile, class FileWriter, class ValueTypeInBuffer>
[[nodiscard]] inline auto WriteMultipleSamplesBuffered(
    FileWriter& file_writer,
    const size_t /*num_frames*/,
    const size_t /*num_channels*/,
    const std::span<const ValueTypeInBuffer> samples) -> bool {
  static_assert(TL_AUDIO_WAV_WRITER_BUFFER_SIZE >= 0);

  // This is more of provisionary check, to avoid possible warnings about zero
  // sized buffer.
  if constexpr (TL_AUDIO_WAV_WRITER_BUFFER_SIZE > 1) {
    // Buffer for reading from the file.
    std::array<ValueTypeInFile, TL_AUDIO_WAV_WRITER_BUFFER_SIZE> buffer;

    size_t num_samples_written = 0;
    size_t num_remaining_samples = samples.size();

    while (num_remaining_samples != 0) {
      const size_t num_samples_to_write = std::min(
          size_t(TL_AUDIO_WAV_WRITER_BUFFER_SIZE), num_remaining_samples);
      const size_t num_bytes_to_write =
          num_samples_to_write * sizeof(ValueTypeInFile);

      // Convert type from how it's stored in the file to the return sample
      // type.
      for (size_t i = 0; i < num_samples_to_write; ++i) {
        buffer[i] = ConvertValue<ValueTypeInBuffer, ValueTypeInFile>(
            samples[num_samples_written++]);
      }

      // Write current buffer.
      if (file_writer.Write(buffer.data(), num_bytes_to_write) !=
          num_bytes_to_write) {
        return false;
      }

      num_remaining_samples -= num_samples_to_write;
    }

    return true;
  }

  return false;
}

// Write multiple samples. Will choose the best strategy to use based on the
// current configuration.
//
// Writes the entire samples buffer, without performing any checks about
// validity.
//
// Returns true on success, false otherwise.
template <class ValueTypeInFile, class FileWriter, class ValueTypeInBuffer>
[[nodiscard]] inline auto WriteMultipleSamples(
    FileWriter& file_writer,
    const size_t num_frames,
    const size_t num_channels,
    const std::span<const ValueTypeInBuffer> samples) -> bool {
  static_assert(TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE >= 0);

  if constexpr (TL_AUDIO_WAV_WRITER_BUFFER_SIZE > 1) {
    return WriteMultipleSamplesBuffered<ValueTypeInFile>(
        file_writer, num_frames, num_channels, samples);
  }

  return WriteMultipleSamplesUnbuffered<ValueTypeInFile>(
      file_writer, num_frames, num_channels, samples);
}

}  // namespace internal

template <class FileWriter>
[[nodiscard]] auto Writer<FileWriter>::WritePlaceholderHeader() -> bool {
  assert(is_open_);

  // Specify the chunk size to the max possible value, allowing reader of a
  // partially saved file to read the file until the EOF.
  const uint32_t max_num_samples = MaxNumSamples(format_spec_);
  return internal::WriteHeader(*file_writer_, format_spec_, max_num_samples);
}

template <class FileWriter>
[[nodiscard]] auto Writer<FileWriter>::WriteFinalHeader() -> bool {
  assert(is_open_);

  return internal::WriteHeader(
      *file_writer_, format_spec_, num_samples_written_);
}

template <class FileWriter>
template <class ValueTypeInFile, class ValueTypeInBuffer>
[[nodiscard]] auto Writer<FileWriter>::WriteSingleSample(
    const std::span<const ValueTypeInBuffer> sample) -> bool {
  assert(is_open_);

  // Check for every sample as depending on data structure in the user code it
  // might be not guaranteed that all samples contains the same number of
  // channels.
  const size_t num_values = std::size(sample);
  if (num_values != format_spec_.num_channels) {
    return false;
  }

  // TODO(sergey): Make sure the number of samples fits MaxNumSamples().

  if (!internal::WriteSingleSample<ValueTypeInFile>(*file_writer_, sample)) {
    return false;
  }

  ++num_samples_written_;

  return true;
}

template <class FileWriter>
template <class ValueTypeInFile, class ValueTypeInBuffer>
[[nodiscard]] auto Writer<FileWriter>::WriteMultipleSamples(
    const std::span<const ValueTypeInBuffer> samples) -> bool {
  assert(is_open_);

  const size_t num_channels = format_spec_.num_channels;
  const size_t num_frames = samples.size() / num_channels;

  // Check that the buffer has expected size.
  if (num_frames * num_channels != samples.size()) {
    return false;
  }

  // TODO(sergey): Make sure the number of samples fits MaxNumSamples().

  if (!internal::WriteMultipleSamples<ValueTypeInFile>(
          *file_writer_, num_frames, num_channels, samples)) {
    return false;
  }

  num_samples_written_ += num_frames;

  return true;
}

template <class FileWriter>
template <class ValueTypeInFile, class FileWriterType, class ValueTypeInBuffer>
[[nodiscard]] auto Writer<FileWriter>::Write(
    FileWriterType&& file_writer,
    const FormatSpec& format_spec,
    const std::span<const ValueTypeInBuffer> samples) -> bool {
  Writer<FileWriter> writer;

  if (!writer.Open(file_writer, format_spec)) {
    return false;
  }

  if (!writer.template WriteMultipleSamples<ValueTypeInFile, ValueTypeInBuffer>(
          samples)) {
    return false;
  }

  return writer.Close();
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

}  // namespace TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE
}  // namespace TL_AUDIO_WAV_WRITER_NAMESPACE

#undef TL_AUDIO_WAV_WRITER_VERSION_MAJOR
#undef TL_AUDIO_WAV_WRITER_VERSION_MINOR
#undef TL_AUDIO_WAV_WRITER_VERSION_REVISION

#undef TL_AUDIO_WAV_WRITER_SAMPLE_FRAME_BUFFER_SIZE

#undef TL_AUDIO_WAV_WRITER_NAMESPACE

#undef TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE_CONCAT_HELPER
#undef TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE_CONCAT
#undef TL_AUDIO_WAV_WRITER_VERSION_NAMESPACE
