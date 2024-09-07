// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Processor which takes care of decoding horizontal line synchronization.
//
// The input is filtered frequency samples.

#pragma once

#include <functional>

#include "radio_core/base/frequency.h"
#include "radio_core/base/interval.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/math/time.h"
#include "radio_core/picture/sstv/mode_spec.h"
#include "radio_core/signal/edge_detector.h"

namespace radio_core::picture::sstv {

template <class RealType>
class LineSync {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate;

    // Mode of the picture encoding.
    Mode mode{Mode::kUnknown};
  };

  inline LineSync() { Reset(); }

  inline void Configure(const Options& options) {
    const RealType sample_rate = options.sample_rate;

    const ModeSpec<RealType> mode_spec = ModeSpec<RealType>::Get(options.mode);

    // Edge detector.
    edge_detector_.SetSampleWeight(kEdgeDetectorSampleWeight,
                                   kEdgeDetectorSampleSlowWeight);
    edge_detector_.SetRisingEdgeThreshold(kDetectorRisingThreshold);
    edge_detector_.SetFallingEdgeThreshold(kDetectorFallingThreshold);

    // Line synchronization.
    line_sync_freq_interval_ =
        Interval<Frequency>(mode_spec.line_sync.frequency)
            .Expanded(kFrequencyTolerance);

    num_expected_sync_samples_ = MillisecondsToNumSamples(
        sample_rate,
        mode_spec.line_sync.duration_ms - kTransitionInMilliseconds);
  }

  // Reset the line synchronization machine to its initial state.
  inline void Reset() { state_ = State::kWaitForTone; }

  // Process given frequency sample.
  //
  // Invokes the given callback with the given arguments when line
  // synchronization is detected.
  template <class F, class... Args>
  inline void operator()(const RealType frequency,
                         F&& callback,
                         Args&&... args) {
    // Always feed samples to the edge detector, so that it can properly track
    // average values and other internal state.
    detected_edge_ = edge_detector_(frequency);

    switch (state_) {
      case State::kWaitForTone: return HandleWaitForTone(frequency);
      case State::kSampleTone: return HandleSyncSampleTone(frequency);
      case State::kWaitForEdge:
        return HandleWaitForEdge(
            frequency, std::forward<F>(callback), std::forward<Args>(args)...);
    }

    Unreachable();
  }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // Configuration.

  // Tolerance of the pattern matching.
  inline static constexpr RealType kFrequencyTolerance{50};
  inline static constexpr RealType kTransitionInMilliseconds{4};

  inline static constexpr RealType kEdgeDetectorSampleWeight{0.25};
  inline static constexpr RealType kEdgeDetectorSampleSlowWeight{0.025};

  // Use lower threshold for trailing edge so that transition from start VIS
  // bit to VIS code is detected and the line synchronization is cancelled.
  inline static constexpr RealType kDetectorRisingThreshold{80};
  inline static constexpr RealType kDetectorFallingThreshold{30};

  //////////////////////////////////////////////////////////////////////////////
  // State machine.

  // Horizontal line synchronization.
  // The state starts with waiting for the line synchronization tone, which
  // then is followed with time-based match to verify the sync lasted long
  // enough, and finally is followed by an edge detector to know where the
  // actual line tone begins.

  inline void HandleWaitForTone(const RealType frequency) {
    if (line_sync_freq_interval_.Contains(frequency)) {
      SwitchToSampleSyncTone();
    }
  }

  // Sample line synchronization tone, checking its duration and frequency
  // range.

  inline void SwitchToSampleSyncTone() {
    state_ = State::kSampleTone;
    num_samples_in_state_ = 0;
  }

  inline void HandleSyncSampleTone(const RealType frequency) {
    if (!line_sync_freq_interval_.Contains(frequency)) {
      return Reset();
    }

    // Wait for the expected synch tone duration.
    //
    // NOTE: A saturated add is used to avoid integer overflow on a signals
    // which consists of a synchronization frequency.
    if (num_samples_in_state_ < num_expected_sync_samples_) {
      ++num_samples_in_state_;
      return;
    }

    // The synchronization lasted long enough to comply to the specification,
    // so now need to look for an edge which will denote beginning of a line.
    //
    // NOTE: There is no upper limit to the synchronization pulse. This way it
    // is possible to handle synchronization if for some reason VIS was not
    // detected and a longer synchronization tone is seen by this decoder (the
    // VIS stop bit uses same frequency as the line synchronization).
    SwitchToWaitForLineEdge();
  }

  // Wait for an edge from line synchronization to the scanline pixels.
  // This is a raising edge from lower synchronization frequency to a higher
  // black level frequency.

  inline void SwitchToWaitForLineEdge() { state_ = State::kWaitForEdge; }

  template <class F, class... Args>
  inline void HandleWaitForEdge(const RealType /*frequency*/,
                                F&& callback,
                                Args&&... args) {
    if (detected_edge_.falling) {
      // Trailing edge is met when the line VIS start bit is confused with the
      // line synchronization. In this case one of the data bits will have a
      // trailing edge. If this happens cancel the line synchronization
      // sequence as we are in the middle of VIS.
      return Reset();
    }

    if (detected_edge_.rising) {
      std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

      // Line is synchronized. Reset the state waiting for a synchronization
      // of the next line.
      Reset();
    }

    // TODO(sergey): Reset the state if the wait happens too long (longer than
    // stop bit plus line sync duration), or when there are too much noise or
    // when the tone is near black luma value (which would mean an edge was
    // missed).
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Range of frequencies which count as a line synchronization tone.
  Interval<Frequency> line_sync_freq_interval_;

  // Duration of line synchronization tome in samples.
  int num_expected_sync_samples_;

  // Current state of the machine.
  enum class State {
    kWaitForTone,
    kSampleTone,
    kWaitForEdge,
  };
  State state_;

  // Number of samples handled by the current state.
  // NOTE: Only for states which needs to track the number of samples.
  int num_samples_in_state_;

  using EdgeDetector = signal::EdgeDetector<RealType, true, true>;
  EdgeDetector edge_detector_;
  typename EdgeDetector::Edge detected_edge_;
};

}  // namespace radio_core::picture::sstv
