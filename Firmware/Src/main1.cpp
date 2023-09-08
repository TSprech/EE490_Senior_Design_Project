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
  constexpr int32_t mppt_duty_step_size = 5000;

  int32_t average_in_voltage = 0;
  int32_t average_in_current = 0;
  int32_t average_out_voltage = 0;
  int32_t average_out_current = 0;

  for (int i = 0; i < 4; ++i) {
    auto [in_voltage, in_current] = sys::ReadADC0();
    auto [out_voltage, out_current] = sys::ReadADC1();
    average_in_voltage += in_voltage;
    average_in_current += in_current;
    average_out_voltage += out_voltage;
    average_out_current += out_current;
  }

  average_in_voltage /= 4;
  average_in_current /= 4;
  average_out_voltage /= 4;
  average_out_current /= 4;

  constexpr auto panel_voltage_offset = 74_mV_i32;
  constexpr auto battery_voltage_offset = -117_mV_i32;

//  constexpr auto positive_panel_current_offset = 0_mA_i32;
//  constexpr auto positive_panel_current_scale = 0.0066F;
//  constexpr auto negative_panel_current_offset = 0_mA_i32;
//  constexpr auto negative_panel_current_scale = 0.0074F;
//
//  constexpr auto positive_battery_current_offset = 15_mA_i32;
//  constexpr auto positive_battery_current_scale = 0.0053F;
//  constexpr auto negative_battery_current_offset = -17_mA_i32;
//  constexpr auto negative_battery_current_scale = 0.0044F;

  constexpr auto positive_panel_current_offset = 0_mA_i32;
  constexpr auto positive_panel_current_scale = 66;
  constexpr auto negative_panel_current_offset = 0_mA_i32;
  constexpr auto negative_panel_current_scale = 74;

  constexpr auto positive_battery_current_offset = 15_mA_i32;
  constexpr auto positive_battery_current_scale = 53;
  constexpr auto negative_battery_current_offset = -17_mA_i32;
  constexpr auto negative_battery_current_scale = 44;


//  auto [average_in_voltage, average_in_current] = sys::ReadADC0();
//  auto [average_out_voltage, average_out_current] = sys::ReadADC1();

  sys::panel_voltage = units::voltage::millivolt_i32_t{average_in_voltage * voltage_gain} - panel_voltage_offset;              // Do integer math to calculate the values
  sys::battery_voltage = units::voltage::millivolt_i32_t{average_out_voltage * voltage_gain} - battery_voltage_offset;              // Do integer math to calculate the values

  sys::panel_current = units::current::milliampere_i32_t{(average_in_current - current_offset) * 5 / 3}; // This nice ratio is the result of simplifying the following:
  sys::battery_current = units::current::milliampere_i32_t{(average_out_current - current_offset) * 5 / 3}; // Gain 200 V/V * 1 / 6mΩ shunt * 0.5 divider ratio


  if (sys::panel_current.Value() <= 0_mA_i32) {
    sys::panel_current = sys::panel_current.Value() + positive_panel_current_offset;
    sys::panel_current = sys::panel_current.Value() + units::current::milliampere_i32_t{sys::panel_current.Value().value() * positive_panel_current_scale / 10'000};
  } else {
    sys::panel_current = sys::panel_current.Value() - negative_panel_current_offset;
    sys::panel_current = sys::panel_current.Value() + units::current::milliampere_i32_t{sys::panel_current.Value().value() * negative_panel_current_scale / 10'000};
  }

  if (sys::battery_current.Value() <= 0_mA_i32) {
    sys::battery_current = sys::battery_current.Value() + positive_battery_current_offset;
    sys::battery_current = sys::battery_current.Value() + units::current::milliampere_i32_t{sys::battery_current.Value().value() * positive_battery_current_scale / 10'000};
  } else {
    sys::battery_current = sys::battery_current.Value() - negative_battery_current_offset;
    sys::battery_current = sys::battery_current.Value() + units::current::milliampere_i32_t{sys::battery_current.Value().value() * negative_battery_current_scale / 10'000};
  }


//  if (sys::panel_current.Value() <= 0_mA_i32) {
//    sys::panel_current = sys::panel_current.Value() + positive_panel_current_offset;
//    sys::panel_current = sys::panel_current.Value() + units::current::milliampere_i32_t{static_cast<int32_t>(static_cast<float>(sys::panel_current.Value().value()) * positive_panel_current_scale)};
//  } else {
//    sys::panel_current = sys::panel_current.Value() - negative_panel_current_offset;
//    sys::panel_current = sys::panel_current.Value() + units::current::milliampere_i32_t{static_cast<int32_t>(static_cast<float>(sys::panel_current.Value().value()) * negative_panel_current_scale)};
//  }
//
//  if (sys::battery_current.Value() <= 0_mA_i32) {
//    sys::battery_current = sys::battery_current.Value() + positive_battery_current_offset;
//    sys::battery_current = sys::battery_current.Value() + units::current::milliampere_i32_t{static_cast<int32_t>(static_cast<float>(sys::battery_current.Value().value()) * positive_battery_current_scale)};
//  } else {
//    sys::battery_current = sys::battery_current.Value() - negative_battery_current_offset;
//    sys::battery_current = sys::battery_current.Value() + units::current::milliampere_i32_t{static_cast<int32_t>(static_cast<float>(sys::battery_current.Value().value()) * negative_battery_current_scale)};
//  }

  sys::panel_power = sys::panel_voltage.Value() * sys::panel_current.Value(); // Then calculate power
  sys::battery_power = sys::battery_voltage.Value() * sys::battery_current.Value(); // Then calculate power

  sys::coulomb_counter_call(sys::battery_current.Value(), 10_ms_i32);
  auto change = sys::mppt_call(sys::panel_voltage.Value(), sys::panel_current.Value());
  if (change) {
    if (sys::panel_power.Value() > 500_mW_i32) {
      FMTDebug("Change: {}\n", change.value_or(0));
      sys::duty = sys::duty.Load() + change.value_or(0) * mppt_duty_step_size;
      sys::pwm_smps.DutyCycle(sys::duty.Load());
    } else {
      FMTDebug("Min power limit: {}\n", change.value_or(0));
      sys::duty = sys::duty.Load() + 1 * mppt_duty_step_size;
      sys::pwm_smps.DutyCycle(sys::duty.Load());
    }
  }
  sys::core_1_utilization.Write(rpp::gpio::Levels::low);
  return true;
}

auto main1() -> void {
  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(500, UpdateADC, nullptr, &timer_adc))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");
  while (true);
}