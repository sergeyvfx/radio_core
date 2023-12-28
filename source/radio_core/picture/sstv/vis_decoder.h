// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of VIS signal.
//
// Handles frequency samples (often are generated by sstv::Prefilter) and
// produces output result which contains VIS code.
//
// General idea of this decoder is to perform a "pattern" matching of
// frequencies of the input signal with VIS tones, with some tolerance in both
// frequency and timing domain.
//
// The decoder will apply a prefilter on the input frequency samples which helps
// it to detect VIS in a signal with low SNR. The motivation behind this is to
// allow locking to a noisy signal and recover as much information as possible.
//
// The internal network:
//
//   ┌╌╌╌╌╌╌╌┐   ┌───────────╖   ┌──────────────────────────╖   ┌╌╌╌╌╌╌╌╌┐
//   ┆ Input ┆ → │ Prefilter ║ → │ Pattern matching machine ║ → ┆ Output ┆
//   └╌╌╌╌╌╌╌┘   ╘═══════════╝   ╘══════════════════════════╝   └╌╌╌╌╌╌╌╌┘

#pragma once

#include <memory>

#include "radio_core/base/frequency.h"
#include "radio_core/base/interval.h"
#include "radio_core/base/result.h"
#include "radio_core/base/unreachable.h"
#include "radio_core/math/average.h"
#include "radio_core/math/time.h"
#include "radio_core/picture/sstv/vis.h"
#include "radio_core/signal/digital_pll.h"
#include "radio_core/signal/edge_detector.h"
#include "radio_core/signal/filter.h"
#include "radio_core/signal/filter_design.h"
#include "radio_core/signal/filter_window_heuristic.h"
#include "radio_core/signal/simple_fir_filter.h"
#include "radio_core/signal/window.h"

namespace radio_core::picture::sstv {

template <class RealType, template <class> class Allocator = std::allocator>
class VISDecoder {
 public:
  struct Options {
    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate;

    // Fine-tuned parameters.
    //
    // They are obtained empirically by tweaking kernel sized and optimizing
    // for the number of decoded VIS digital codes.
    //
    // TODO(sergey): Look into making a more well-defined way to check quality
    // of the decoder.

    // Parameters of the input frequency low-pass prefilter.
    // This is a low-pass filter.
    RealType prefilter_transition_bandwidth{280};
    RealType prefilter_frequency_cutoff{300};

    // Tolerance of the pattern matching: maximum deviation in frequency and
    // time of transition edge.
    RealType frequency_tolerance{50};
    RealType transition_in_milliseconds{4};

    // Inertia of a PLL which locks to the VIS tones edges.
    RealType pll_inertia{0.75};

    // Parameters of edge detector.
    // This is a falling edge detector used to lock into the end of second
    // leader tone to help with timing recovery.
    RealType edge_sample_weight{0.25};
    RealType edge_sample_slow_weight{0.025};
    RealType edge_threshold{180};
  };

  enum class Error {
    // Given data has been processed but the VIS code is not complete yet and
    // hence is not available for access.
    //
    // This code does not indicate a processing or protocol error, it just
    // indicates that more data is needed to fully decode VIS from the
    // transmission.
    kUnavailable,
  };

  // Result of the signal processor.
  // The value is the VIS code.
  using Result = radio_core::Result<uint8_t, Error>;

  explicit VISDecoder() { Reset(); }

  inline void Configure(const Options& options) {
    ConfigurePrefilter(options);
    ConfigureMatchingTolerances(options);
    ConfigureDataSampler(options);
    ConfigureEdgeDetector(options);
  }

  auto operator()(const RealType sample) -> Result {
    const RealType clean_sample = prefilter_(sample);
    return PushFrequencySampleToMachine(clean_sample);
  }

  // Get delay of this decoder in milliseconds.
  //
  // The delay is measured from the last sample of VIS was pushed to this
  // decoder and decoder triggered the VIS synchronization.
  inline int GetDelayInMilliseconds() const { return delay_in_ms_; }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // Configuration.

