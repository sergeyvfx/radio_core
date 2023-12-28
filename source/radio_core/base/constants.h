// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <numbers>

#include "radio_core/base/half.h"

namespace radio_core::constants {

// Templated constants from the std::numbers namespace.
template <class T>
inline constexpr T e_v = std::numbers::e_v<T>;
template <class T>
inline constexpr T log2e_v = std::numbers::log2e_v<T>;
template <class T>
inline constexpr T log10e_v = std::numbers::log10e_v<T>;
template <class T>
inline constexpr T pi_v = std::numbers::pi_v<T>;
template <class T>
inline constexpr T inv_pi_v = std::numbers::inv_pi_v<T>;
template <class T>
inline constexpr T inv_sqrtpi_v = std::numbers::inv_sqrtpi_v<T>;
template <class T>
inline constexpr T ln2_v = std::numbers::ln2_v<T>;
template <class T>
inline constexpr T ln10_v = std::numbers::ln10_v<T>;
template <class T>
inline constexpr T sqrt2_v = std::numbers::sqrt2_v<T>;
template <class T>
inline constexpr T sqrt3_v = std::numbers::sqrt3_v<T>;
template <class T>
inline constexpr T inv_sqrt3_v = std::numbers::inv_sqrt3_v<T>;
template <class T>
inline constexpr T egamma_v = std::numbers::egamma_v<T>;
template <class T>
inline constexpr T phi_v = std::numbers::phi_v<T>;

// Constants from the std::numbers namespace.
inline constexpr double e = std::numbers::e;
inline constexpr double log2e = std::numbers::log2e;
inline constexpr double log10e = std::numbers::log10e;
inline constexpr double pi = std::numbers::pi;
inline constexpr double inv_pi = std::numbers::inv_pi;
inline constexpr double inv_sqrtpi = std::numbers::inv_sqrtpi;
inline constexpr double ln2 = std::numbers::ln2;
inline constexpr double ln10 = std::numbers::ln10;
inline constexpr double sqrt2 = std::numbers::sqrt2;
inline constexpr double sqrt3 = std::numbers::sqrt3;
inline constexpr double inv_sqrt3 = std::numbers::inv_sqrt3;
inline constexpr double egamma = std::numbers::egamma;
inline constexpr double phi = std::numbers::phi;

#if RADIO_CORE_HAVE_HALF
template <>
inline constexpr Half e_v<Half> = 2.718281828459045235360287471352662;
template <>
inline constexpr Half log2e_v<Half> = 1.442695040888963407359924681001892;
template <>
inline constexpr Half log10e_v<Half> = 0.434294481903251827651128918916605;
template <>
inline constexpr Half pi_v<Half> = 3.141592653589793238462643383279502;
template <>
inline constexpr Half inv_pi_v<Half> = 0.318309886183790671537767526745028;
template <>
inline constexpr Half inv_sqrtpi_v<Half> = 0.564189583547756286948079451560772;
template <>
inline constexpr Half ln2_v<Half> = 0.693147180559945309417232121458176;
template <>
inline constexpr Half ln10_v<Half> = 2.302585092994045684017991454684364;
template <>
inline constexpr Half sqrt2_v<Half> = 1.414213562373095048801688724209698;
template <>
inline constexpr Half sqrt3_v<Half> = 1.732050807568877293527446341505872;
template <>
inline constexpr Half inv_sqrt3_v<Half> = 0.577350269189625764509148780501957;
template <>
inline constexpr Half egamma_v<Half> = 0.577215664901532860606512090082402;
template <>
inline constexpr Half phi_v<Half> = 1.618033988749894848204586834365638;
#endif

}  // namespace radio_core::constants
