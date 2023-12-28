// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Matcher for Google Test testing functionality like EXPECT_THAT which operates
// on vectorized types (such as Float4).

#pragma once

#include <tuple>
#include <type_traits>

#include "radio_core/base/unroll.h"
#include "radio_core/math/math.h"
#include "radio_core/math/vectorized_float_type.h"
#include "radio_core/unittest/mock.h"

namespace radio_core::testing {

namespace internal {

// Implements the polymorphic vectorized floating point  value equality matcher
// (element-wise), which matches two complex values using ULP-based
// approximation or, optionally, a user-specified epsilon.
template <class RealType, int N>
class VectorizedEqMatcher {
 public:
  // Constructor for VectorizedEqMatcher.
  // The matcher's input will be compared with expected. The matcher treats two
  // NANs as equal if nan_eq_nan is true.  Otherwise, under IEEE standards,
  // equality comparisons between NANs will always return false.  We specify a
  // negative max_abs_error_ term to indicate that ULP-based approximation will
  // be used for comparison.
  VectorizedEqMatcher(const VectorizedFloatType<RealType, N>& expected,
                      const bool nan_eq_nan)
      : expected_(expected), nan_eq_nan_(nan_eq_nan), max_abs_error_(-1) {}

  // Constructor that supports a user-specified max_abs_error that will be used
  // for comparison instead of ULP-based approximation.  The max absolute
  // should be non-negative.
  VectorizedEqMatcher(const VectorizedFloatType<RealType, N>& expected,
                      const bool nan_eq_nan,
                      const RealType max_abs_error)
      : expected_(expected),
        nan_eq_nan_(nan_eq_nan),
        max_abs_error_(max_abs_error) {
    GTEST_CHECK_(max_abs_error >= 0)
        << ", where max_abs_error is" << max_abs_error;
  }

  // Implements complex values equality matcher as a Matcher<T>.
  template <class T>
  class Impl : public MatcherInterface<T> {
   public:
    Impl(const VectorizedFloatType<RealType, N>& expected,
         const bool nan_eq_nan,
         const RealType max_abs_error)
        : expected_(expected),
          nan_eq_nan_(nan_eq_nan),
          max_abs_error_(max_abs_error) {}

    auto MatchAndExplain(const T actual, MatchResultListener* listener) const
        -> bool override {
      bool did_match = true;
      Unroll<N>([&](const auto i) {
        did_match &= MatchElement(actual.template Extract<i>(),
                                  expected_.template Extract<i>());
      });

      if (did_match) {
        return true;
      }

      if (listener->IsInterested()) {
        *listener << "which is " << (actual - expected_) << " from "
                  << expected_;
      }

      return false;
    }

    void DescribeTo(std::ostream* os) const override {
      // os->precision() returns the previously set precision, which we
      // store to restore the ostream to its original configuration
      // after outputting.
      const std::streamsize old_precision =
          os->precision(std::numeric_limits<RealType>::digits10 + 2);
      *os << "is approximately " << expected_;
      if (HasMaxAbsError()) {
        *os << " (absolute error <= " << max_abs_error_ << ")";
      }
      os->precision(old_precision);
    }

    void DescribeNegationTo(std::ostream* os) const override {
      // As before, get original precision.
      const std::streamsize old_precision =
          os->precision(std::numeric_limits<RealType>::digits10 + 2);
      *os << "isn't approximately " << expected_;
      if (HasMaxAbsError()) {
        *os << " (absolute error > " << max_abs_error_ << ")";
      }
      // Restore original precision.
      os->precision(old_precision);
    }

   private:
    auto HasMaxAbsError() const -> bool { return max_abs_error_ >= 0; }

    auto MatchElement(const RealType actual, const RealType expected) const
        -> bool {
      // Compares NaNs first, if nan_eq_nan_ is true.
      if (IsNaN(actual) || IsNaN(expected)) {
        if (IsNaN(actual) && IsNaN(expected)) {
          return nan_eq_nan_;
        }
        // One is nan; the other is not nan.
        return false;
      }

      if (!HasMaxAbsError()) {
        using FloatingPoint = ::testing::internal::FloatingPoint<RealType>;

        return FloatingPoint(actual).AlmostEquals(FloatingPoint(expected));
      }

      // We perform an equality check so that inf will match inf, regardless
      // of error bounds.  If the result of value - expected_ would result in
      // overflow or if either value is inf, the default result is infinity,
      // which should only match if max_abs_error_ is also infinity.
      if (actual == expected) {
        return true;
      }

      if (Abs(actual - expected) <= max_abs_error_) {
        return true;
      }

      return false;
    }

    const VectorizedFloatType<RealType, N> expected_;
    const bool nan_eq_nan_;
    // max_abs_error will be used for value comparison when >= 0.
    const RealType max_abs_error_;
  };

  // The following 3 type conversion operators allow VectorizedEqMatcher to be
  // used as a Matcher<ArgumentType>, a Matcher<const ArgumentType&>, or a
  // Matcher<ArgumentType&>, but nothing else.