  inline void ConfigurePrefilter(const Options& options) {
    const int prefilter_num_taps =
        signal::EstimateFilterSizeForTransitionBandwidth(
            options.prefilter_transition_bandwidth, options.sample_rate) |
        1;

    prefilter_.SetKernelSize(prefilter_num_taps);

    const int delay_in_samples =
        signal::FilterDelayInSamples(prefilter_.GetKernelSize());
    delay_in_ms_ = NumSamplesToMilliseconds<RealType>(options.sample_rate,
                                                      delay_in_samples);

    signal::DesignLowPassFilter(
        prefilter_.GetKernel(),
        signal::WindowEquation<RealType, signal::Window::kHamming>(),
        options.prefilter_frequency_cutoff,
        options.sample_rate);
  }

  inline void ConfigureMatchingTolerances(const Options& options) {
    using VISTones = sstv::VISTones<RealType>;

    const RealType sample_rate = options.sample_rate;
    const RealType frequency_tolerance = options.frequency_tolerance;
    const RealType transition_in_ms = options.transition_in_milliseconds;

    leader_tone_freq_interval_ =
        Interval<Frequency>(VISTones::kLeaderTone.frequency)
            .Expanded(frequency_tolerance);
    break_tone_freq_interval_ =
        Interval<Frequency>(VISTones::kBreakTone.frequency)
            .Expanded(frequency_tolerance);

    start_bit_freq_interval_ =
        Interval<Frequency>(VISTones::kStartBit.frequency)
            .Expanded(frequency_tolerance);
    bit_value_zero_freq_interval_ =
        Interval<Frequency>(VISTones::kBitValueZero.frequency)
            .Expanded(frequency_tolerance);
    bit_value_one_freq_interval_ =
        Interval<Frequency>(VISTones::kBitValueOne.frequency)
            .Expanded(frequency_tolerance);
    stop_bit_freq_interval_ = Interval<Frequency>(VISTones::kStopBit.frequency)
                                  .Expanded(frequency_tolerance);

    // Duration of leader and break tones.
    // Worst case estimate: assume the transition eats the tone from both sides
    // making the tone shortest possible value.
    num_expected_leader_tone_samples_ = MillisecondsToNumSamples(
        sample_rate, VISTones::kLeaderTone.duration_ms - transition_in_ms);
    num_expected_break_tone_samples_ = MillisecondsToNumSamples(
        sample_rate, VISTones::kBreakTone.duration_ms - transition_in_ms);

    // Duration of transition.
    //
    // Multiply by 2 to compensate for possible too early transition to the
    // state caused by worst case tone duration estimation (we might have
    // estimated it wrong that the transition edge only affects the duration
    // of the leader tone).
    num_transition_samples_ =
        MillisecondsToNumSamples(sample_rate, transition_in_ms * 2);
  }

  inline void ConfigureDataSampler(const Options& options) {
    // Configure PLL.
    //
    // Based on VIS specification the baud is 33.3, so we trick integer
    // arithmetic by pretending sample rate and baud are 3x of what they are.
    const typename signal::DigitalPLL<RealType>::Options pll_options = {
        .data_baud = 100,
        .sample_rate = options.sample_rate * 3,
        .inertia = options.pll_inertia,
    };
    pll_.Configure(pll_options);

    // Configure data bit averaging.
    //
    // We average some amount of samples prior to the bit sample point. This
    // allows to more reliably get bit value in a noisy signal.
    //
    // Here a heuristic is used to calculate averaging weight in a way that
    // averaging only happens for samples of the current bit and does not extent
    // to the previous bit.
    const RealType bit_duration_in_samples =
        VISTones<RealType>::kBitDurationInMilliseconds / 1000.0f *
        options.sample_rate;
    code_sample_weight_ = 100.0f / bit_duration_in_samples;
  }

