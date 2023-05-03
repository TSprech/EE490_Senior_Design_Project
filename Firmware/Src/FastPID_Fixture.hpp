//
// Created by treys on 5/3/2023.
//

#ifndef FASTPID_FIXTURE_HPP
#define FASTPID_FIXTURE_HPP

#include <gtest/gtest.h>
#include "FastPID.hpp"

class FastPIDTest : public ::testing::Test {
 protected:
  pid::FastPID pid = pid::FastPID();
};

#endif  // FASTPID_FIXTURE_HPP
