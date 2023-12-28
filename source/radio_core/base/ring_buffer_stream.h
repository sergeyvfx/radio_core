// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Stream buffer which solves producer-consumer problem using ring buffer for
// storage and mutex and conditional variables for thread synchronization.
//
// The buffer has a configurable length which does not change when new data is
// written or is read from it.

#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <span>
#include <vector>

namespace radio_core {

template <class T, class Allocator = std::allocator<T>>
class RingBufferStream {
 public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  RingBufferStream() = default;

  ~RingBufferStream() { Close(); }

  // Returns the number of elements that the stream buffer has space for before
  // it overflows.
  inline auto Capacity() const noexcept -> size_type {
    std::unique_lock lock(mutex_);
    return CapacityUnsafe();
  }

  // Get the number of elements stored in the buffer.
  inline auto Size() const noexcept -> size_type {
    std::unique_lock lock(mutex_);
    return SizeUnsafe();
  }

  // Returns true if there are no elements in the buffer.
  inline auto IsEmpty() const noexcept -> bool { return Size() == 0; }

  // Reserve size for the buffer.
  // After this call the stream can hold up to the new_size elements before it
  // overflows.
  //
  // Resizing non-empty stream results in an undefined content of the stream
  // buffer.
  inline void Reserve(const size_type new_size) {
    std::unique_lock lock(mutex_);
    return buffer_.resize(new_size);
  }

  // Write data to the stream.
  //
  // The function does not return until the whole data has been consumed, or the
  // stream is closed. If the data does not fit into the stream buffer the call
  // waits consumer to consume older data.
  auto Write(const std::span<const T> new_data) -> bool {
    const size_type capacity = CapacityUnsafe();

    size_type start_index = 0;
    size_type remaining_size = new_data.size();

    while (remaining_size) {
      std::unique_lock lock(mutex_);

      if (!IsOpen()) {
        return false;
      }

      const size_type remaining_capacity = capacity - SizeUnsafe();
      const size_type chunk_size = std::min(remaining_size, remaining_capacity);

      // The stream buffer is full. Wait for a consumer to consume some data,
      // freeing space for more data to be written.
      if (!chunk_size) {
        writer_condition_variable_.wait(lock);

        // Enforce the new iteration, to obtain the new stream capacity.
        continue;
      }

      WriteWrapped(new_data.subspan(start_index, chunk_size));
      reader_condition_variable_.notify_one();

      start_index += chunk_size;
      remaining_size -= chunk_size;
    }

    return true;
  }

  // Write elements to the stream until the buffer is full or the whole data is
  // written.
  // Returns the number of elements written to the stream
  auto WriteNonblocking(const std::span<const T> new_data) -> size_type {
    std::unique_lock lock(mutex_);

    if (!IsOpen()) {
      return 0;
    }

    const size_type remaining_capacity = CapacityUnsafe() - SizeUnsafe();
    const size_type write_size = std::min(new_data.size(), remaining_capacity);

    if (write_size) {
      WriteWrapped(new_data.subspan(0, write_size));
    }

    reader_condition_variable_.notify_one();

    return write_size;
  }

  // Read data from the stream.
  //
  // The call does not return until the given buffer is fully filled or the
  // stream is closed. If there is no enough data in the stream at this point,
  // the function waits the producer to provide more data.
  auto Read(const std::span<T> data) -> bool {
    size_type start_index = 0;
    size_type remaining_size = data.size();

    while (remaining_size) {
      std::unique_lock lock(mutex_);

      if (!IsOpen()) {
        return false;
      }

      const size_type stream_size = SizeUnsafe();
      const size_type chunk_size = std::min(remaining_size, stream_size);

      if (!chunk_size) {
        reader_condition_variable_.wait(lock);

        // Enforce the new iteration, to obtain the new stream capacity.
        continue;
      }

      ReadWrapped(data.subspan(start_index, chunk_size));
      writer_condition_variable_.notify_one();

      start_index += chunk_size;
      remaining_size -= chunk_size;
    }

    return true;
  }

  // Read available data from the stream.
  //
  // At maximum the size of the given buffer is read. If there is no enough data
  // in the stream the buffer is filled partially.
  //
  // When only partial read is performed the elements in the buffer past the
  // read block are left unchanged.
  //
  // Returns the number of elements actually read from the stream.
  auto ReadNonblocking(const std::span<T> data) -> size_type {
    std::unique_lock lock(mutex_);

    if (!IsOpen()) {
      return 0;
    }

    const size_type stream_size = SizeUnsafe();
    const size_type read_size = std::min(data.size(), stream_size);

    if (read_size) {
      ReadWrapped(data.subspan(0, read_size));
      writer_condition_variable_.notify_one();
    }

    return read_size;
  }

