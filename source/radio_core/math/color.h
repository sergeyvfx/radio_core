// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#include "radio_core/math/math.h"

namespace radio_core {

////////////////////////////////////////////////////////////////////////////////
// Common utilities.

// Declaration of converter of a color channel value from the type From to the
// type To.
// Takes care of clamping and quantization when needed.
template <class From, class To>
struct ColorChannelConverter;

template <>
struct ColorChannelConverter<uint8_t, float> {
  static inline auto Convert(const uint8_t value) -> float {
    return float(value) / 255;
  }
};

template <>
struct ColorChannelConverter<float, uint8_t> {
  static inline auto Convert(const float value) -> float {
    return Saturate(value) * 255;
  }
};

////////////////////////////////////////////////////////////////////////////////
// 1-component color.

template <class T>
class Color1 {
 public:
  static inline constexpr int N = 1;
  using value_type = T;

  constexpr Color1() = default;
  constexpr Color1(const T new_value) : value(new_value) {}

  inline auto operator==(const Color1& other) const -> bool {
    return value == other.value;
  }
  inline auto operator!=(const Color1& other) const -> bool {
    return !(*this == other);
  }

  // Unary mathematics.
  // NOTE: Does not perform any saturation or overflow tests.
  inline auto operator+=(const Color1& rhs) -> Color1& {
    value += rhs.value;
    return *this;
  }
  inline auto operator-=(const Color1& rhs) -> Color1& {
    value -= rhs.value;
    return *this;
  }
  template <class Scalar>
  inline auto operator*=(const Scalar& rhs) -> Color1& {
    value *= rhs;
    return *this;
  }
  template <class Scalar>
  inline auto operator/=(const Scalar& rhs) -> Color1& {
    value /= rhs;
    return *this;
  }

  // Helper to access the same color type but with different value type.
  template <class OtherValueType>
  struct ConvertValueType {
    using ColorType = Color1<OtherValueType>;
  };

  // Convert this color to the other color type.
  // Perform conversion of the channel type with proper quantization and
  // clamping when needed.
  template <class Other>
  inline auto ConvertTo() const -> Other {
    static_assert(N == Other::N);

    return Other(
        ColorChannelConverter<T, typename Other::value_type>::Convert(value));
  }

  T value;
};

template <class T>
inline auto operator+(const Color1<T>& lhs, const Color1<T>& rhs) -> Color1<T> {
  Color1<T> result = lhs;
  result += rhs;
  return result;
}

template <class T>
inline auto operator-(const Color1<T>& lhs, const Color1<T>& rhs) -> Color1<T> {
  Color1<T> result = lhs;
  result -= rhs;
  return result;
}

template <class T, class Scalar>
inline auto operator*(const Color1<T>& lhs, const Scalar& rhs) -> Color1<T> {
  Color1<T> result = lhs;
  result *= rhs;
  return result;
}

template <class T, class Scalar>
inline auto operator/(const Color1<T>& lhs, const Scalar& rhs) -> Color1<T> {
  Color1<T> result = lhs;
  result /= rhs;
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// 3-component color.

template <class T>
class Color3 {
 public:
  static inline constexpr int N = 3;
  using value_type = T;

  constexpr Color3() = default;
  constexpr Color3(const T r, const T g, const T b) {
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
  }
  explicit constexpr Color3(const T v) : Color3(v, v, v) {}

  inline constexpr auto operator==(const Color3& other) const -> bool {
    return rgb.r == other.rgb.r && rgb.g == other.rgb.g && rgb.b == other.rgb.b;
  }
  inline constexpr auto operator!=(const Color3& other) const -> bool {
    return !(*this == other);
  }

  // Unary mathematics.
  // NOTE: Does not perform any saturation or overflow tests.
  inline auto operator+=(const Color3& rhs) -> Color3& {
    rgb.r += rhs.rgb.r;
    rgb.g += rhs.rgb.g;
    rgb.b += rhs.rgb.b;
    return *this;
  }
  inline auto operator-=(const Color3& rhs) -> Color3& {
    rgb.r -= rhs.rgb.r;
    rgb.g -= rhs.rgb.g;
    rgb.b -= rhs.rgb.b;
    return *this;
  }
  template <class Scalar>
  inline auto operator*=(const Scalar& rhs) -> Color3& {
    rgb.r *= rhs;
    rgb.g *= rhs;
    rgb.b *= rhs;
    return *this;
  }
  template <class Scalar>
  inline auto operator/=(const Scalar& rhs) -> Color3& {
    rgb.r /= rhs;
    rgb.g /= rhs;
    rgb.b /= rhs;
    return *this;
  }

  // Helper to access the same color type but with different value type.
  template <class OtherValueType>
  struct ConvertValueType {
    using ColorType = Color3<OtherValueType>;
  };

  // Convert this color to the other color type.
  // Perform conversion of the channel type with proper quantization and
  // clamping when needed.
  template <class Other>
  inline auto ConvertTo() const -> Other {
    static_assert(N == Other::N);

    return Other(
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgb.r),
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgb.g),
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgb.b));
  }

  union {
    // Channels of RGB type of colorspace.
    struct {
      T r, g, b;
    } rgb;

    // Channels of YCbCr type of colorspace.
    struct {
      T y, cb, cr;
    } ycc;
  };

  static_assert(sizeof(rgb) == sizeof(T) * 3);
  static_assert(sizeof(ycc) == sizeof(T) * 3);
};

