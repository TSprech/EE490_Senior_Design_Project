//
// Created by treys on 2023/08/11
//

#include "main1.hpp"
#include "System.hpp"
#include "pico/time.h"
#include "TypedUnits.hpp"

using namespace units::literals;

//printf("Voltage: %dmV Current: %dmA\n", (data1 >> 1) * 13, static_cast<int16_t>((data2 >> 1) - 1024) * 5 / 3);

auto UpdateADC(repeating_timer_t *rt) -> bool {
//  rpp::gpio::gpio_2.Write(rpp::gpio::Levels::high); // TODO: Reenable pin toggling
  constexpr int32_t voltage_gain = 13;                               // Voltage divider ratio
  constexpr int32_t current_offset = 1'024;                          // mV
  constexpr int32_t mppt_duty_step_size = 20;

  auto [in_voltage, in_current] = sys123::ReadADC0();
  auto [out_voltage, out_current] = sys123::ReadADC1();

  sys123::panel_voltage = units::voltage::millivolt_i32_t{in_voltage * voltage_gain};              // Do integer math to calculate the values
  sys123::battery_voltage = units::voltage::millivolt_i32_t{out_voltage * voltage_gain};              // Do integer math to calculate the values

  sys123::panel_current = units::current::milliampere_i32_t{(in_current - current_offset) * 5 / 3}; // This nice ratio is the result of simplifying the following:
  sys123::battery_current = units::current::milliampere_i32_t{(out_current - current_offset) * 5 / 3}; // Gain 200 V/V * 1 / 6mΩ shunt * 0.5 divider ratio

  sys123::panel_power = sys123::panel_voltage.Value() * sys123::panel_current.Value(); // Then calculate power
  sys123::battery_power = sys123::battery_voltage.Value() * sys123::battery_current.Value(); // Then calculate power

  sys123::coulomb_counter_call(sys123::battery_current.Value(), 10_ms_i32);
  auto change = sys123::mppt_call(sys123::panel_voltage.Value(), sys123::panel_current.Value());
  sys123::duty += change.value_or(0) * mppt_duty_step_size;
//  rpp::gpio::gpio_2.Write(rpp::gpio::Levels::low);
  return true;
}

auto main1() -> void {
  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(10, UpdateADC, nullptr, &timer_adc))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");
  while (true);
}