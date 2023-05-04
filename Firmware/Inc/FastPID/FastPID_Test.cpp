//
// Created by TSprech on 5/3/2023
//

#include <gtest/gtest.h>
#include "FastPID_Fixture.hpp"

TEST_F(FastPIDTest, ValidInitialize) {
  auto value_error = pid.Initialize(1, 2, 3, 10);
  EXPECT_TRUE(value_error);
}

TEST_F(FastPIDTest, InvalidInitialize) {
  {
    auto value_error = pid.Initialize(999999, 2, 3, 10);
    EXPECT_FALSE(value_error);
    EXPECT_EQ(value_error.error(), pid::FastPID::FastPIDError::invalid_range);
  }
  {
    auto value_error = pid.Initialize(1, -2, 3, 10);
    EXPECT_FALSE(value_error);
    EXPECT_EQ(value_error.error(), pid::FastPID::FastPIDError::invalid_range);
  }
}

TEST_F(FastPIDTest, HighSetPointSigned) {
  auto value_error = pid.Initialize(10, 2, 3, 10, true);
  auto output = pid.Evaluate(10, 2);
  EXPECT_GT(output, 0);
}

TEST_F(FastPIDTest, LowSetPointSigned) {
  auto value_error = pid.Initialize(10, 2, 3, 10, true);
  auto output = pid.Evaluate(2, 10);
  EXPECT_LT(output, 0);
}

TEST_F(FastPIDTest, HighSetPointUnsigned) {
  auto value_error = pid.Initialize(10, 2, 3, 10, false);
  auto output = pid.Evaluate(10, 2);
  EXPECT_GT(output, 0);
}

TEST_F(FastPIDTest, LowSetPointUnsigned) {
  auto value_error = pid.Initialize(10, 2, 3, 10, false);
  auto output = pid.Evaluate(2, 10);
  EXPECT_EQ(output, 0);
}