template <class T>
inline auto operator+(const Color3<T>& lhs, const Color3<T>& rhs) -> Color3<T> {
  Color3<T> result = lhs;
  result += rhs;
  return result;
}

template <class T>
inline auto operator-(const Color3<T>& lhs, const Color3<T>& rhs) -> Color3<T> {
  Color3<T> result = lhs;
  result -= rhs;
  return result;
}

template <class T, class Scalar>
inline auto operator*(const Color3<T>& lhs, const Scalar& rhs) -> Color3<T> {
  Color3<T> result = lhs;
  result *= rhs;
  return result;
}

template <class T, class Scalar>
inline auto operator/(const Color3<T>& lhs, const Scalar& rhs) -> Color3<T> {
  Color3<T> result = lhs;
  result /= rhs;
  return result;
}

template <class T>
inline auto Saturate(const Color3<T>& color) -> Color3<T> {
  return Color3<T>(
      Saturate(color.rgb.r), Saturate(color.rgb.g), Saturate(color.rgb.b));
}

template <class T>
inline auto Lerp(const Color3<T>& v0, const Color3<T>& v1, const float t)
    -> Color3<T> {
  // TODO(sergey): Vectorize on platforms where possible.
  return Color3<T>(Lerp(v0.rgb.r, v1.rgb.r, t),
                   Lerp(v0.rgb.g, v1.rgb.g, t),
                   Lerp(v0.rgb.b, v1.rgb.b, t));
}

////////////////////////////////////////////////////////////////////////////////
// 4-component color.

template <class T>
class Color4 {
 public:
  static inline constexpr int N = 4;
  using value_type = T;

  constexpr Color4() = default;
  constexpr Color4(const T r, const T g, const T b, const T a) {
    rgba.r = r;
    rgba.g = g;
    rgba.b = b;
    rgba.a = a;
  }
  explicit constexpr Color4(const T v) : Color4(v, v, v, v) {}

  inline constexpr auto operator==(const Color4& other) const -> bool {
    return rgba.r == other.rgba.r && rgba.g == other.rgba.g &&
           rgba.b == other.rgba.b && rgba.a == other.rgba.a;
  }
  inline constexpr auto operator!=(const Color4& other) const -> bool {
    return !(*this == other);
  }

