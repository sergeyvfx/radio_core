// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// A wrapper which implements Sink API and passes samples processing to the
// given object method.
//
// Example:
//
//   class MyProcessor {
//    public:
//     void ProcessSamples(const std::span<const float> samples) {
//       ..
//     }
//   };
//
//   auto sink = SinkMethodWrapper<float,
//                                 MyProcessor,
//                                 decltype(&MyProcessor::ProcessSamples)>(
//       &processor, &MyProcessor::ProcessSamples);
//
// TODO(sergey): Find less verbose way of creating the sink.

#pragma once

#include "radio_core/signal_path/sink.h"

namespace radio_core::signal_path {

template <class T, class ClassType, class MethodType>
class SinkMethodWrapper : public radio_core::signal_path::Sink<T> {
 public:
  SinkMethodWrapper(ClassType* object, MethodType method)
      : object_(object), method_(method) {}

  void PushSamples(const std::span<const T> samples) override {
    (object_->*method_)(samples);
  }

 private:
  ClassType* object_{nullptr};
  MethodType method_;
};

}  // namespace radio_core::signal_path
