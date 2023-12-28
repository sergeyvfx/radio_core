// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <array>
#include <cassert>

#include "radio_core/math/math.h"

namespace radio_core {

// Accurate-ish moving average calculator.
//
// Stores last kNumSamples samples to calculate average. For the performance
// reasons sum of elements is updated when new samples are pushed. This could
// lead to precision issues when samples are too big, or when samples has too
// different magnitude.
template <class T, size_t kNumSamples>
class MovingAverage {
 public:
  MovingAverage() = default;

  inline void PushSample(const T& sample) {
    // Append new sample to the sum.
    sum_ += sample;

    if (num_samples_ < kNumSamples) {
      ++num_samples_;
    } else {
      // Reduce sum by the element which will be overwritten.
      sum_ -= samples_[tail_index_];
    }

    // Write sample to the storage.
    assert(tail_index_ < kNumSamples);
    samples_[tail_index_] = sample;

    // Move tail and wrap it if needed.
    //
    // TODO(sergey): Investigate if it is branching or module leads to better
    // performance.
    //
    // TODO(sergey): Optimize out by using bit magic when the number of samples
    // is a power of two.
    ++tail_index_;
    if (tail_index_ >= kNumSamples) {
      tail_index_ = 0;
    }
  }

  // Get current average.
  // If there were no samples provided yet T(0) is returned.
  inline auto Get() const -> T {
    if (num_samples_ == 0) {
      return T(0);
    }
    return sum_ / num_samples_;
  }

  // Get the number of samples which is used for the average calculation.
  inline auto GetNumSamples() const -> size_t { return num_samples_; }

  // Process sample and return an updated average.
  inline auto operator()(const T& sample) -> T {
    PushSample(sample);
    return Get();
  }

 private:
  std::array<T, kNumSamples> samples_;
  size_t num_samples_ = 0;

  // Index at which next sample will be written.
  size_t tail_index_ = 0;

  // Pre-calculated sum of elements in the samples array.
  T sum_ = 0;
};

// Simplified version of an exponential moving average.
//
// Uses single coefficient and only requires to store a single value which is
// an averaged result. The difference from the ground-truth implementation is
// that the averaging is always done, regardless of the time.
//
// It is possible to achieve the ground truth behavior if the caller will take
// care of initializing the average value to the sample for the first time
// sample.
//
// Example usage snippet:
//
//   const float average = 0.0f;
//   for (...) {
//     const float sample = calculateNewSample();
//     average = ExponentialMovingAverage(sample, 0.75f, average);
//     std::cout << "Smoothed signal sample: " << average << std::endl;
//   }
//
// The weight from the number of samples can be estimates as `2 / (n + 1)`.
// See the Relationship between SMA and EMA section of the
//   https://en.wikipedia.org/wiki/Moving_average
//
// References:
//   Moving average, exponential moving average
//   https://wikipedia.org/wiki/Moving_average#Exponential_moving_average
template <class T>
inline auto ExponentialMovingAverage(const T sample,
                                     const T sample_weight,
                                     const T average) -> T {
  assert(sample_weight >= 0);
  assert(sample_weight <= 1);

  // More accurate form:
  //   RESULT = sample * sample_weight + (T(1) - sample_weight) * average;

  return Lerp(average, sample, sample_weight);
}

}  // namespace radio_core
