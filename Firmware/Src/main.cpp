//#include <iostream>
//
//#include "GetLine.hpp"
//#include "ModuloScheduler.hpp"
//#include "hardware/adc.h"
//#include "hardware/gpio.h"
//#include "nlohmann/json.hpp"
//#include "pico/stdio.h"
//#include "pico/time.h"
//
//using json = nlohmann::json;
//
//using namespace units::literals;
//
//#include <pico/time.h>
//
//#include "DMA.hpp"
//#include "FastPID.hpp"
//#include "GPIO.hpp"
//#include "RP2040_PWM.hpp"
//#include "TypedUnits.hpp"
//#include "hardware/i2c.h"
//#include "pico/stdlib.h"
//#include "pico/time.h"
//using namespace rpp;
//
//auto pwm_manager = pwm::PWMManager<0, 1>(true);
//
//constexpr float Kp = 2, Ki = 0.5, Kd = 0, Hz = 10;
//constexpr bool output_signed = false;
//
//constinit pid::FastPID myPID;
//
//int main() {
//  stdio_init_all();
//
//  int32_t destination = 0;
//  auto channel = dma::DMA();
//  if (channel.Claim()) channel.Init().Trigger(dma::Triggers::pio_0_rx_0).TransferSize(dma::TransferSizes::s_32_bit).WriteAddress(&destination).HighPriority(dma::States::enabled).Enable(dma::States::enabled);
//
//  gpio::gpio_0.Init().Direction(gpio::Directions::output).Pull(gpio::Pulls::disable).SlewRate(gpio::SlewRates::fast).DriveStrength(gpio::DriveStrengths::ds_12mA).Write(gpio::Levels::low);
//  if (!myPID.Initialize(Kp, Ki, Kd, Hz, output_signed)) {
//    puts("PID initialization error");
//  }
//
//  //  auto quant_1 = (units::frequency::hertz_i32_t{82} + static_cast<units::frequency::hertz_i32_t>(units::frequency::hertz_u32_t{32})).value();
//  //  auto quant_2 = (units::frequency::hertz_i32_t{82} + units::frequency::hertz_i32_t{32}).value();
//  //  //    auto quant_2 = (units::frequency::hertz_i32_t{32} + units::frequency::kilohertz_u32_t{82}).value();
//  //  //    auto quant_3 = (units::frequency::hertz_i32_t{32} + units::frequency::kilohertz_u32_t{2}).value();
//
//  constexpr auto led_pin = PICO_DEFAULT_LED_PIN;
//  gpio_init(led_pin);
//  gpio_set_dir(led_pin, GPIO_OUT);
//
//  gpio_put(led_pin, true);
//  sleep_ms(250);
//  gpio_put(led_pin, false);
//  sleep_ms(250);
//
//  //  std::array<char, 512> buf{};
//  //  json j;
//  printf("ADC Example, measuring GPIO26\n");
//  sleep_ms(3000);
//  //    printf("Outmin: %lli, Outmax: %lli, DutyMin: %lu, DutyMax: %lu\n", myPID.outmin_, myPID.outmax_, 0, pwm::PWMManager<0, 1>::MaxDuty());
//  adc_init();
//  // Make sure GPIO is high-impedance, no pullups etc
//  adc_gpio_init(26);
//  // Select ADC input 0 (GPIO26)
//  adc_select_input(0);
//
//  srand(64);
//
//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "EndlessLoop"
//  pwm_manager.Initialize();
//  pwm_manager.DeadBand(1);
//  pwm_manager.Frequency(100'000);
//  pwm_manager.Enable();
//
//  const float conversion_factor = 3.3f / (1 << 12);
//  while (true) {
//    uint32_t sum = 0;
//    for (int i = 0; i < 100; ++i) {
//      sum += adc_read();
//    }
//    auto result = sum / 100;
//    int16_t millivoltage = static_cast<float>(result) * conversion_factor * 1000.0;
//    auto start = get_absolute_time();
//    uint16_t output = myPID.Evaluate(2486, millivoltage);
//    //      auto diff = get_absolute_time()._private_us_since_boot - start._private_us_since_boot;
//    auto diff = 0;
//    auto duty = Map<int64_t>(output, 0, UINT16_MAX, 0, pwm::PWMManager<0, 1>::MaxDuty());
//    pwm_manager.DutyCycle(duty);
//    std::cout << "Output: " << output << " Duty: " << duty << " Voltage: " << millivoltage << "mV"
//              << "Time: " << diff << "us\n";
//    //      printf("Output: %u, Duty: %lu, Raw value: 0x%04lx, voltage: %d mV\n", output, duty, result, millivoltage);
//    sleep_ms(98);
//
//    //    auto in_str = GetLine(buf);                   // Read in some JSON if it is available
//    //    if (!in_str.empty()) {                        // If it is available, it will be in the string_view that sits within buf
//    //      if (json::accept(in_str)) {                 // Check if JSON is valid (if it isn't an exception is raised and core 0 crashes)
//    //        j = json::parse(in_str, nullptr, false);  // Parse the JSON in
//    //        if (j.contains("LED")) {                  // Check if LED is sent
//    //          gpio_put(led_pin, j["LED"]);
//    //        }
//    //      }
//    //    }
//    //    sleep_ms(1000);
//    //    j.clear();
//    //    j["Int"] = 24;
//    //    j["String"] = "Hello World!";
//    //    j["Float"] = 3.1415;
//    //    j["Bool"] = true;
//    //    auto str = j.dump();
//    //    puts(str.data());
//  }
//#pragma clang diagnostic pop
//}
////  stdio_init_all();
////  pwm_manager.Initialize();
////  pwm_manager.DeadBand(1);
////
////  while (true) {
////    pwm_manager.Enable();
////    pwm_manager.Frequency(100'000);
////    for (int i = 0; i < 1'000'000; i += 5'000) {
////      pwm_manager.DutyCycle(i);  // 10%
////      sleep_us(10);
////    }
////    pwm_manager.Disable();
////    sleep_ms(1);
////    pwm_manager.Enable();
////    pwm_manager.Frequency(500'000);
////    for (int i = 0; i < 1'000'000; i += 5'000) {
////      pwm_manager.DutyCycle(i);  // 10%
////      sleep_us(10);
////    }
////    pwm_manager.Disable();
////    sleep_ms(10);
////  }
////
////  return 0;
////}

