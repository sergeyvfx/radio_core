// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// An implementation of signal path processor which is the simplest one for use
// in typical C++ application on a desktop (or any platform which supports
// std::mutex).
//
// This signal path allows processing of samples in one thread and
// re-configuration from another thread. A mutex lock from std library is used
// to ensure thread-safety.

#pragma once

#include <mutex>

#include "radio_core/signal_path/base_signal_path.h"

namespace radio_core::signal_path {

template <class T, template <class> class Allocator = std::allocator>
class SimpleSignalPath : public BaseSignalPath<T, Allocator> {
 protected:
  void Lock() override { mutex_.lock(); }
  void Unlock() override { mutex_.unlock(); }

 private:
  // Lock which ensures thread safety of configuration of processing threads.
  std::mutex mutex_;
};

}  // namespace radio_core::signal_path