  void ConfigureEdgeDetector(const Options& options) {
    edge_detector_.SetSampleWeight(options.edge_sample_weight,
                                   options.edge_sample_slow_weight);
    edge_detector_.SetFallingEdgeThreshold(options.edge_threshold);
  }

  //////////////////////////////////////////////////////////////////////////////
  // State machine.

  enum class State {
    kWaitForLeaderToneAfterReset,
    kWaitForLeaderToneAfterBreak,
    kSampleLeaderTone,

    kWaitForBreakTone,
    kSampleBreakTone,

    kWaitForLeaderToneEdge,

    kSampleCode,
  };

  // Reset the machine to its initial state.
  inline void Reset() {
    state_ = State::kWaitForLeaderToneAfterReset;
    num_decoded_leader_tones_ = 0;
  }

  // Machine entry point.
  inline auto PushFrequencySampleToMachine(const RealType frequency) -> Result {
    // Always feed samples to the edge detector, so that it can properly track
    // average values and other internal state.
    detected_edge_ = edge_detector_(frequency);

    switch (state_) {
      case State::kWaitForLeaderToneAfterReset:
        return HandleWaitForLeaderToneAfterReset(frequency);
      case State::kWaitForLeaderToneAfterBreak:
        return HandleWaitForLeaderToneAfterBreak(frequency);
      case State::kSampleLeaderTone: return HandleSampleLeaderTone(frequency);

      case State::kWaitForBreakTone: return HandleWaitForBreakTone(frequency);
      case State::kSampleBreakTone: return HandleSampleBreakTone(frequency);

      case State::kWaitForLeaderToneEdge:
        return HandleWaitForLeaderToneEdge(frequency);

      case State::kSampleCode: return HandleSampleCode(frequency);
    }

    UNREACHABLE();
  }

  // Wait for frequency to fall into the range of leader tone.

  inline auto HandleWaitForLeaderToneAfterReset(const RealType frequency)
      -> Result {
    if (!leader_tone_freq_interval_.Contains(frequency)) {
      Reset();
      return Result(Error::kUnavailable);
    }

    SwitchToSampleLeaderTone();

    return Result(Error::kUnavailable);
  }

  // Wait for the leader tone after break.
  // Will take care of ignoring transient processes which causes transition edge
  // slope and possible ringing.

  inline void SwitchToWaitForLeaderToneAfterBreak() {
    state_ = State::kWaitForLeaderToneAfterBreak;
    num_samples_in_state_ = 0;
  }

  auto HandleWaitForLeaderToneAfterBreak(const RealType frequency) -> Result {
    if (leader_tone_freq_interval_.Contains(frequency)) {
      SwitchToSampleLeaderTone();
      return Result(Error::kUnavailable);
    }

    ++num_samples_in_state_;
    if (num_samples_in_state_ > num_transition_samples_) {
      Reset();
    }

    return Result(Error::kUnavailable);
  }

  // Sample the leader tone and watch that all samples fall into its range.

  inline void SwitchToSampleLeaderTone() {
    state_ = State::kSampleLeaderTone;
    num_samples_in_state_ = 0;
  }

  auto HandleSampleLeaderTone(const RealType frequency) -> Result {
    if (!leader_tone_freq_interval_.Contains(frequency)) {
      Reset();
      return Result(Error::kUnavailable);
    }

    ++num_samples_in_state_;
    if (num_samples_in_state_ < num_expected_leader_tone_samples_) {
      return Result(Error::kUnavailable);
    }

    ++num_decoded_leader_tones_;

    if (num_decoded_leader_tones_ == 1) {
      SwitchToWaitForBreakTone();
      return Result(Error::kUnavailable);
    }

    if (num_decoded_leader_tones_ == 2) {
      SwitchToWaitLeaderEdge();
      return Result(Error::kUnavailable);
    }

    return Result(Error::kUnavailable);
  }

  // Wait for a transition from leader to break tone happen, watching for it to
  // take reasonable amount of time.