  operator Matcher<VectorizedFloatType<RealType, N>>() const {
    return MakeMatcher(new Impl<VectorizedFloatType<RealType, N>>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

  operator Matcher<const VectorizedFloatType<RealType, N>&>() const {
    return MakeMatcher(new Impl<const VectorizedFloatType<RealType, N>&>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

  operator Matcher<VectorizedFloatType<RealType, N>&>() const {
    return MakeMatcher(new Impl<VectorizedFloatType<RealType, N>&>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

  // A shortcut to static_cast<Matcher<VectorizedType>>(eq_matcher).
  //
  // This is a workaround for compilation error with MSVC
  //     error C2440: 'static_cast': cannot convert from
  //         'radio_core::testing::internal::VectorizedEqMatcher<float,4>' to
  //         'testing::Matcher<radio_core::Float4>'
  // which happens in the VectorizedEq2Matcher.
  auto CastToMatcher() -> Matcher<VectorizedFloatType<RealType, N>> {
    return MakeMatcher(new Impl<VectorizedFloatType<RealType, N>>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

 private:
  const VectorizedFloatType<RealType, N> expected_;
  const bool nan_eq_nan_;
  // max_abs_error will be used for value comparison when >= 0.
  const RealType max_abs_error_;
};

// A 2-tuple ("binary") wrapper around VectorizedEqMatcher:
// VectorizedEq2Matcher() matches (x, y) by matching VectorizedEqMatcher(x,
// false) against y, and VectorizedEq2Matcher(e) matches VectorizedEqMatcher(x,
// false, e) against y. The former implements "Eq", the latter "Near". At
// present, there is no version that compares NaNs as equal.
template <class RealType>
class VectorizedEq2Matcher {
 public:
  VectorizedEq2Matcher() { Init(-1, false); }

  explicit VectorizedEq2Matcher(const bool nan_eq_nan) { Init(-1, nan_eq_nan); }

  explicit VectorizedEq2Matcher(const RealType max_abs_error) {
    Init(max_abs_error, false);
  }

  VectorizedEq2Matcher(const RealType max_abs_error, bool nan_eq_nan) {
    Init(max_abs_error, nan_eq_nan);
  }

  template <typename T1, typename T2>
  operator Matcher<::std::tuple<T1, T2>>() const {
    return MakeMatcher(
        new Impl<::std::tuple<T1, T2>>(max_abs_error_, nan_eq_nan_));
  }
  template <typename T1, typename T2>
  operator Matcher<const ::std::tuple<T1, T2>&>() const {
    return MakeMatcher(
        new Impl<const ::std::tuple<T1, T2>&>(max_abs_error_, nan_eq_nan_));
  }

 private:
  static auto GetDesc(std::ostream& os) -> std::ostream& {  // NOLINT
    return os << "an almost-equal pair";
  }

  template <class Tuple>
  class Impl : public MatcherInterface<Tuple> {
    using TupleType = std::remove_reference_t<Tuple>;

    using LhsType = typename std::tuple_element<0, TupleType>::type;
    using RhsType = typename std::tuple_element<1, TupleType>::type;

    using MatcherType = Matcher<std::remove_cvref_t<RhsType>>;

   public:
    Impl(const RealType max_abs_error, const bool nan_eq_nan)
        : max_abs_error_(max_abs_error), nan_eq_nan_(nan_eq_nan) {}

    auto MatchAndExplain(Tuple args, MatchResultListener* listener) const
        -> bool override {
      if (max_abs_error_ == -1) {
        VectorizedEqMatcher fm(std::get<0>(args), nan_eq_nan_);
        return fm.CastToMatcher().MatchAndExplain(std::get<1>(args), listener);
      }

      VectorizedEqMatcher fm(std::get<0>(args), nan_eq_nan_, max_abs_error_);
      return fm.CastToMatcher().MatchAndExplain(std::get<1>(args), listener);
    }
    void DescribeTo(std::ostream* os) const override {
      *os << "are " << GetDesc;
    }
    void DescribeNegationTo(std::ostream* os) const override {
      *os << "aren't " << GetDesc;
    }

   private:
    RealType max_abs_error_;
    const bool nan_eq_nan_;
  };

  void Init(const RealType max_abs_error_val, const bool nan_eq_nan_val) {
    max_abs_error_ = max_abs_error_val;
    nan_eq_nan_ = nan_eq_nan_val;
  }

  RealType max_abs_error_;
  bool nan_eq_nan_;
};

}  // namespace internal

// Creates a matcher that matches any floating point vectorized value
// approximately equal to rhs (element-wise), up to the specified max absolute
// error bound, where two NANs are considered unequal. The max absolute error
// bound must be non-negative.
template <class RealType, int N>
inline auto VectorizedNear(const VectorizedFloatType<RealType, N>& rhs,
                           const RealType max_abs_error) {
  return internal::VectorizedEqMatcher<RealType, N>(rhs, false, max_abs_error);
}

// Creates a polymorphic matcher that matches a 2-tuple where
// VectorizedNear(first field, max_abs_error) matches the second field.
template <class RealType>
inline auto VectorizedNear(const RealType max_abs_error) {
  return internal::VectorizedEq2Matcher<RealType>(max_abs_error);
}

}  // namespace radio_core::testing
