// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Encoding machine of non-return-to-zero space (also known as NRZ(S)) code.
// The machine receives data bit and returns an encoded data bit. This is a low
// level building block for processors.
//
// NRZ(S): A logical 0 is transmitted as a transition,
//         A logical 1 is transmitted as no transition.
//
// The initial state after the decoder reset is that the previous encoded bit
// is assumed to be logical 0.

#pragma once

#include <functional>

namespace radio_core::protocol::binary::nrzs {

class Encoder {
 public:
  Encoder() = default;

  // Reset the encoder to its initial state.
  void Reset() { previous_encoded_bit_ = false; }

  // Returns encoded bit.
  inline auto operator()(bool data_bit) -> bool {
    const bool encoded_bit =
        (data_bit) ? previous_encoded_bit_ : !previous_encoded_bit_;
    previous_encoded_bit_ = encoded_bit;
    return encoded_bit;
  }

  // Daisy-chainable encoiding, which allows to pass received of an encoded bit
  // as a functor.
  //
  // The encoded bit is passed to the callback. The given list of args... is
  // passed to the callback after the bit. This makes the required callback
  // signature to be:
  //
  //   callback(bool encoded_bit, <optional arguments>)
  template <class F, class... Args>
  inline void operator()(const bool data_bit, F&& callback, Args&&... args) {
    const bool encoded_bit = (*this)(data_bit);
    std::invoke(
        std::forward<F>(callback), encoded_bit, std::forward<Args>(args)...);
  }

 private:
  bool previous_encoded_bit_ = false;
};

}  // namespace radio_core::protocol::binary::nrzs