  inline void SwitchToWaitForBreakTone() {
    state_ = State::kWaitForBreakTone;
    num_samples_in_state_ = 0;
  }

  inline auto HandleWaitForBreakTone(const RealType frequency) -> Result {
    if (break_tone_freq_interval_.Contains(frequency)) {
      SwitchToSampleBreakTone();
      return Result(Error::kUnavailable);
    }

    ++num_samples_in_state_;

    if (num_samples_in_state_ > num_transition_samples_) {
      Reset();
      return Result(Error::kUnavailable);
    }

    return Result(Error::kUnavailable);
  }

  // Sample the break tone and watch that all samples fall into its range.

  inline void SwitchToSampleBreakTone() {
    state_ = State::kSampleBreakTone;
    num_samples_in_state_ = 0;
  }

  inline auto HandleSampleBreakTone(const RealType frequency) -> Result {
    if (!break_tone_freq_interval_.Contains(frequency)) {
      Reset();
      return Result(Error::kUnavailable);
    }

    ++num_samples_in_state_;
    if (num_samples_in_state_ < num_expected_break_tone_samples_) {
      return Result(Error::kUnavailable);
    }

    SwitchToWaitForLeaderToneAfterBreak();

    return Result(Error::kUnavailable);
  }

  // Wait for the falling edge of the leader tone prior to the VIS code
  // encoded sequence.

  inline void SwitchToWaitLeaderEdge() {
    state_ = State::kWaitForLeaderToneEdge;
    num_samples_in_state_ = 0;
  }

  inline auto HandleWaitForLeaderToneEdge(const RealType /*frequency*/)
      -> Result {
    if (detected_edge_.falling) {
      SwitchToSampleCode();
      return Result(Error::kUnavailable);
    }

    ++num_samples_in_state_;
    if (num_samples_in_state_ > num_transition_samples_) {
      Reset();
    }

    return Result(Error::kUnavailable);
  }

  // VIS code sampling.

  inline void SwitchToSampleCode() {
    state_ = State::kSampleCode;

    num_samples_in_state_ = 0;
    code_skip_next_bit_ = false;
    num_decoded_code_bits_ = 0;
    decoded_code_ = 0;
    num_ones_in_decoded_code_ = 0;

    // Initialize averaged frequency value to an expected value of the tone.
    // This simplifies averaging later as no extra state tracking is needed.
    averaged_frequency_ = RealType(VISTones<RealType>::kStartBit.frequency);

    pll_.Reset();
  }

