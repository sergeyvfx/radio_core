// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Sink is a black box input of data, abstracting a way to provide input data to
// a signal processing path.

#pragma once

#include <span>

namespace radio_core::signal_path {

template <class T>
class Sink {
 public:
  using SampleType = T;

  Sink() = default;
  virtual ~Sink() = default;

  // Push multiple samples to the sink.
  virtual void PushSamples(std::span<const SampleType> samples) = 0;
};

}  // namespace radio_core::signal_path
