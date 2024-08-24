// Copyright (c) 2024 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <span>

namespace radio_core {

namespace resample_internal {

template <class T, class Reduction, class Callback>
void ForeachDownsampledValue(const std::span<const T> samples,
                             const size_t num_output_samples,
                             Reduction reduction,
                             Callback callback) {
  const size_t num_input_samples = samples.size();
  const float downsample_ratio = float(num_input_samples) / num_output_samples;

  size_t prev_index = 0;
  for (size_t i = 0; i < num_output_samples - 1; ++i) {
    const size_t next_index = float(i + 1) * downsample_ratio;
    const T output_sample =
        reduction(samples.subspan(prev_index, next_index - prev_index));
    callback(output_sample);
    prev_index = next_index;
  }

  const size_t next_index = num_input_samples;
  const T output_sample =
      reduction(samples.subspan(prev_index, next_index - prev_index));
  callback(output_sample);
}

template <class T, class Reduction, class Callback>
void ForeachUpsampledValue(const std::span<const T> samples,
                           const size_t num_output_samples,
                           Callback callback) {
  const size_t num_input_samples = samples.size();
  const float upsample_ratio_inv =
      float(num_input_samples) / num_output_samples;

  for (size_t i = 0; i < num_output_samples; ++i) {
    const size_t orig_i = i * upsample_ratio_inv;
    callback(samples[orig_i]);
  }
}

}  // namespace resample_internal

// A simple resampling of input samples to the given number of output samples.
//
// When downsampling the given reduction functor is called to combine multiple
// samples together.
// When upsampling the input samples are repeated multiple times.
//
// For every output sample the given callback is invoked.
template <class T, class Reduction, class Callback>
void ForeachResampledValue(const std::span<const T> samples,
                           const size_t num_output_samples,
                           Reduction reduction,
                           Callback callback) {
  if (samples.empty() || num_output_samples == 0) {
    return;
  }

  // Simple case: no actual resmapling happens.
  if (samples.size() == num_output_samples) {
    for (const T& sample : samples) {
      callback(sample);
    }
    return;
  }

  if (samples.size() >= num_output_samples) {
    resample_internal::ForeachDownsampledValue<T, Reduction, Callback>(
        samples, num_output_samples, reduction, callback);
    return;
  }

  resample_internal::ForeachUpsampledValue<T, Reduction, Callback>(
      samples, num_output_samples, callback);
}

}  // namespace radio_core