  inline auto HandleSampleCode(const RealType frequency) -> Result {
    enum class BitValue {
      UNKNOWN = -1,
      ZERO = 0,
      ONE = 1,
      START_STOP = 2,
    };

    averaged_frequency_ = ExponentialMovingAverage(
        frequency, code_sample_weight_, averaged_frequency_);

    if (!pll_(false)) {
      return Result(Error::kUnavailable);
    }

    if (code_skip_next_bit_) {
      // TODO(sergey): Provide some information about synchronization. It should
      // be possible to lock the signal relative to the trailing edge of the
      // second leader tone.

      // Decoding is fully done.
      Reset();

      return Result(decoded_code_);
    }

    BitValue bit_value = BitValue::UNKNOWN;
    if (bit_value_zero_freq_interval_.Contains(averaged_frequency_)) {
      bit_value = BitValue::ZERO;
    } else if (bit_value_one_freq_interval_.Contains(averaged_frequency_)) {
      bit_value = BitValue::ONE;
    } else if (start_bit_freq_interval_.Contains(averaged_frequency_) ||
               stop_bit_freq_interval_.Contains(averaged_frequency_)) {
      bit_value = BitValue::START_STOP;
    }

    // Advance the number of bits now, so that we can have early exits.
    ++num_decoded_code_bits_;

    if (num_decoded_code_bits_ == 1) {
      // FIrst bit is a start bit. Is not part of the code, so just verify it
      // and move on to the next bit.
      //
      // TODO(sergey): It is possible that on noisy signals it might be helpful
      // to ease protocol check a bit here and allow the start and stop bits to
      // be outside of the specified range.

      if (bit_value != BitValue::START_STOP) {
        Reset();
      }

      return Result(Error::kUnavailable);
    }

    // 10 is start bit, 7 code bits, 1 parity bit, and 1 stop bit.
    if (num_decoded_code_bits_ == 10) {
      if (bit_value != BitValue::START_STOP) {
        Reset();
      }

      // We are in the middle of the last bit and we need to skip the second
      // part of it. Since the PLL samples bits in the middle of bit we can
      // reset the PLL and stop decoding code when PLL next time requests to
      // sample bit. At the time the signal will be at the end of the stop bit.
      code_skip_next_bit_ = true;
      pll_.Reset();

      return Result(Error::kUnavailable);
    }

    if (bit_value != BitValue::ZERO && bit_value != BitValue::ONE) {
      return Result(Error::kUnavailable);
    }

    const int int_bit_value = static_cast<int>(bit_value);

    // 9 is 1 start bit, 7 code bits, 1 parity bit.
    // Here parity of the received code is checked: the parity should be even.
    if (num_decoded_code_bits_ == 9) {
      if ((num_ones_in_decoded_code_ & 0x1) != int_bit_value) {
        Reset();
        return Result(Error::kUnavailable);
      }

      return Result(Error::kUnavailable);
    }

    decoded_code_ |= (int_bit_value << (num_decoded_code_bits_ - 2));
    num_ones_in_decoded_code_ += int_bit_value;

    return Result(Error::kUnavailable);
  }

  // Properties related to the state machine.

  // Current state of the machine.
  State state_;

  // Number of samples handled by the current state.
  // NOTE: Only for states which needs to track the number of samples.
  int num_samples_in_state_;

  int num_decoded_leader_tones_;

  // Aggressive averaging for data bit sampling.
  // Is calculated during VIS code sampling since there the clock is defined by
  // PLL and sharpness of transition edges is not that important.
  RealType averaged_frequency_;

  int num_decoded_code_bits_;
  int decoded_code_;
  int num_ones_in_decoded_code_;

  // Skip next bit requested to be sampled by PLL when decoding VIS code.
  bool code_skip_next_bit_;

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Range of frequencies which count as a leader and break tones respectively.
  Interval<Frequency> leader_tone_freq_interval_;
  Interval<Frequency> break_tone_freq_interval_;

  Interval<Frequency> start_bit_freq_interval_;
  Interval<Frequency> bit_value_zero_freq_interval_;
  Interval<Frequency> bit_value_one_freq_interval_;
  Interval<Frequency> stop_bit_freq_interval_;

  // Number of samples which should be within the leader and break tones
  // frequency range respectively. for the signal to be considered as a leader
  // tone.
  //
  // Note that this number is lower than what math gives for the tone duration
  // and sample rate to allow some slack on tone transients (for example,
  // overshoots of the Hilbert machine, edge slope due to smoothing).
  int num_expected_leader_tone_samples_;
  int num_expected_break_tone_samples_;

  // Number of samples within which frequency of a tone is expected to
  // stabilize.
  int num_transition_samples_;

  // Weight used for averaging of frequency samples during bit sampling.
  // This weight is high enough to help with noisy signal, but is low enough to
  // not have previous bit affect sampling of the current bit.
  RealType code_sample_weight_;

  signal::SimpleFIRFilter<RealType, RealType, Allocator> prefilter_;

  using EdgeDetector = signal::EdgeDetector<RealType, false, true>;
  EdgeDetector edge_detector_;
  typename EdgeDetector::Edge detected_edge_;

  signal::DigitalPLL<RealType> pll_;

  // Delay of this processor in milliseconds.
  //
  // The delay is measured from the last sample of VIS was pushed to this
  // decoder and decoder triggered the VIS synchronization.
  RealType delay_in_ms_;
};

}  // namespace radio_core::picture::sstv
