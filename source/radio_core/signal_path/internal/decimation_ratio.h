// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Utilities to calculate decimation ratio between different stages of the
// signal processing path.

#pragma once

#include <ostream>

#include "radio_core/base/verify.h"

namespace radio_core::signal_path::internal {

template <class T>
class StagesDecimation {
 public:
  // Information about decimation ratio between various stages of the signal
  // processing path.
  class Ratio {
   public:
    Ratio() = default;

    Ratio(const int new_iq_to_if, const int new_if_to_af)
        : iq_to_if(new_iq_to_if), if_to_af(new_if_to_af) {}

    constexpr inline auto operator==(const Ratio& other) const -> bool {
      return iq_to_if == other.iq_to_if && if_to_af == other.if_to_af;
    }
    constexpr inline auto operator!=(const Ratio& other) const -> bool {
      return !(*this == other);
    }

    friend auto operator<<(std::ostream& os,
                           const Ratio& ratio) -> std::ostream& {
      os << "IQ to IF: " << ratio.iq_to_if << ", IF to AF: " << ratio.if_to_af;
      return os;
    }

    // Decimation ratio from the input IQ sample rate to an intermediate
    // frequency (IF) stage sample rate. At this stage the receive filter is
    // applied, and the demodulation is performed.
    int iq_to_if{1};

    // Decimation ratio from the IF to the audio frequency (AF) stage.
    // At this stage audio processing performed, and this is also the audio
    // output sample rate of the signal processing path.
    int if_to_af{1};
  };

  // Calculate decimation ratio for the given signal path configuration:
  //
  //   - iq_sample_rate is the sample rate of the input IQ samples.
  //
  //   - af_sample_rate is the sample rate of the audio output of the path.
  //
  //   - receive_filter_bandwidth is the bandwidth of the receive filter which
  //     is applied at the IF stage prior to demodulation.
  //
  // The factors are such that they minimize the sample rate at the IF stage,
  // so that the receive filter is applied as computationally efficient as
  // possible.
  //
  // NOTE: Only integer decimation factors are suported.
  static inline auto Calculate(const int iq_sample_rate,
                               const int af_sample_rate,
                               const T receive_filter_bandwidth) -> Ratio {
    // TODO(sergey): Support rational factor downsampling.
    VERIFY(iq_sample_rate % af_sample_rate == 0);

    const int iq_to_af_ratio = iq_sample_rate / af_sample_rate;

    // The receive filter is a low-pass filter with a cut-off frequency equal to
    // the half of its bandwidth since the band is centered around the DC, hence
    // can optimize out division by 2 on both sides.
    if (receive_filter_bandwidth < af_sample_rate) {
      // The audio sample rate is enough to give good results for the filter.
      // Don't go below this sample rate: if needed the receive filter will use
      // down and up conversion if the bandwidth is very narrow.
      //
      // Not going below the audio sample rate allows demodulators like CW to
      // have enough space for the frequency shift.
      return {iq_to_af_ratio, 1};
    }

    // Iterate over possible divisors and find the lowest IF sample rate which
    // satisfies the receive filter bandwidth.
    for (int if_to_af_ratio = 2; if_to_af_ratio <= iq_to_af_ratio;
         ++if_to_af_ratio) {
      const int if_sample_rate = af_sample_rate * if_to_af_ratio;

      if (if_sample_rate < receive_filter_bandwidth) {
        // Sample rate is too small for the bandwidth of the filter.
        continue;
      }

      if (iq_sample_rate % if_sample_rate) {
        // Non-integer ratio, which is not currently supported.
        continue;
      }

      return {iq_sample_rate / if_sample_rate, if_to_af_ratio};
    }

    // The algorithm was unable to find mid-point for the IF, so ise the input
    // sample rate as the intermediate frequency, and downsample it to audio
    // after demodulation.
    return {1, iq_to_af_ratio};
  }
};

}  // namespace radio_core::signal_path::internal
