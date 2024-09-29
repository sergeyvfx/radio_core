// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

// Abstraction of a typical signal path in radio.
//
// Implements decimation to an intermediate frequency (IF), applying a filter,
// and demodulating into an audio frequency (AF).
//
// It is also possible to attach sinks at different stages of the path.
//
// This class implements the most portable part of the signal processing. It
// does not perform any thread synchronization which makes it portable on
// various devices, but requires manual thread synchronization: the signal path
// is not to be modified or re-configured while it processes samples.

#pragma once

#include <memory>
#include <span>
#include <vector>

#include "radio_core/base/container.h"
#include "radio_core/modulation/analog/bandwidth.h"
#include "radio_core/modulation/analog/iq_demodulator.h"
#include "radio_core/signal/decimator.h"
#include "radio_core/signal/ema_agc.h"
#include "radio_core/signal/frequency_shifter.h"
#include "radio_core/signal_path/internal/decimation_ratio.h"
#include "radio_core/signal_path/internal/demodulator.h"
#include "radio_core/signal_path/internal/receive_filter.h"
#include "radio_core/signal_path/sink.h"
#include "radio_core/signal_path/sink_collection.h"

namespace radio_core::signal_path {

namespace signal_path_internal {

// Compile-time stages definition.
//
// Provides information about stages of the signal processing and type of
// samples the stage is operating on.
//
// The goal is to provide an API which allows to code-generate signal
// processor which attaches to a stage which is known at compile time.

// TODO(sergey): Support attachment to the IQ and PSD sinks in the templated
// manner.

// Stage of the radio signal path at which the decoder is tapped to it.
enum class Stage {
  // Intermediate frequency stage.
  // Signal at this stage has passed through frequency shift and the input
  // receive filter.
  kIF,

  // Audio frequency.
  // Signal at this stage has passed through demodulator and decimation to
  // the audio frequency.
  kAF,
};

// Traits of the radio stage.
//
// Provides the following types:
//  - SampleType is the type of a sample at the stage.
//  - SinkType type of a sink which is expected at the stage.
template <class RealType, Stage kStage>
struct StageTraits;

// Intermediate frequency stage.
template <class RealType>
struct StageTraits<RealType, Stage::kIF> {
  using SampleType = BaseComplex<RealType>;
  using SinkType = Sink<SampleType>;
};

// Audio frequency stage.
template <class RealType>
struct StageTraits<RealType, Stage::kAF> {
  using SampleType = RealType;
  using SinkType = Sink<SampleType>;
};

// Storage of attachable sink at various signal processing stages of the signal
// path.
// It is defined outside of the class to allow compile time access to a sink
// collection of a specific stage.
template <class RealType, template <class> class Allocator = std::allocator>
struct Sinks {
  SinkCollection<BaseComplex<RealType>, Allocator> if_sink;
  SinkCollection<RealType, Allocator> af_sink;
};

// Accessor to an attachable sink collection at the specified by the template
// argument stage.
//
// The accessor is defined outside of the SignalPath because it is not possible
// in C++ to provide template specializations in the class, and it is not
// possible to provide specializations for a method of a no-fully specialized
// class.
//
// Example access:
//
//   SinkAccessor<T, Allocator, Stage::kAF>(sinks_);

template <class RealType, template <class> class Allocator, Stage kStage>
struct SinkAccessor {};

template <class RealType, template <class> class Allocator>
struct SinkAccessor<RealType, Allocator, Stage::kIF> {
  static auto Get(Sinks<RealType, Allocator>& sinks)
      -> SinkCollection<typename StageTraits<RealType, Stage::kIF>::SampleType,
                        Allocator>& {
    return sinks.if_sink;
  }
};

template <class RealType, template <class> class Allocator>
struct SinkAccessor<RealType, Allocator, Stage::kAF> {
  static auto Get(Sinks<RealType, Allocator>& sinks)
      -> SinkCollection<typename StageTraits<RealType, Stage::kAF>::SampleType,
                        Allocator>& {
    return sinks.af_sink;
  }
};

}  // namespace signal_path_internal

template <class T, template <class> class Allocator = std::allocator>
class BaseSignalPath : public Sink<BaseComplex<T>> {
  using Demodulator = internal::Demodulator<T, Allocator>;
  using ReceiveFilter = internal::ReceiveFilter<T, Allocator>;

