//
// Created by treys on 2023/09/02
//

#include "MPPT.hpp"

#include <gtest/gtest.h>

#include "units.h"

class SolarPanel {
  units::voltage::millivolt_u32_t voltage_;
  units::current::milliampere_u32_t current_;

  // Define the "knee" of the IV curve
  units::voltage::millivolt_u32_t knee_voltage_ = units::voltage::millivolt_u32_t(2000);

 public:
  SolarPanel(units::voltage::millivolt_u32_t voltage, units::current::milliampere_u32_t current)
      : voltage_(voltage), current_(current) {}

  units::voltage::millivolt_u32_t GetVoltage() { return voltage_; }
  units::current::milliampere_u32_t GetCurrent() {
    // If voltage is less than "knee", return constant current
    // If voltage is over the "knee", decrease the current linearly with voltage
    return voltage_ < knee_voltage_ ? current_ : current_ - units::current::milliampere_u32_t(voltage_.value() - knee_voltage_.value());
  }

  void IncreaseVoltage(units::voltage::millivolt_u32_t increment) { voltage_ += increment; }
};

class IncrementalConductanceTest : public ::testing::Test {
 protected:
  SolarPanel panel;

  IncrementalConductanceTest()
      : panel(units::voltage::millivolt_u32_t(1500), units::current::milliampere_u32_t(1000)) {}

  void SetUp() override {
    // Generate an initial data point for the delta
    mppt::IncrementalConductance(panel.GetVoltage(), panel.GetCurrent());
  }
};

TEST_F(IncrementalConductanceTest, NegativeDutyCycleStep) {
  // Simulate increasing voltage above the knee
  panel.IncreaseVoltage(units::voltage::millivolt_u32_t(1000));
  EXPECT_LT(panel.GetCurrent(), units::current::milliampere_u32_t(1000));

  auto duty_cycle_step = mppt::IncrementalConductance(panel.GetVoltage(), panel.GetCurrent());

  EXPECT_LT(duty_cycle_step, 0);
}

TEST_F(IncrementalConductanceTest, NoChangeDutyCycleStep) {
  // Simulate no change in voltage and current
  auto duty_cycle_step = mppt::IncrementalConductance(panel.GetVoltage(), panel.GetCurrent());

  EXPECT_EQ(duty_cycle_step, 0);
}

TEST_F(IncrementalConductanceTest, PositiveDutyCycleStep) {
  // Simulate decreasing voltage below the knee
  panel.IncreaseVoltage(units::voltage::millivolt_u32_t(-1000));

  auto duty_cycle_step = mppt::IncrementalConductance(panel.GetVoltage(), panel.GetCurrent());

  EXPECT_GT(duty_cycle_step, 0);
}