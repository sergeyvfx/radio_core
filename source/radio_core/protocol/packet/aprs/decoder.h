// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoder of APRS transmissions.
//
// Uses configurable tones and baud rate, but expects messages to use AX.25
// framing and NRZS coding.
//
// The input of the decoder is IF samples in an amplitude domain, and the output
// is decoded AX.25 messages in either Result form or passed to a given
// callback.

#pragma once

#include "radio_core/modulation/digital/fsk/demodulator.h"
#include "radio_core/modulation/digital/fsk/tones.h"
#include "radio_core/protocol/binary/nrzs/decoder.h"
#include "radio_core/protocol/datalink/ax25/decoder.h"
#include "radio_core/protocol/datalink/hdlc/decoder.h"

namespace radio_core::protocol::packet::aprs {

template <class RealType, template <class> class Allocator = std::allocator>
class Decoder {
 public:
  struct Options {
    // Tones of modulated mark and space symbols.
    modulation::digital::fsk::Tones tones{0, 0};

    // Sample rate of the incoming samples (samples per second).
    RealType sample_rate{0};

    // Baud rate: symbols per second in the data stream.
    int data_baud{0};
  };

  using Error = protocol::datalink::ax25::Decoder::Error;
  using Result = protocol::datalink::ax25::Decoder::Result;

  Decoder() = default;
  explicit Decoder(const Options& options) { Configure(options); }

  void Configure(const Options& options) {
    typename FSKDemodulator::Options fsk_options;
    fsk_options.tones = options.tones;
    fsk_options.sample_rate = options.sample_rate;
    fsk_options.data_baud = options.data_baud;
    fsk_demodulator_.Configure(fsk_options);
  }

  // Process sample of input signal.
  //
  // The result follows semantic of the AX.25 decoder.
  auto operator()(const RealType sample) -> Result {
    Result result(Error::kUnavailable);

    const typename FSKDemodulator::Result fsk_result = fsk_demodulator_(sample);
    if (!fsk_result.Ok()) {
      return result;
    }

    const bool demodulated_bit = fsk_result.GetValue();
    const bool decoded_bit = nrzs_decoder_(demodulated_bit);

    const HDLCDecoder::Result hdlc_result = hdlc_decoder_(decoded_bit);
    if (!hdlc_result.Ok()) {
      return result;
    }

    // Process all possible frame markers and data bits.
    for (const auto& frame_byte : hdlc_result.GetValue()) {
      const AX25Decoder::Result ax25_result = ax25_decoder_(frame_byte);
      if (ax25_result.Ok()) {
        // Processing happens on a per-bit level, so it is not expected to have
        // multiple messages decoded.
        assert(!result.Ok());

        result = ax25_result;
      }
    }

    return result;
  }

 private:
  using FSKDemodulator =
      modulation::digital::fsk::Demodulator<RealType, Allocator>;
  using NRZSDecoder = protocol::binary::nrzs::Decoder;
  using HDLCDecoder = protocol::datalink::hdlc::Decoder;
  using AX25Decoder = protocol::datalink::ax25::Decoder;

  FSKDemodulator fsk_demodulator_;
  NRZSDecoder nrzs_decoder_;
  HDLCDecoder hdlc_decoder_;
  AX25Decoder ax25_decoder_;
};

}  // namespace radio_core::protocol::packet::aprs