 public:
  struct Options {
    // Input stage configuration.
    struct {
      int sample_rate{0};

      // Shift in frequency prior down-sampling the signal to IF, measured in
      // Hz.
      //
      // If the IQ signal centered around 145.4 MHz and the radio station of
      // interest is at 145.3 MHz the shift is to be set to -100000.
      T frequency_shift{0};
    } input;

    // Receive filter.
    //
    // The receive filter is applied on the signal at the intermediate frequency
    // sample rate prior to sending the signal to the demodulator.
    struct {
      // Bandwidth of the receive filter, in hertz.
      T bandwidth{1200};

      // Accuracy of the bandwidth, should be equal or less than 1.
      //
      // Is used when calculating decimation factor for the IF stage: allow
      // less precise filter bandwidth if that means that the overall signal
      // processing will be less mathematically complex.
      //
      // For example, when using IQ sample rate of 6 Msps and WFM filter
      // bandwidth of 250 kHz this allows to keep the IF sample rate at 240000
      // with the filter bandwidth being off by 4%. If the accuracy is 1 the IF
      // sample rate would to be 1200 kHz, and overall processing will be 4x
      // slower.
      T bandwidth_accuracy{0.95};

      // Width of the transition band measured as a factor of the bandwidth.
      T transition_band_factor{0.05};
    } receive_filter;

    // NOTE: Sample rates in the specific demodulator options are overwritten by
    // the signal path.
    typename Demodulator::Options demodulator;

    // Audio frequency stage.
    struct {
      int sample_rate{0};

      struct {
        T charge_rate{0.007};
        T discharge_rate{0.00003};
      } agc;

      // Configuration of soft transition when radio is first started and when
      // it is reconfigured.
      // The time is measured in seconds.
      //
      // The soft startup forces audio to gradually increase in volume from 0 to
      // the demodulated volume upon the application startup. This avoid a
      // perceived pop un the radio start.
      //
      // The soft re-configure forces audio to go to zero and gradually to the
      // desired level whenever the radio settings affecting modulation are
      // changed. This gives AGC time to re-adjust and avoids popping sound when
      // modulation setting is changed.
      T soft_startup_time{1};
      T soft_configure_time{0.1};
    } audio;
  };

  // Stage of the radio signal path at which the decoder is tapped to it.
  using Stage = signal_path_internal::Stage;

  template <Stage kStage>
  using StageTraits = signal_path_internal::StageTraits<T, kStage>;

  // Sing for the IQ samples at the intermediate state.
  // The samples are subjected to the receive filter and decimation from the
  // input to the intermediate frequency sample rate.
  using IFSink = Sink<typename StageTraits<Stage::kIF>::SampleType>;

  // Sink for the demodulated audio samples at the audio frequency sample rate.
  using AFSink = Sink<typename StageTraits<Stage::kAF>::SampleType>;

  void Configure(const Options& options) {
    Lock();

    if (NeedResetSoftAudioStart(options)) {
      ResetSoftAudioStartUnsafe();
    }

    ConfigureDecimation(options);

    ConfigureInputFrequencyShifter(options);
    ConfigureReceiveFilter(options);

    ConfigureDemodulator(options);

    ConfigureAudioOutput(options);

    Unlock();
  }

  // Perform soft audio startup.
  // Resets the state of the AGC and forces output audio volume to start from 0
  // and gradually rise up.
  void ResetSoftAudioStart() {
    Lock();
    ResetSoftAudioStartUnsafe();
    Unlock();
  }

  // Compile-time flexible sink connection to a specific stage of the signal
  // processing path.
  //
  // The signal path references the sink, so caller needs to ensure the lifetime
  // of the sink.
  //
  // Example:
  //
  //   using SignalPath = SignalPath<float>;
  //
  //   MyAFSink af_sink;
  //   signal_path.AddSink<SignalPath::Stage::kAF>(af_sink);

