//
// Created by TSprech on 5/4/2023.
//

#ifndef MODULOSCHEDULER_FIXTURE_HPP
#define MODULOSCHEDULER_FIXTURE_HPP

#include <gtest/gtest.h>

#include "ModuloScheduler.hpp"
#include "TypedUnits.hpp"

using namespace units::literals;

class ModuloSchedulerTest : public ::testing::Test {
 public:
  static inline auto first_num = 0;
  static inline auto second_num = 0;
  static inline auto third_num = 0;
  static inline auto fourth_num = 0;

 protected:
  modsch::ModuloTask first = modsch::ModuloTask{30_ms_u32, [] { ++first_num; }};
  modsch::ModuloTask second = modsch::ModuloTask{60_ms_u32, [] { ++second_num; }};
  modsch::ModuloTask third = modsch::ModuloTask{100_ms_u32, [] { ++third_num; }};
  modsch::ModuloTask fourth = modsch::ModuloTask{200_ms_u32, [] { ++fourth_num; }};

  modsch::ModuloScheduler<4> scheduler = modsch::CreateScheduler(second, fourth, first, third);
};

#endif  // MODULOSCHEDULER_FIXTURE_HPP
