// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Asynchronous sink using threading implementation from the std.
//
// Implements non-blocking pushSamples method and a way to wait for processing
// to finish.
//
// NOTE: The input samples are to be valid during the processing.

#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

#include "radio_core/signal_path/sink.h"

namespace radio_core::signal_path {

template <class T>
class AsyncSink : public Sink<T> {
 public:
  using SampleType = T;

  AsyncSink() : thread_([&]() { Run(); }) {}

  ~AsyncSink() override { StopAndWait(); }

  void PushSamples(std::span<const SampleType> samples) override {
    {
      std::unique_lock lock(mutex_);

      // TODO(sergey): Verify there is no collision in the input data.

      samples_ = samples;
      has_samples_to_process_ = true;
    }

    input_condition_variable_.notify_one();
  }

  virtual void Wait() {
    std::unique_lock lock(mutex_);
    output_condition_variable_.wait(lock, [&]() -> bool {
      return stop_requested_ || !has_samples_to_process_;
    });
  }

  // Signal worker thread to stop and wait for it to finish.
  void StopAndWait() {
    {
      std::unique_lock lock(mutex_);
      if (stop_requested_) {
        return;
      }
      stop_requested_ = true;
    }
    input_condition_variable_.notify_one();
    output_condition_variable_.notify_one();

    thread_.join();
  }

 private:
  // Work thread callback.
  void Run() {
    ConfigureThread();

    while (true) {
      {
        std::unique_lock lock(mutex_);
        input_condition_variable_.wait(lock, [&]() -> bool {
          return stop_requested_ || has_samples_to_process_;
        });

        if (stop_requested_) {
          break;
        }
      }

      ProcessSamples(samples_);
      samples_ = {};

      {
        std::unique_lock lock(mutex_);
        has_samples_to_process_ = false;
      }

      output_condition_variable_.notify_one();
    }
  }

  // The function is called from the thread before starting loop which handles
  // samples processing.
  // Allows subclasses to define thread affinity and priority.
  virtual void ConfigureThread() {}

  virtual void ProcessSamples(std::span<const SampleType> samples) = 0;

  // Primitives to ensure thread-safety and communication between caller and
  // worker threads.
  std::mutex mutex_;
  std::condition_variable input_condition_variable_;
  std::condition_variable output_condition_variable_;

  std::thread thread_;

  // Samples to be processed.
  //
  // Set in the pushSamples and accessed from the work thread.
  std::span<const SampleType> samples_;

  // There are new samples to be processed.
  //
  // Worker thread uses it as a signal to wake up and start processing.
  // Radio thread uses this to verify data processing happens quick enough.
  bool has_samples_to_process_ = false;

  // The work thread should stop processing and finish.
  bool stop_requested_ = false;
};

}  // namespace radio_core::signal_path