  template <Stage kStage>
  void AddSink(typename StageTraits<kStage>::SinkType& sink) {
    Lock();
    GetSinkForStage<kStage>().AddSink(sink);
    Unlock();
  }

  template <Stage kStage>
  void RemoveSink(const typename StageTraits<kStage>::SinkType& sink) {
    Lock();
    GetSinkForStage<kStage>().RemoveSink(sink);
    Unlock();
  }

  // Sink of signal sampled at the intermediate frequency sample rate.
  //
  // Signal at this stage has passed through frequency shift and the input
  // receive filter.
  //
  // The signal path references the sink, so caller needs to ensure the lifetime
  // of the sink.
  void AddIFSink(IFSink& sink) { AddSink<Stage::kIF>(sink); }
  void RemoveIFSink(const IFSink& sink) { RemoveSink<Stage::kIF>(sink); }

  // Sink of demodulated signal sampled at the audio frequency sample rate.
  // The signal is affected by squelch and soft audio startup.
  //
  // Signal at this stage has passed through demodulator and decimation to
  // the audio frequency.
  //
  // The signal path references the sink, so caller needs to ensure the lifetime
  // of the sink.
  void AddAFSink(AFSink& sink) { AddSink<Stage::kAF>(sink); }
  void RemoveAFSink(const AFSink& sink) { RemoveSink<Stage::kAF>(sink); }

  // Actual signal processing.
  void PushSamples(std::span<const BaseComplex<T>> input_iq_samples) override {
    Lock();

    // Make sure the work buffers are big enough for the samples.

    const size_t decimated_if_size =
        if_decimator_.CalcNeededOutputBufferSize(input_iq_samples.size());
    const size_t filtered_if_size =
        receive_filter_.CalcNeededOutputBufferSize(decimated_if_size);
    const size_t if_buffer_size = Max(decimated_if_size, filtered_if_size);

    EnsureSizeAtLeast(iq_buffer_, input_iq_samples.size());
    EnsureSizeAtLeast(if_buffer_, if_buffer_size);
    EnsureSizeAtLeast(af_buffer_, if_buffer_.size());

    // Shift the frequency.
    //
    // TODO(sergey): Look into some const-expression way to disable the shift,
    // to help using the pipeline on a slow hardware.
    //
    // On a fast hardware always use the frequency shift to avoid situations
    // when user input increases compute power, and help debugging the
    // bottlenecks and thr worst case processing scenario.
    const std::span<BaseComplex<T>> shifted_iq_samples =
        iq_frequency_shifter_(input_iq_samples, iq_buffer_);

    // Decimated IQ samples from the radio sampling rate to the IF sampling
    // rate.
    const std::span<BaseComplex<T>> if_samples =
        if_decimator_(shifted_iq_samples, if_buffer_);

    // Apply bandwidth filter.
    const std::span<BaseComplex<T>> filtered_if_samples =
        receive_filter_(if_samples, if_buffer_);

    // Move the sideband back: the input frequency shifter centered the side
    // band around DC, but for de-modulation it needs to be moved to where it
    // was before such shift.
    if_frequency_shifter_(filtered_if_samples);

    sinks_.if_sink.PushSamples(filtered_if_samples);

    // Demodulate the audio.
    const std::span<T> demodulated_samples =
        demodulator_(filtered_if_samples, af_buffer_);
    const std::span<T> af_samples = af_decimator_(demodulated_samples);

    // AGC and soft start volume.
    for (T& af_sample : af_samples) {
      // TODO(sergey): Implement squelch.

      // TODO(sergey): Is the really AGC needed for all modulation types? It
      // seems that it makes popping sound when NFM transmission ends worse.
      // (Even without AGC there is some popping, but it is not as bad as when
      // the AGC is used).
      af_sample = agc_(af_sample) * soft_start_volume_ * soft_configure_volume_;

      soft_start_volume_ = Min(T(1), soft_start_volume_ + soft_start_weight_);
      soft_configure_volume_ =
          Min(T(1), soft_configure_volume_ + soft_configure_weight_);
    }

    // TODO(sergey): Consider adding an explicit AF filter, for modulation types
    // which has much lower bandwidth than the audio sink.

    sinks_.af_sink.PushSamples(af_samples);

    Unlock();
  }

