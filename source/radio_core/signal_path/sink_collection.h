// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// A collection of sinks. Pushing samples to this sink will push samples to
// all the sinks in the collection.
//
// From the API point of view the collection itself is seen as a sink.
//
// This implementation is thread safe which means one thread can be pushing
// samples to the sink while other thread adds or removes sinks.

#pragma once

#include <list>
#include <memory>
#include <mutex>

#include "radio_core/signal_path/sink.h"

namespace radio_core::signal_path {

template <class T, template <class> class Allocator = std::allocator>
class SinkCollection : public Sink<T> {
 public:
  using SampleType = T;

  // Add sink to the collection.
  //
  // The collection references the sink, so caller needs to ensure the lifetime
  // of the sink.
  inline void AddSink(Sink<SampleType>& sink) {
    std::unique_lock lock(mutex_);
    sinks_.push_back(&sink);
  }

  // Remove sink from the collection.
  // After this call the sink will no longer receive samples, and the collection
  // does not reference it.
  // Calling with a sink which is not in the collection is undefined.
  inline void RemoveSink(const Sink<SampleType>& sink) {
    std::unique_lock lock(mutex_);

    std::erase(sinks_, &sink);
  }

  // Push samples to all currently registered sinks.
  void PushSamples(std::span<const SampleType> samples) override {
    std::unique_lock lock(mutex_);

    for (Sink<SampleType>* sink : sinks_) {
      sink->PushSamples(samples);
    }
  }

 private:
  using SinkPtr = Sink<SampleType>*;

  // Mutex to ensure thread safety between sink list manipulation and signal
  // processing thread.
  std::mutex mutex_;

  // Sinks of this collection.
  // Stored in the list to help memory fragmentation on a resource-limited
  // systems, as well as helping removing sinks.
  std::list<SinkPtr, Allocator<SinkPtr>> sinks_;
};

}  // namespace radio_core::signal_path
