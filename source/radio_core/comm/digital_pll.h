// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// PLL used to sample stream of digital data which clock is unknown.
//
// Sampling happens in the middle of bit, re-alignment of PLL to the bit edge
// happens every time bit value changes.

#pragma once

#include <cassert>
#include <cstdint>

#include "radio_core/math/math.h"

namespace radio_core::comm {

template <class RealType>
class DigitalPLL {
 public:
  struct Options {
    // The baud of the signal for which clock is being recovered.
    int data_baud{0};

    // The rate of the signal samples provided to the `pushSample()`.
    RealType sample_rate{0};

    // Inertia defines how quickly the PLL locks to signal on its transitions.
    // Higher values mean slower but less jittery response, lower values mean
    // faster response but possibly more jittery clock.
    //
    // The value is expected to be within [0 .. 1] range.
    RealType inertia{0};
  };

  DigitalPLL() = default;

  explicit DigitalPLL(const Options& options) { Configure(options); }

  void Configure(const Options& options) {
    assert(options.inertia >= 0.0f);
    assert(options.inertia <= 1.0f);

    inertia_ = options.inertia;

    lo_counter_advance_per_input_sample_ =
        RoundToInt(RealType(kTicksPerLOPeriod) *
                   (RealType(options.data_baud) / options.sample_rate));
  }

  void Reset() {
    lo_counter_ = 0;
    previous_signal_sample_ = 0;
  }

  // Returns true if the input signal is to be sampled at the current time to
  // get proper bit value.
  auto operator()(const int signal_sample) -> bool {
    assert(lo_counter_advance_per_input_sample_ != 0);

    bool need_sample_data = false;

    const int32_t prev_local_oscillator_counter = lo_counter_;
    lo_counter_ += lo_counter_advance_per_input_sample_;

    // Sample when overflow from positive to negative happens.
    //
    // Assuming that the PLL is perfectly locked to the signal the overflow will
    // happen in the middle of the bit. This is because of signed nature of the
    // local oscillator counter and the sampling frequency configured by the
    // `setDataRate()`.
    if (lo_counter_ < prev_local_oscillator_counter) {
      // If the oscillator value is less than previous an overflow occurred.
      // Sample data bit at this point of time.
      need_sample_data = true;
    }

    if (signal_sample != previous_signal_sample_) {
      // Multiply signal by inertia, bringing it closer to 0 (which is the sign
      // transition point).
      //
      // TODO(sergey): Implement different inertia for locked and non-locked
      // states, to make it so the PLL quickly locks to a new signal but gives
      // more stable clock once it is locked to signal.
      lo_counter_ = lo_counter_ * inertia_;
      previous_signal_sample_ = signal_sample;
    }

    return need_sample_data;
  }

 private:
  // Local oscillator (LO) is simulated as a fixed-bit counter.
  // This value is the number of counter ticks per LO period.
  static constexpr uint32_t kTicksPerLOPeriod{0xffffffff};

  RealType inertia_{0.5};

  // Current value of the local oscillator.
  int32_t lo_counter_{0};

  // How much the local oscillator is to advance per input sample.
  uint32_t lo_counter_advance_per_input_sample_{0};

  // Value of the previous signal sample.
  int previous_signal_sample_{0};
};

}  // namespace radio_core::comm