  // Close the stream.
  // Inform all pending readers and writers that the stream is no longer
  // accepting or providing data.
  inline void Close() {
    {
      std::unique_lock lock(mutex_);
      is_open_ = false;
    }
    reader_condition_variable_.notify_one();
    writer_condition_variable_.notify_one();
  }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // Internal implementation and common utilities.

  // Returns true if the stream is open to receive and provide data.
  inline auto IsOpen() const -> bool { return is_open_; }

  // Returns the number of elements that the stream buffer has space for before
  // it overflows, without thread safety guard.
  inline constexpr auto CapacityUnsafe() const -> size_type {
    return buffer_.size();
  }

  // Get the number of elements stored in the buffer, without thread safety
  // guard.
  inline constexpr auto SizeUnsafe() const -> size_type { return size_; }

  // Advance the index by the given value, wrapping it to 0 if the index reaches
  // capacity.
  inline auto AdvanceWarped(size_type& index, const size_type advance) const {
    index += advance;
    if (index >= CapacityUnsafe()) {
      index = 0;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Internal write implementation.

  // Write the new data to the end of the buffer, starting from the current
  // write index and wrapping around the end of the storage when needed.
  //
  // Assumes that there is enough capacity in the buffer.
  inline void WriteWrapped(std::span<const T> new_data) {
    assert(new_data.size() <= CapacityUnsafe());

    const size_type new_data_size = new_data.size();

    // Copy new data to the current buffer position up until the end of the
    // buffer.
    const size_type head_size =
        std::min(new_data_size, CapacityUnsafe() - write_index_);
    WriteContinuous(new_data.subspan(0, head_size));

    // If there is still data to be written, write it from the beginning of the
    // buffer.
    if (head_size != new_data_size) {
      const size_type tail_size = new_data_size - head_size;
      WriteContinuous(new_data.subspan(head_size, tail_size));
    }
  }

  // Write the new data to the end of the buffer, starting from the current
  // write index. Performs the copy in a single memory copy call, without doing
  // memory wrap.
  //
  // Advances the write position in the stream and updates the stream size.
  inline void WriteContinuous(std::span<const T> new_data) {
    const size_type new_data_size = new_data.size();

    std::memcpy(&buffer_[write_index_], new_data.data(), new_data.size_bytes());

    AdvanceWarped(write_index_, new_data_size);
    size_ += new_data_size;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Internal read implementation.

  // Read data from the buffer, starting from the current read index and
  // wrapping around the end of the storage when needed.
  //
  // Assumes that the requested amount of data is less or equal to the amount of
  // data in the buffer.
  inline void ReadWrapped(std::span<T> data) {
    assert(data.size() <= SizeUnsafe());

    const size_type data_size = data.size();

    const size_type head_size =
        std::min(data_size, CapacityUnsafe() - read_index_);
    ReadContinuous(data.subspan(0, head_size));

    // If there is still data to be read, read it from the beginning of the
    // buffer.
    if (head_size != data_size) {
      const size_type tail_size = data_size - head_size;
      ReadContinuous(data.subspan(head_size, tail_size));
    }
  }

  // Read data to the buffer starting from the current read index.
  // Performs the copy in a single memory copy call, without doing memory wrap.
  // Advances the read position in the stream and updates the stream size.
  inline void ReadContinuous(std::span<T> data) {
    const size_type data_size = data.size();

    std::memcpy(data.data(), &buffer_[read_index_], data.size_bytes());

    AdvanceWarped(read_index_, data_size);
    size_ -= data_size;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // True if the stream is open to accept and provide data.
  // Use atomic to ensue the value is properly propagated to both producer and
  // consumer threads.
  std::atomic<bool> is_open_{true};

  // Thread synchronization.
  mutable std::mutex mutex_;
  std::condition_variable reader_condition_variable_;
  std::condition_variable writer_condition_variable_;

  // Storage of the data in this stream.
  std::vector<T, Allocator> buffer_;

  // The number of elements in the buffer.
  size_type size_{0};

  // Index in the buffer at which first element to be written or read is
  // located.
  size_type read_index_{0};
  size_type write_index_{0};
};

}  // namespace radio_core
