// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

// Matcher for Google Test testing functionality like EXPECT_THAT which operates
// on complex numbers.

#pragma once

#include <tuple>
#include <type_traits>

#include "radio_core/math/complex.h"
#include "radio_core/math/math.h"
#include "radio_core/unittest/mock.h"

namespace radio_core::testing {

namespace internal {

// Implements the polymorphic complex value equality matcher, which matches
// two complex values using ULP-based approximation or, optionally, a
// user-specified epsilon. The real and imaginary parts are compared separately.
template <class RealType, class ErrorType>
class ComplexEqMatcher {
 public:
  // Constructor for ComplexEqMatcher.
  // The matcher's input will be compared with expected. The matcher treats two
  // NANs as equal if nan_eq_nan is true.  Otherwise, under IEEE standards,
  // equality comparisons between NANs will always return false.  We specify a
  // negative max_abs_error_ term to indicate that ULP-based approximation will
  // be used for comparison.
  ComplexEqMatcher(const BaseComplex<RealType>& expected, const bool nan_eq_nan)
      : expected_(expected), nan_eq_nan_(nan_eq_nan), max_abs_error_(-1) {}

  // Constructor that supports a user-specified max_abs_error that will be used
  // for comparison instead of ULP-based approximation.  The max absolute
  // should be non-negative.
  ComplexEqMatcher(const BaseComplex<RealType>& expected,
                   const bool nan_eq_nan,
                   const ErrorType max_abs_error)
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
    Impl(const BaseComplex<RealType>& expected,
         const bool nan_eq_nan,
         const ErrorType max_abs_error)
        : expected_(expected),
          nan_eq_nan_(nan_eq_nan),
          max_abs_error_(max_abs_error) {}

    auto MatchAndExplain(const T actual, MatchResultListener* listener) const
        -> bool override {
      // Compares NaNs first, if nan_eq_nan_ is true.
      if (IsNaN(actual) || IsNaN(expected_)) {
        if (IsNaN(actual) && IsNaN(expected_)) {
          return nan_eq_nan_;
        }
        // One is nan; the other is not nan.
        return false;
      }

      if (!HasMaxAbsError()) {
        // TODO(sergey): Look into supporting half float.

        using FloatingPoint = ::testing::internal::FloatingPoint<RealType>;

        return FloatingPoint(actual.real)
                   .AlmostEquals(FloatingPoint(expected_.real)) &&
               FloatingPoint(actual.imag)
                   .AlmostEquals(FloatingPoint(expected_.imag));
      }

      const BaseComplex<RealType> diff = actual - expected_;
      if (ErrorType(Abs(diff.real)) <= max_abs_error_ &&
          ErrorType(Abs(diff.imag)) <= max_abs_error_) {
        return true;
      }

      if (listener->IsInterested()) {
        *listener << "which is " << diff << " from " << expected_;
      }

      return false;
    }

    void DescribeTo(std::ostream* os) const override {
      // os->precision() returns the previously set precision, which we
      // store to restore the ostream to its original configuration
      // after outputting.
      const std::streamsize old_precision =
          os->precision(std::numeric_limits<RealType>::digits10 + 2);
      if (IsNaN(expected_)) {
        if (nan_eq_nan_) {
          *os << "is NaN";
        } else {
          *os << "never matches";
        }
      } else {
        *os << "is approximately " << expected_;
        if (HasMaxAbsError()) {
          *os << " (absolute error <= " << max_abs_error_ << ")";
        }
      }
      os->precision(old_precision);
    }

    void DescribeNegationTo(std::ostream* os) const override {
      // As before, get original precision.
      const std::streamsize old_precision =
          os->precision(std::numeric_limits<RealType>::digits10 + 2);
      if (IsNaN(expected_)) {
        if (nan_eq_nan_) {
          *os << "isn't NaN";
        } else {
          *os << "is anything";
        }
      } else {
        *os << "isn't approximately " << expected_;
        if (HasMaxAbsError()) {
          *os << " (absolute error > " << max_abs_error_ << ")";
        }
      }
      // Restore original precision.
      os->precision(old_precision);
    }

   private:
    auto HasMaxAbsError() const -> bool { return max_abs_error_ >= 0; }

    const BaseComplex<RealType> expected_;
    const bool nan_eq_nan_;
    // max_abs_error will be used for value comparison when >= 0.
    const ErrorType max_abs_error_;
  };