  // Unary mathematics.
  // NOTE: Does not perform any saturation or overflow tests.
  inline auto operator+=(const Color4& rhs) -> Color4& {
    rgba.r += rhs.rgba.r;
    rgba.g += rhs.rgba.g;
    rgba.b += rhs.rgba.b;
    rgba.a += rhs.rgba.a;
    return *this;
  }
  inline auto operator-=(const Color4& rhs) -> Color4& {
    rgba.r -= rhs.rgba.r;
    rgba.g -= rhs.rgba.g;
    rgba.b -= rhs.rgba.b;
    rgba.a -= rhs.rgba.a;
    return *this;
  }
  template <class Scalar>
  inline auto operator*=(const Scalar& rhs) -> Color4& {
    rgba.r *= rhs;
    rgba.g *= rhs;
    rgba.b *= rhs;
    rgba.a *= rhs;
    return *this;
  }
  template <class Scalar>
  inline auto operator/=(const Scalar& rhs) -> Color4& {
    rgba.r /= rhs;
    rgba.g /= rhs;
    rgba.b /= rhs;
    rgba.a /= rhs;
    return *this;
  }

  // Helper to access the same color type but with different value type.
  template <class OtherValueType>
  struct ConvertValueType {
    using ColorType = Color4<OtherValueType>;
  };

  // Convert this color to the other color type.
  // Perform conversion of the channel type with proper quantization and
  // clamping when needed.
  template <class Other>
  inline auto ConvertTo() const -> Other {
    static_assert(N == Other::N);

    return Other(
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgba.r),
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgba.g),
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgba.b),
        ColorChannelConverter<T, typename Other::value_type>::Convert(rgba.a));
  }

  union {
    struct {
      T r, g, b, a;
    } rgba;

    struct {
      T y, cb, cr, a;
    } ycca;
  };

  static_assert(sizeof(rgba) == sizeof(T) * 4);
  static_assert(sizeof(ycca) == sizeof(T) * 4);
};

template <class T>
inline auto operator+(const Color4<T>& lhs, const Color4<T>& rhs) -> Color4<T> {
  Color4<T> result = lhs;
  result += rhs;
  return result;
}

template <class T>
inline auto operator-(const Color4<T>& lhs, const Color4<T>& rhs) -> Color4<T> {
  Color4<T> result = lhs;
  result -= rhs;
  return result;
}

template <class T, class Scalar>
inline auto operator*(const Color4<T>& lhs, const Scalar& rhs) -> Color4<T> {
  Color4<T> result = lhs;
  result *= rhs;
  return result;
}

template <class T, class Scalar>
inline auto operator/(const Color4<T>& lhs, const Scalar& rhs) -> Color4<T> {
  Color4<T> result = lhs;
  result /= rhs;
  return result;
}

template <class T>
inline auto Saturate(const Color4<T>& color) -> Color4<T> {
  // TODO(sergey): Vectorize on platforms where possible.
  return Color4<T>(Saturate(color.rgba.r),
                   Saturate(color.rgba.g),
                   Saturate(color.rgba.b),
                   Saturate(color.rgba.a));
}

template <class T, class FactorType>
inline auto Lerp(const Color4<T>& v0, const Color4<T>& v1, const FactorType t)
    -> Color4<T> {
  // TODO(sergey): Vectorize on platforms where possible.
  return Color4<T>(Lerp(v0.rgba.r, v1.rgba.r, t),
                   Lerp(v0.rgba.g, v1.rgba.g, t),
                   Lerp(v0.rgba.b, v1.rgba.b, t),
                   Lerp(v0.rgba.a, v1.rgba.a, t));
}

////////////////////////////////////////////////////////////////////////////////
// Common color types.

using Color1f = Color1<float>;
using Color1ub = Color1<uint8_t>;

using Color3f = Color3<float>;
using Color3ub = Color3<uint8_t>;

using Color4f = Color4<float>;
using Color4ub = Color4<uint8_t>;

}  // namespace radio_core