#include <algorithm>
#include <charconv>

#include "ADC.hpp"
#include "GetLine.hpp"
#include "RP2040_PWM.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "pico/stdlib.h"

auto pwm_manager = pwm::PWMManager<0, 1>(true);

inline auto UARTPrint(std::string_view str) -> void {
  for (auto letter : str) putc(letter, stdout);
}

inline auto UARTVPrint(fmt::string_view format_str, fmt::format_args args) -> void {
  fmt::memory_buffer buffer;
  fmt::detail::vformat_to(buffer, format_str, args);
  UARTPrint({buffer.data(), buffer.size()});
}

template <typename... T>
inline auto FMTDebug(fmt::format_string<T...> fmt, T&&... args) -> void {
  const auto& vargs = fmt::make_format_args(args...);
  UARTVPrint(fmt, vargs);
}

auto random_float = [] { return static_cast<float>(rand()) / static_cast<float>(rand()); };

uint32_t in_mV = 0;
uint32_t in_mA = 0;
uint32_t out_mV = 0;
uint32_t out_mA = 0;
uint32_t in_mW = 0;
uint32_t out_mW = 0;
uint8_t run_state = 0;
uint8_t duty = 1;

auto PrintData(repeating_timer_t* rt) -> bool {
  FMTDebug("VPV={}, IPV={}, PPV={}, VBAT={}, IBAT={}, PBAT={}, QBAT={}, SOC={}, RS={}, D={}\n", static_cast<float>(in_mV) / 1000.0F, static_cast<float>(in_mA) / 1000.0F, static_cast<float>(in_mW) / 1000.0F, static_cast<float>(out_mV) / 1000.0F, static_cast<float>(out_mA) / 1000.0F, static_cast<float>(out_mW) / 1000.0F, random_float(), random_float(), run_state, duty);
  return true;
}

auto UpdateADC(repeating_timer_t* rt) -> bool {
  constexpr uint32_t voltage_gain = 13; // Voltage divider ratio
  constexpr uint32_t current_gain = 200; // Current amp gain
  constexpr uint32_t shunt_mOhm = 6; // Shunt resistance
  auto adc_26_mV = rpp::adc::adc_26.ReadmV(); // Read all the ADC channels
  auto adc_27_mV = rpp::adc::adc_27.ReadmV();
  auto adc_28_mV = rpp::adc::adc_28.ReadmV();
  auto adc_29_mV = rpp::adc::adc_29.ReadmV();
  in_mV = adc_26_mV * voltage_gain; // Do integer math to calculate the values
  in_mA = adc_27_mV * 1000 / current_gain / shunt_mOhm;
  out_mV = adc_28_mV * voltage_gain;
  out_mA = adc_29_mV * 1000 / current_gain / shunt_mOhm;
  in_mW = in_mV * in_mA / 1000; // Then calculate power
  out_mW = out_mV * out_mA / 1000;
  return true;
}

auto main() -> int {
  stdio_init_all();
  rpp::adc::adc_26.Init().ReferenceVoltage(3300);  // Configure all the ADC pins
  rpp::adc::adc_27.Init().ReferenceVoltage(3300);
  rpp::adc::adc_28.Init().ReferenceVoltage(3300);
  rpp::adc::adc_29.Init().ReferenceVoltage(3300);

  repeating_timer_t timer_print;                                       // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(100, PrintData, nullptr, &timer_print))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(10, UpdateADC, nullptr, &timer_adc))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  srand(64);
  pwm_manager.Initialize().Frequency(10'000).Divider(100).Enable().DutyCycle(500'000);
  std::array<char, 512> buf{};  // String buffer

  while (true) {
    auto in_str = GetLine(buf, '\r');                   // Read in some serial data if available
    if (!in_str.empty()) {                              // If it is available, it will be in the string_view that sits within buf
      std::string_view numbers = {&buf[1], buf.end()};  // Grab everything after the first character as a single string which contains a number
      int argument;                                     // Variable which the number sent over serial will be read into

      auto [ptr, error] = std::from_chars(numbers.data(), numbers.end(), argument);  // Attempt to extract a number from the string

      if (error == std::errc()) [[likely]]  // If there were no issues
        FMTDebug("Attempting to change {} to {}\n", buf[0], argument);
      else [[unlikely]]  // There was some problem with parsing the number
        FMTDebug("Invalid argument, trouble parsing number\n");

      switch (buf[0]) {                                                                 // Decided what to do with the number based on the first character given
        case ('S'): {                                                                   // Change run state
          run_state = (argument == std::clamp(argument, 0, 3)) ? argument : run_state;  // If the argument is within the valid range, change the run state to the argument, otherwise do nothing
          break;
        }
        case ('D'): {                                                          // Change duty cycle
          duty = (argument == std::clamp(argument, 1, 99)) ? argument : duty;  // If the argument is within the valid range, change the duty to the argument, otherwise do nothing
          break;
        }
      }
    }
  }
}