// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/result.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

enum class Error {
  kUnkown,
  kGenericError,
  kImpossibleError,
};

TEST(Result, Construct) {
  // Empty.
  {
    Result<int, Error> result(Error::kGenericError);
    EXPECT_FALSE(result.Ok());
    EXPECT_EQ(result.GetError(), Error::kGenericError);
    EXPECT_FALSE(result.HasValue());
  }

  // Initialize from rvalue.
  {
    Result<int, Error> result(20);
    EXPECT_TRUE(result.Ok());
    EXPECT_TRUE(result.HasValue());
    EXPECT_EQ(result.GetValue(), 20);
  }

  // Initialize from rvalue with an error.
  {
    Result<int, Error> result(20, Error::kGenericError);
    EXPECT_FALSE(result.Ok());
    EXPECT_EQ(result.GetError(), Error::kGenericError);
    EXPECT_TRUE(result.HasValue());
    EXPECT_EQ(result.GetValue(), 20);
  }
}

TEST(Result, ValueAccessViaOperator) {
  class Value {
   public:
    Value(int value) : int_value(value) {}

    int int_value;
  };

  {
    Result<Value, Error> result(10);
    EXPECT_EQ(result->int_value, 10);
    EXPECT_EQ((*result).int_value, 10);
  }

  {
    const Result<Value, Error> result(10);
    EXPECT_EQ(result->int_value, 10);
    EXPECT_EQ((*result).int_value, 10);
  }
}

}  // namespace radio_core
