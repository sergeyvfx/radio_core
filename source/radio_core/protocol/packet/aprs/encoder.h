// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoder of APRS transmissions.
//
// Uses configurable tones and baud rate, but expects messages to use AX.25
// framing and NRZS coding.
//
// The input of the encoder is AX.25 message, the output is signal in its
// amplitude domain, which is ready to either be fed to a radio or saved to
// a storage.

#pragma once

#include <functional>

#include "radio_core/modulation/digital/fsk/modulator.h"
#include "radio_core/modulation/digital/fsk/tones.h"
#include "radio_core/protocol/binary/nrzs/encoder.h"
#include "radio_core/protocol/datalink/ax25/encoder.h"
#include "radio_core/protocol/datalink/ax25/message.h"
#include "radio_core/protocol/datalink/hdlc/encoder.h"
#include "radio_core/signal/generator.h"

namespace radio_core::protocol::packet::aprs {

template <class RealType>
class Encoder {
 public:
  struct Options {
    // Tones of modulated mark and space symbols.
    modulation::digital::fsk::Tones tones{0, 0};

    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate{0};

    // Baud rate: symbols per second in the data stream.
    int data_baud{0};

    // Number of empty frames which lead and trail the encoded message.
    //
    // On a protocol level this follows the HDLC requirement to continuously
    // transmit a frame delimeter when no frames are being transmitted.
    //
    // In practice transmitting extra frame delimeter over the air allows the
    // receiver side to lock its time recovery circuit to the transmission.
    //
    // Trailing frame delimeter might not be so important from a perspective of
    // decoding a continuous stream, but it does help some applications to
    // decode messages encoded by this encoder. For example, without trailing
    // frame delimeter DireWolf will not decode a message from a WAV file
    // generated using this encoder. At the same time, SoundModem has no issues
    // decoding the same file.
    int num_leading_empty_frames{1};
    int num_trailing_empty_frames{1};
  };

  Encoder() = default;

  explicit Encoder(const Options& options) { Configure(options); }

  inline void Configure(const Options& options) {
    typename FSKModulator::Options fsk_options;
    fsk_options.tones = options.tones;
    fsk_options.data_baud = options.data_baud;
    fsk_modulator_.Configure(fsk_options);

    generator_.Configure(options.sample_rate);

    num_leading_empty_frames_ = options.num_leading_empty_frames;
    num_trailing_empty_frames_ = options.num_trailing_empty_frames;
  }

  // Encode the message.
  //
  // The callback will be invoked with a signal amplitude.
  //
  // The encoded and modulated signal in its amplitude domain is passes to the
  // callback, one sample per invocation. The given list of args... is passed to
  // the callback after the frame byte. This makes the required callback
  // signature to be:
  //
  //   callback(const RealType sample, <optional arguments>)
  template <class F, class... Args>
  void operator()(const protocol::datalink::ax25::Message& message,
                  F&& callback,
                  Args&&... args) {
    EncodeNumEmptyFrames(num_leading_empty_frames_,
                         std::forward<F>(callback),
                         std::forward<Args>(args)...);

    ax25_encoder_(message,
                  hdlc_encoder_,
                  nrzs_encoder_,
                  fsk_modulator_,
                  generator_,
                  std::forward<F>(callback),
                  std::forward<Args>(args)...);

    EncodeNumEmptyFrames(num_trailing_empty_frames_,
                         std::forward<F>(callback),
                         std::forward<Args>(args)...);

    generator_.FadeToZero(std::forward<F>(callback),
                          std::forward<Args>(args)...);
  }

 private:
  template <class F, class... Args>
  inline void EncodeNumEmptyFrames(const int num_empty_frames,
                                   F&& callback,
                                   Args&&... args) {
    for (int i = 0; i < num_empty_frames; ++i) {
      EncodeEmptyFrame(std::forward<F>(callback), std::forward<Args>(args)...);
    }
  }

  template <class F, class... Args>
  inline void EncodeEmptyFrame(F&& callback, Args&&... args) {
    hdlc_encoder_(protocol::datalink::FrameMarker::kBegin,
                  fsk_modulator_,
                  generator_,
                  std::forward<F>(callback),
                  std::forward<Args>(args)...);

    hdlc_encoder_(protocol::datalink::FrameMarker::kEnd,
                  fsk_modulator_,
                  generator_,
                  std::forward<F>(callback),
                  std::forward<Args>(args)...);
  }

  using AX25Encoder = protocol::datalink::ax25::Encoder;
  using HDLCEncoder = protocol::datalink::hdlc::Encoder;
  using NRZSEncoder = protocol::binary::nrzs::Encoder;
  using FSKModulator = modulation::digital::fsk::Modulator<RealType>;
  using Generator = signal::Generator<RealType>;

  AX25Encoder ax25_encoder_;
  HDLCEncoder hdlc_encoder_;
  NRZSEncoder nrzs_encoder_;
  FSKModulator fsk_modulator_;
  Generator generator_;

  int num_leading_empty_frames_{0};
  int num_trailing_empty_frames_{0};
};

}  // namespace radio_core::protocol::packet::aprs
