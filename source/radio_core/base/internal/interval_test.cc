// Copyright (c) 2022 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/base/interval.h"

#include "radio_core/unittest/test.h"

namespace radio_core {

TEST(Interval, compare) {
  EXPECT_TRUE(Interval(10, 20) == Interval(10, 20));
  EXPECT_FALSE(Interval(10, 20) != Interval(10, 20));

  EXPECT_FALSE(Interval(10, 20) == Interval(30, 40));
  EXPECT_TRUE(Interval(10, 20) != Interval(30, 40));
}

TEST(Interval, IsEmpty) {
  EXPECT_TRUE(Interval<int>().IsEmpty());
  EXPECT_FALSE(Interval<int>(10, 20).IsEmpty());
}

TEST(Interval, Contains) {
  EXPECT_FALSE(Interval<int>().Contains(0));
  EXPECT_FALSE(Interval<int>().Contains(1));
  EXPECT_FALSE(Interval<int>().Contains(-1));

  EXPECT_FALSE(Interval<int>(10, 20).Contains(0));
  EXPECT_TRUE(Interval<int>(10, 20).Contains(10));
  EXPECT_TRUE(Interval<int>(10, 20).Contains(15));
  EXPECT_TRUE(Interval<int>(10, 20).Contains(20));
  EXPECT_FALSE(Interval<int>(10, 20).Contains(30));
}

TEST(Interval, Intersection) {
  EXPECT_TRUE(Interval<int>().Intersection(Interval<int>()).IsEmpty());
  EXPECT_TRUE(Interval<int>(1, 2).Intersection(Interval<int>()).IsEmpty());
  EXPECT_TRUE(Interval<int>().Intersection(Interval<int>(1, 2)).IsEmpty());

  EXPECT_TRUE(Interval(1, 2).Intersection(Interval(3, 4)).IsEmpty());

  EXPECT_EQ(Interval(1, 8).Intersection(Interval(2, 3)), Interval(2, 3));
  EXPECT_EQ(Interval(1, 5).Intersection(Interval(2, 8)), Interval(2, 5));
  EXPECT_EQ(Interval(2, 8).Intersection(Interval(1, 5)), Interval(2, 5));
}

TEST(Interval, Expanded) {
  EXPECT_EQ(Interval<int>().Expanded(3), Interval<int>());
  EXPECT_EQ(Interval<int>(10, 20).Expanded(3), Interval<int>(7, 23));
}

TEST(Interval, Diameter) {
  EXPECT_EQ(Interval<int>(10, 20).Diameter(), 10);
  EXPECT_EQ(Interval<int>(-10, 20).Diameter(), 30);
}

}  // namespace radio_core
