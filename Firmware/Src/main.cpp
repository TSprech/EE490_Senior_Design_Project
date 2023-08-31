//#include "pico/stdlib.h"
//
//int main() {
//  stdio_init_all();
//
//  gpio_init(16);
//  gpio_init(17);
//  gpio_set_dir(16, GPIO_OUT);
//  gpio_set_dir(17, GPIO_OUT);
//  gpio_put(16, true);
//  gpio_put(17, true);
//  while(true);
//}


#include <algorithm>
#include <charconv>
#include <functional>

#include "ADC.hpp"
#include "GetLine.hpp"
#include "States.hpp"
#include "System.hpp"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "main1.hpp"

auto PrintData(repeating_timer_t *rt) -> bool {
  FMTDebug("VPV={:.2f}, IPV={:.2f}, PPV={:.2f}, VBAT={:.2f}, IBAT={:.2f}, PBAT={:.2f}, QBAT={}, SOC={:.2f}, RS={}, D={:.2f}\n",
           static_cast<float>(sys::panel_voltage.Value().value()) / 1000.0F, static_cast<float>(sys::panel_current.Value().value()) / 1000.0F,
           static_cast<float>(sys::panel_power.Value().value()) / 1000.0F, static_cast<float>(sys::battery_voltage.Value().value()) / 1000.0F,
           static_cast<float>(sys::battery_current.Value().value()) / 1000.0F, static_cast<float>(sys::battery_power.Value().value()) / 1000.0F,
           sys::bm.Charge().value(), static_cast<float>(sys::bm.ChargeState()),
           RunStateFSM::current_state(), static_cast<float>(sys::duty.Load()) / 10'000.0F);
  return true;
}

auto DispatchRunState(uint8_t state) -> void {
  switch (state) {
    case 0:
      RunStateFSM::dispatch(RS0());
      break;
    case 1:
      RunStateFSM::dispatch(RS1());
      break;
    case 2:
      RunStateFSM::dispatch(RS2());
      break;
    case 3:
      RunStateFSM::dispatch(RS3());
      break;
    default:
      break;
  }
}

auto main() -> int {
  patom::PseudoAtomicInit();
  stdio_init_all();

  sys::Initialize();

//  sys::pwm_a.Write(rpp::gpio::Levels::high);
//  sys::pwm_b.Write(rpp::gpio::Levels::high);

  multicore_launch_core1(main1);

  RunStateFSM::start();

  rpp::adc::adc_26.Init().ReferenceVoltage(2048);  // Configure all the ADC pins
  rpp::adc::adc_27.Init().ReferenceVoltage(2048);
  rpp::adc::adc_28.Init().ReferenceVoltage(2048);
  rpp::adc::adc_29.Init().ReferenceVoltage(2048);

  repeating_timer_t timer_print;                                       // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(100, PrintData, nullptr, &timer_print))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  while (true) {
    static std::array<char, 512> buf{};                 // String buffer
    auto in_str = GetLine(buf, '\r');                   // Read in some serial data if available
    if (!in_str.empty()) {                              // If it is available, it will be in the string_view that sits within buf
      std::string_view numbers = {&buf[1], buf.end()};  // Grab everything after the first character as a single string which contains a number
      int argument;                                     // Variable which the number sent over serial will be read into

      auto [ptr, error] = std::from_chars(numbers.data(), numbers.end(), argument);  // Attempt to extract a number from the string

      if (error == std::errc()) [[likely]]  // If there were no issues
        FMTDebug("Attempting to change {} to {}\n", buf[0], argument);
      else [[unlikely]]  // There was some problem with parsing the number
        FMTDebug("Invalid argument, trouble parsing number\n");

      switch (buf[0]) {  // Decided what to do with the number based on the first character given
        case ('S'): {    // Change run state
          static auto run_state = 0;
          run_state = (argument == std::clamp(argument, 0, 3)) ? argument : run_state;  // If the argument is within the valid range, change the run state to the argument, otherwise do nothing
          DispatchRunState(run_state);
          break;
        }
        case ('D'): {  // Change duty cycle
          sys::duty = (argument == std::clamp(argument, 1, 99)) ? argument * 10'000 : sys::duty.Load();  // If the argument is within the valid range, change the duty to the argument, otherwise do nothing
          FMTDebug("DUTY VAR: {}", sys::duty.Load());
          sys::pwm_smps.DutyCycle(sys::duty.Load());
          break;
        }
        case ('T'): {
          DispatchRunState(0);
          break;
        }
      }
    }
  }
}