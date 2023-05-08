//
// Created by TSprech on 5/4/2023
//

#include <gtest/gtest.h>

#include "ModuloScheduler_Fixture.hpp"

TEST_F(ModuloSchedulerTest, Period) {
  EXPECT_EQ(scheduler.Period(), 30_ms_u32);
}

TEST_F(ModuloSchedulerTest, BasicInitialization) {
  EXPECT_EQ(scheduler.Tasks()[0].modulo_number, 1);
  EXPECT_EQ(scheduler.Tasks()[1].modulo_number, 2);
  EXPECT_EQ(scheduler.Tasks()[2].modulo_number, 3);
  EXPECT_EQ(scheduler.Tasks()[3].modulo_number, 6);
}

#define CHECK_TASK_NUMBERS(num0, num1, num2, num3) \
  EXPECT_EQ(ModuloSchedulerTest::first_num, num0);\
  EXPECT_EQ(ModuloSchedulerTest::second_num, num1);\
  EXPECT_EQ(ModuloSchedulerTest::third_num, num2);\
  EXPECT_EQ(ModuloSchedulerTest::fourth_num, num3);\
  static_assert(true)

TEST_F(ModuloSchedulerTest, BasicExecution) {
  CHECK_TASK_NUMBERS(0, 0, 0, 0); // Initial state

  scheduler.Execute();
  CHECK_TASK_NUMBERS(1, 1, 1, 1); // 0 All execute the first time scheduler starts up

  scheduler.Execute();
  CHECK_TASK_NUMBERS(2, 1, 1, 1); // 1

  scheduler.Execute();
  CHECK_TASK_NUMBERS(3, 2, 1, 1); // 2

  scheduler.Execute();
  CHECK_TASK_NUMBERS(4, 2, 2, 1); // 3

  scheduler.Execute();
  CHECK_TASK_NUMBERS(5, 3, 2, 1); // 3

  scheduler.Execute();
  CHECK_TASK_NUMBERS(6, 3, 2, 1); // 5

  scheduler.Execute();
  CHECK_TASK_NUMBERS(7, 4, 3, 2); // 6

  scheduler.Execute();
  CHECK_TASK_NUMBERS(8, 4, 3, 2); // 7
}