  // Get configured sample rate at the different stages.
  auto GetInputSampleRate() const -> int { return input_sample_rate_; }
  auto GetIFSampleRate() const -> int { return if_sample_rate_; }
  auto GetAFSampleRate() const -> int { return af_sample_rate_; }

  // Get receive filter configuration.
  auto GetReceiveFilterDecimationRatio() -> T {
    return receive_filter_.GetDecimationRatio();
  }
  auto GetReceiveFilterBandwidth() -> T {
    return receive_filter_.GetBandwidth();
  }
  auto GetReceiveFilterTransitionBand() -> T {
    return receive_filter_.GetTransitionBand();
  }
  auto GetReceiveFilterKernelSize() -> size_t {
    return receive_filter_.GetKernelSize();
  }

 protected:
  // Thread synchronization: an explicit lock/unlock functionality.
  virtual void Lock() {}
  virtual void Unlock() {}

 private:
  using StagesDecimation = internal::StagesDecimation<T>;

  //////////////////////////////////////////////////////////////////////////////
  // Helpers.

  // Get sink at the given processing stage.
  template <Stage kStage>
  inline auto GetSinkForStage()
      -> SinkCollection<typename StageTraits<kStage>::SampleType, Allocator>& {
    return SinkAccessor<kStage>::Get(sinks_);
  }

