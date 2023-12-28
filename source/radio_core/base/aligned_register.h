// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

// Naive implementation of a register data storage with a given data alignment.

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>

#include "radio_core/base/unroll.h"

namespace radio_core {

template <class ElementType, int NumElements, int Alignment>
class AlignedRegister {
 public:
  // Construct non-initialized register storage.
  constexpr AlignedRegister() = default;

  // Construct register with data from the given bounded array.
  constexpr AlignedRegister(ElementType data[NumElements]) {
    std::copy(data, data + NumElements, std::begin(data_));
  }
  constexpr AlignedRegister(const ElementType data[NumElements]) {
    std::copy(data, data + NumElements, std::begin(data_));
  }

  // Initialize all elements of the vector to the given value.
  constexpr AlignedRegister(const ElementType& value) {
    Unroll<NumElements>([&](const auto i) { data_[i] = value; });
  }

  // Construct register with the given elements.
  //
  // The number of given elements must be the same as the number of elements in
  // the register.
  template <class... Args,
            typename = std::enable_if_t<sizeof...(Args) == NumElements>>
  constexpr AlignedRegister(const Args&... args)
      : data_({static_cast<ElementType>(args)...}) {}

  // Access register element with the given index.
  //
  // If the index is above the number elements the behavior is undefined.
  constexpr auto operator[](const size_t index) -> ElementType& {
    assert(index < NumElements);
    return data_[index];
  }
  constexpr auto operator[](const size_t index) const -> const ElementType& {
    assert(index < NumElements);
    return data_[index];
  }

 private:
  alignas(Alignment) std::array<ElementType, NumElements> data_;
};

}  // namespace radio_core
