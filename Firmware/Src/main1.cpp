//
// Created by treys on 2023/08/11
//

#include  "main1.hpp"
#include "System.hpp"
#include "pico/time.h"
#include "TypedUnits.hpp"

using namespace units::literals;

auto UpdateADC(repeating_timer_t *rt) -> bool {
  sys::core_1_utilization.Write(rpp::gpio::Levels::high);
  constexpr int32_t voltage_gain = 13;                               // Voltage divider ratio
  constexpr int32_t current_offset = 1'024;                          // mV
  constexpr int32_t mppt_duty_step_size = 20;

//  int32_t average_in_voltage = 0;
//  int32_t average_in_current = 0;
//  int32_t average_out_voltage = 0;
//  int32_t average_out_current = 0;

//  for (int i = 0; i < 4; ++i) {
//    auto [in_voltage, in_current] = sys::ReadADC0();
//    auto [out_voltage, out_current] = sys::ReadADC1();
//    average_in_voltage += in_voltage;
//    average_in_current += in_current;
//    average_out_voltage += out_voltage;
//    average_out_current += out_current;
//  }

//  average_in_voltage /= 4;
//  average_in_current /= 4;
//  average_out_voltage /= 4;
//  average_out_current /= 4;

  auto [average_in_voltage, average_in_current] = sys::ReadADC0();
  auto [average_out_voltage, average_out_current] = sys::ReadADC1();

  sys::panel_voltage = units::voltage::millivolt_i32_t{average_in_voltage * voltage_gain};              // Do integer math to calculate the values
  sys::battery_voltage = units::voltage::millivolt_i32_t{average_out_voltage * voltage_gain};              // Do integer math to calculate the values

  sys::panel_current = units::current::milliampere_i32_t{(average_in_current - current_offset) * 5 / 3}; // This nice ratio is the result of simplifying the following:
  sys::battery_current = units::current::milliampere_i32_t{(average_out_current - current_offset) * 5 / 3}; // Gain 200 V/V * 1 / 6mΩ shunt * 0.5 divider ratio

  sys::panel_power = sys::panel_voltage.Value() * sys::panel_current.Value(); // Then calculate power
  sys::battery_power = sys::battery_voltage.Value() * sys::battery_current.Value(); // Then calculate power

  sys::coulomb_counter_call(sys::battery_current.Value(), 10_ms_i32);
  auto change = sys::mppt_call(sys::panel_voltage.Value(), sys::panel_current.Value());
  sys::duty += change.value_or(0) * mppt_duty_step_size;
  sys::core_1_utilization.Write(rpp::gpio::Levels::low);
  return true;
}

auto main1() -> void {
  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(10, UpdateADC, nullptr, &timer_adc))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");
  while (true);
}