  // The following 3 type conversion operators allow ComplexEqMatcher to be
  // used as a Matcher<ArgumentType>, a Matcher<const ArgumentType&>, or a
  // Matcher<ArgumentType&>, but nothing else.

  operator Matcher<BaseComplex<RealType>>() const {
    return MakeMatcher(new Impl<BaseComplex<RealType>>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

  operator Matcher<const BaseComplex<RealType>&>() const {
    return MakeMatcher(new Impl<const BaseComplex<RealType>&>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

  operator Matcher<BaseComplex<RealType>&>() const {
    return MakeMatcher(new Impl<BaseComplex<RealType>&>(
        expected_, nan_eq_nan_, max_abs_error_));
  }

 private:
  const BaseComplex<RealType> expected_;
  const bool nan_eq_nan_;
  // max_abs_error will be used for value comparison when >= 0.
  const ErrorType max_abs_error_;
};

// A 2-tuple ("binary") wrapper around ComplexEqMatcher:
// ComplexEq2Matcher(e) matches ComplexEqMatcher(x, e) against y.
template <class RealType>
class ComplexEq2Matcher {
 public:
  ComplexEq2Matcher() { Init(-1, false); }

  explicit ComplexEq2Matcher(const bool nan_eq_nan) { Init(-1, nan_eq_nan); }

  explicit ComplexEq2Matcher(const RealType max_abs_error) {
    Init(max_abs_error, false);
  }

  ComplexEq2Matcher(const RealType max_abs_error, const bool nan_eq_nan) {
    Init(max_abs_error, nan_eq_nan);
  }

  template <typename T1, typename T2>
  operator Matcher<std::tuple<T1, T2>>() const {
    return MakeMatcher(
        new Impl<std::tuple<T1, T2>>(max_abs_error_, nan_eq_nan_));
  }
  template <typename T1, typename T2>
  operator Matcher<const std::tuple<T1, T2>&>() const {
    return MakeMatcher(
        new Impl<const std::tuple<T1, T2>&>(max_abs_error_, nan_eq_nan_));
  }

 private:
  static std::ostream& GetDesc(std::ostream& os) {  // NOLINT
    return os << "an almost-equal pair";
  }

  template <typename Tuple>
  class Impl : public MatcherInterface<Tuple> {
   public:
    Impl(const RealType max_abs_error, const bool nan_eq_nan)
        : max_abs_error_(max_abs_error), nan_eq_nan_(nan_eq_nan) {}

    bool MatchAndExplain(Tuple args,
                         MatchResultListener* listener) const override {
      using TupleType = std::remove_reference_t<Tuple>;
      using LhsType = std::remove_cvref_t<std::tuple_element_t<0, TupleType>>;

      // Expect that the matcher is created against two complex types.
      using ComplexType = LhsType;
      using ComplexScalarType = typename LhsType::value_type;

      if (max_abs_error_ == -1) {
        ComplexEqMatcher<ComplexScalarType, RealType> fm(std::get<0>(args),
                                                         nan_eq_nan_);
        return static_cast<Matcher<ComplexType>>(fm).MatchAndExplain(
            std::get<1>(args), listener);
      } else {
        ComplexEqMatcher<ComplexScalarType, RealType> fm(
            std::get<0>(args), nan_eq_nan_, max_abs_error_);
        return static_cast<Matcher<ComplexType>>(fm).MatchAndExplain(
            std::get<1>(args), listener);
      }
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

  void Init(RealType max_abs_error_val, bool nan_eq_nan_val) {
    max_abs_error_ = max_abs_error_val;
    nan_eq_nan_ = nan_eq_nan_val;
  }

  RealType max_abs_error_;
  bool nan_eq_nan_;
};

}  // namespace internal

// Creates a matcher that matches any complex value approximately equal to
// rhs, up to the specified max absolute error bound, where two NANs are
// considered unequal. The max absolute error bound must be non-negative.
// The matcher check both real and imaginary parts to be within the given
// error.
template <class RealType, class ErrorType>
inline auto ComplexNear(const BaseComplex<RealType>& rhs,
                        const ErrorType max_abs_error) {
  return internal::ComplexEqMatcher<RealType, ErrorType>(
      rhs, false, max_abs_error);
}

// Creates a polymorphic matcher that matches a 2-tuple where
// ComplexNear(first field, max_abs_error) matches the second field.
template <class RealType>
inline auto ComplexNear(const RealType max_abs_error)
    -> internal::ComplexEq2Matcher<RealType> {
  return internal::ComplexEq2Matcher<RealType>(max_abs_error);
}

}  // namespace radio_core::testing
