//
// Created by TSprech on 5/4/2023.
//

#ifndef MODULOSCHEDULER_FIXTURE_HPP
#define MODULOSCHEDULER_FIXTURE_HPP

#include <gtest/gtest.h>

#include "ModuloScheduler.hpp"

class ModuloSchedulerTest : public ::testing::Test {
 protected:
  ModuloScheduler ms = ModuloScheduler();
};

#endif  // MODULOSCHEDULER_FIXTURE_HPP