  // Non-thread guarded implementation of soft audio start configuration.
  inline void ResetSoftAudioStartUnsafe() {
    soft_configure_volume_ = T(0);
    agc_.Reset();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Configuration.

  // Calculate offset needed to center the sideband of interest around the DC.
  //
  // For AM and FM modulation this offset is 0. For the LSB and USB modulation
  // this value is equal to the half of the receiver filter bandwidth, and is
  // positive for LSB and negative for USB. This makes it so the sideband of
  // interest is centered around DC.
  //
  // This offset allows to use simple low pass filter with real coefficients as
  // a receiver filter.
  auto GetBandwidthOffsetToCenter(const Options& options) -> T {
    const Interval<T> bandwidth_interval =
        modulation::analog::GetBandwidthIntervalAroundCenterFrequency(
            options.demodulator.modulation_type,
            options.receive_filter.bandwidth);

    return -(bandwidth_interval.lower_bound + bandwidth_interval.upper_bound) /
           2;
  }

  void ConfigureInputFrequencyShifter(const Options& options) {
    const T bandwidth_offset = GetBandwidthOffsetToCenter(options);

    // Configure the input frequency shifter, which applies the user-configured
    // frequency shift, as well as the shift needed to be able to use simple
    // low-pass receiver filter with real coefficients.
    iq_frequency_shifter_.Configure(
        options.input.frequency_shift + bandwidth_offset,
        options.input.sample_rate);

    // Cancel out the frequency offset introduced for the receiver filter.
    if_frequency_shifter_.Configure(-bandwidth_offset, if_sample_rate_);
  }

  // Configure decimation across stages of the signal path.
  void ConfigureDecimation(const Options& options) {
    // Store requested sample rates.
    input_sample_rate_ = options.input.sample_rate;
    af_sample_rate_ = options.audio.sample_rate;

    // Calculate decimation ratio between various stages.
    const typename StagesDecimation::Ratio stage_ratio =
        StagesDecimation::Calculate(
            input_sample_rate_,
            af_sample_rate_,
            options.receive_filter.bandwidth *
                options.receive_filter.bandwidth_accuracy);

    if_decimator_.SetRatio(stage_ratio.iq_to_if);
    af_decimator_.SetRatio(stage_ratio.if_to_af);

    // Store calculated intermediate frequency stage sample rate.
    if_sample_rate_ = input_sample_rate_ / stage_ratio.iq_to_if;
  }

  void ConfigureReceiveFilter(const Options& options) {
    const typename ReceiveFilter::Options filter_options = {
        .sample_rate = T(if_sample_rate_),
        .bandwidth = options.receive_filter.bandwidth,
        .transition_band = options.receive_filter.bandwidth *
                           options.receive_filter.transition_band_factor,
    };

    receive_filter_.Configure(filter_options);
  }

  void ConfigureDemodulator(const Options& options) {
    typename Demodulator::Options demodulator_options = options.demodulator;
    demodulator_options.nfm.sample_rate = if_sample_rate_;
    demodulator_options.wfm.sample_rate = if_sample_rate_;
    demodulator_options.cw.sample_rate = if_sample_rate_;

    demodulator_.Configure(demodulator_options);
  }

  // Configure output audio processing: AGC and soft start.
  void ConfigureAudioOutput(const Options& options) {
    const int af_sample_rate = options.audio.sample_rate;

    agc_.Configure(options.audio.agc.charge_rate,
                   options.audio.agc.discharge_rate);

    soft_start_weight_ =
        T(1) / (options.audio.soft_startup_time * af_sample_rate);
    soft_configure_weight_ =
        T(1) / (options.audio.soft_configure_time * af_sample_rate);
  }

  // Return true if the new configuration requires the audio to perform the soft
  // start again.
  inline auto NeedResetSoftAudioStart(const Options& options) -> bool {
    // Changes in the modulation type needs to do soft audio start so that there
    // is no click due to the difference in the demodulated amplitude until the
    // AGC is adapted.
    if (demodulator_.GetModulationType() !=
        options.demodulator.modulation_type) {
      return true;
    }

    return false;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Properties.

  // Shifter of the IQ signal.
  //
  // It applies frequency offset configured by the tuning options, as well as
  // an offset which makes it so the bandwidth of interest is centered around
  // the DC. The latter allows to run simple low-pass filter with real
  // coefficients as a receiver stage filter.
  signal::FrequencyShifter<T> iq_frequency_shifter_;

  // Downsampler from radio sampling rate to a sample rate of an intermediate
  // frequency (IF).
  signal::Decimator<BaseComplex<T>, T, Allocator> if_decimator_;

  // Downsampler from demodulated sample rate to the audio output sample rate.
  signal::Decimator<T, T, Allocator> af_decimator_;

  // Receive filter.
  // It is applied on the IF stage which is expected to have the bandwidth of
  // interest centered around the DC.
  ReceiveFilter receive_filter_;

  // Shifter of the IF signal.
  //
  // This shift is performed after the receiver filter and cancels out the
  // offset applied on the input frequency in order to center the bandwidth of
  // interest around DC for the low-pass receiver filter.
  signal::FrequencyShifter<T> if_frequency_shifter_;

  // Demodulator of IF into AF.
  Demodulator demodulator_;

  // Automatic gain control for audio.
  signal::EMAAGC<T> agc_;

  // Configuration of the soft startup and soft re-configure.
  T soft_start_volume_ = 0;
  T soft_start_weight_ = 0;
  T soft_configure_volume_ = 1;
  T soft_configure_weight_ = 0;

  // Work buffer for IQ preprocessor (such as frequency shifting).
  std::vector<BaseComplex<T>, Allocator<BaseComplex<T>>> iq_buffer_;

  // Work buffer for decimation, bandwidth filter, and decimation to
  // demodulation sample rate.
  std::vector<BaseComplex<T>, Allocator<BaseComplex<T>>> if_buffer_;

  // Work buffer for audio demodulation and AGC.
  std::vector<T, Allocator<T>> af_buffer_;

  // Sinks and templated accessor to them.
  template <Stage kStage>
  using SinkAccessor = signal_path_internal::SinkAccessor<T, Allocator, kStage>;
  signal_path_internal::Sinks<T, Allocator> sinks_;

  // Sample rates at the different stages of the processing path.
  int input_sample_rate_{0};
  int if_sample_rate_{0};
  int af_sample_rate_{0};
};

}  // namespace radio_core::signal_path
