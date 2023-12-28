// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Decoding machine of non-return-to-zero space (also known as NRZ(S)) code.
// The machine receives encoded bit and returns decoded data bit. This is a low
// level building block for processors.
//
// NRZ(S): A logical 0 is transmitted as a transition,
//         A logical 1 is transmitted as no transition.
//
// The initial state after the decoder reset is that the previous encoded bit
// is assumed to be logical 0.

#pragma once

namespace radio_core::protocol::binary::nrzs {

class Decoder {
 public:
  Decoder() = default;

  // Reset the decoder to its initial state.
  void Reset() { previous_encoded_bit_ = false; }

  // Returns decoded data bit.
  inline auto operator()(bool encoded_bit) -> bool {
    const bool data_bit = (encoded_bit == previous_encoded_bit_);
    previous_encoded_bit_ = encoded_bit;
    return data_bit;
  }

 private:
  bool previous_encoded_bit_ = false;
};

}  // namespace radio_core::protocol::binary::nrzs
