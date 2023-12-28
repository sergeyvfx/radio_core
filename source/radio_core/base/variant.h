// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <variant>

namespace radio_core {

// Overloaded visitor pattern, allows vising variant values in the following
// manner:
//
//   std::variant<int, float, double> my_variant = ... ;
//
//   my_variant | match{
//                    [&](int value) { stream << "INT " << value; },
//                    [&](float value) { stream << "FLOAT " << value; },
//                    [&](double value) { stream << "DOUBLE " << value; },
//                };
//
// References:
//
//   cppreference example
//   https://en.cppreference.com/w/cpp/utility/variant/visit
//
//   C++ Variant visit overloaded function
//   https://stackoverflow.com/a/72491201

template <class... Ts>
struct match : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
match(Ts...) -> match<Ts...>;

template <class... Ts, class... Fs>
constexpr auto operator|(std::variant<Ts...> const& v,
                         match<Fs...> const& match) -> decltype(auto) {
  return std::visit(match, v);
}

}  // namespace radio_core
