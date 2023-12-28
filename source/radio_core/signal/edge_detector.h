// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Detector of a rising and/or falling edge of a pulse.
//
// From implementation perspective this is done by subtracking ver aggressively
// smoothed signal from a cleaner signal. This gives a spike in absolute value
// of the difference on pulse lobes.
//
//   Everyday DSP for Programmers: Edge Detection
//   https://sam-koblenski.blogspot.com/2015/09/everyday-dsp-for-programmers-edge.html
//
// Example use:
//
//   EdgeDetector<float> edge_detector;
//   for (const float sample : samples) {
//     const EdgeDetector<float>::Edge edge = edge_detector(sample);
//     if (edge) {
//       std::cout << "Detected edge" << std::endl;
//     }
//   }

#pragma once

#include "radio_core/math/average.h"

namespace radio_core::signal {

template <class T, bool Rising = true, bool Falling = true>
class EdgeDetector {
 public:
  struct Edge {
    inline operator bool() const { return rising || falling; }

    bool rising{false};
    bool falling{false};
  };

  struct Options {
    T sample_weight{0};
    T slow_sample_weight{0};
    T rising_edge_threshold{0};
    T falling_edge_threshold{0};
  };

  EdgeDetector() = default;

  constexpr explicit EdgeDetector(const Options& options) {
    Configure(options);
  }

  constexpr void Configure(const Options& options) {
    SetSampleWeight(options.sample_weight, options.slow_sample_weight);
    SetRisingEdgeThreshold(options.rising_edge_threshold);
    SetFallingEdgeThreshold(options.falling_edge_threshold);
  }

  constexpr void SetSampleWeight(const T sample_weight,
                                 const T slow_sample_weight) {
    sample_weight_ = sample_weight;
    slow_sample_weight_ = slow_sample_weight;
  }

  constexpr void SetRisingEdgeThreshold(const T threshold) {
    rising_edge_threshold_ = threshold;
  }
  constexpr void SetFallingEdgeThreshold(const T threshold) {
    falling_edge_threshold_ = threshold;
  }

  inline auto operator()(const T sample) -> Edge {
    Edge edge;

    clean_sample_ =
        ExponentialMovingAverage(sample, sample_weight_, clean_sample_);
    slow_clean_sample_ = ExponentialMovingAverage(
        sample, slow_sample_weight_, slow_clean_sample_);

    const T delta = clean_sample_ - slow_clean_sample_;

    // Detection of rising edge.
    if constexpr (Rising) {
      if (delta > rising_edge_threshold_) {
        if (!rising_edge_threshold_passed_) {
          edge.rising = true;
        }
        rising_edge_threshold_passed_ = true;
      } else {
        rising_edge_threshold_passed_ = false;
      }
    }

    // Detection of falling edge.
    if constexpr (Falling) {
      if (delta < -falling_edge_threshold_) {
        if (!falling_edge_threshold_passed_) {
          edge.falling = true;
        }
        falling_edge_threshold_passed_ = true;
      } else {
        falling_edge_threshold_passed_ = false;
      }
    }

    return edge;
  }

 private:
  T sample_weight_{0};
  T slow_sample_weight_{0};

  T clean_sample_{0};
  T slow_clean_sample_{0};

  T rising_edge_threshold_{0};
  bool rising_edge_threshold_passed_{false};

  T falling_edge_threshold_{0};
  bool falling_edge_threshold_passed_{false};
};

}  // namespace radio_core::signal
