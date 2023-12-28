// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

namespace radio_core::signal {

// For the given filter size (number of taps) return its delay in samples.
//
// The math comes from the symmetry of the filter, but it could also be
// visualized with the following Python code:
//
//   import math
//   import matplotlib.pyplot as plt
//   from scipy import signal
//
//   kNumTaps = 31
//   kDelay = 15
//
//   h = signal.firwin(kNumTaps, 4000, window="hamming", fs=44100)
//
//   input = []
//   for i in range(128):
//       input.append(math.sin(float(i) * 2 * math.pi * 1000 / 44100))
//
//   filtered = signal.lfilter(h, 1.0, input)
//
//   delayed = [0] * kDelay + input[:-kDelay]
//
//   plt.figure
//   plt.plot(filtered, label="filtered")
//   plt.plot(delayed, label="delayed")
//   plt.legend()
//   plt.grid(True)
//   plt.show()
template <class SizeType>
constexpr inline auto FilterDelayInSamples(const SizeType filter_size)
    -> SizeType {
  if (filter_size == 0) {
    return 0;
  }
  return (filter_size - 1) / 2;
}

// For a kernel size (number of taps) optimized for a specific sample rate
// estimate best filter size for the given sample rate.
// Preserves oddity of the configured filter size.
template <class RealType, class SizeType>
constexpr inline auto ConvertFilterSize(const RealType configured_sample_rate,
                                        const SizeType configured_filter_size,
                                        const RealType effective_sample_rate)
    -> SizeType {
  if (configured_sample_rate == effective_sample_rate) {
    return configured_filter_size;
  }

  const SizeType even_configured_filter_size = configured_filter_size & ~1;
  const RealType scale = effective_sample_rate / configured_sample_rate;

  if (configured_filter_size & 1) {
    return SizeType(even_configured_filter_size * scale + RealType(0.5)) | 1;
  }
  return SizeType(even_configured_filter_size * scale + RealType(0.5));
}

}  // namespace radio_core::signal
