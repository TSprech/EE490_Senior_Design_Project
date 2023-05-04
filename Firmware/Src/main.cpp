#include "nlohmann/json.hpp"
#include "hardware/gpio.h"
#include "GetLine.hpp"
#include "pico/stdio.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include <iostream>

using json = nlohmann::json;

#include <pico/time.h>

#include "FastPID.hpp"
#include "RP2040_PWM.hpp"
#include "TypedUnits.hpp"
#include "pico/stdlib.h"
#include "pico/time.h"

auto pwm_manager = pwm::PWMManager<0, 1>(true);

constexpr float Kp=2, Ki=0.5, Kd=0, Hz=10;
constexpr bool output_signed = false;

constinit pid::FastPID myPID;

int main() {
  stdio_init_all();

  if (!myPID.Initialize(Kp, Ki, Kd, Hz, output_signed)) {
    puts("PID initialization error");
  }

//  auto quant_1 = (units::frequency::hertz_i32_t{82} + static_cast<units::frequency::hertz_i32_t>(units::frequency::hertz_u32_t{32})).value();
//  auto quant_2 = (units::frequency::hertz_i32_t{82} + units::frequency::hertz_i32_t{32}).value();
//  //    auto quant_2 = (units::frequency::hertz_i32_t{32} + units::frequency::kilohertz_u32_t{82}).value();
//  //    auto quant_3 = (units::frequency::hertz_i32_t{32} + units::frequency::kilohertz_u32_t{2}).value();

  constexpr auto led_pin = PICO_DEFAULT_LED_PIN;
  gpio_init(led_pin);
  gpio_set_dir(led_pin, GPIO_OUT);

  gpio_put(led_pin, true);
  sleep_ms(250);
  gpio_put(led_pin, false);
  sleep_ms(250);

//  std::array<char, 512> buf{};
//  json j;
    printf("ADC Example, measuring GPIO26\n");
    sleep_ms(3000);
//    printf("Outmin: %lli, Outmax: %lli, DutyMin: %lu, DutyMax: %lu\n", myPID.outmin_, myPID.outmax_, 0, pwm::PWMManager<0, 1>::MaxDuty());
      adc_init();
      // Make sure GPIO is high-impedance, no pullups etc
      adc_gpio_init(26);
      // Select ADC input 0 (GPIO26)
      adc_select_input(0);

  srand(64);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
      pwm_manager.Initialize();
      pwm_manager.DeadBand(1);
      pwm_manager.Frequency(100'000);
      pwm_manager.Enable();

      const float conversion_factor = 3.3f / (1 << 12);
  while (true) {
      uint32_t sum = 0;
      for (int i = 0; i < 100; ++i) {
        sum += adc_read();
      }
      auto result = sum / 100;
      int16_t millivoltage = static_cast<float>(result) * conversion_factor * 1000.0;
      auto start = get_absolute_time();
      uint16_t output = myPID.Evaluate(2486, millivoltage);
//      auto diff = get_absolute_time()._private_us_since_boot - start._private_us_since_boot;
      auto diff = 0;
      auto duty = Map<int64_t>(output, 0, UINT16_MAX, 0, pwm::PWMManager<0, 1>::MaxDuty());
      pwm_manager.DutyCycle(duty);
      std::cout << "Output: " << output << " Duty: " << duty << " Voltage: " << millivoltage << "mV" << "Time: " << diff << "us\n";
//      printf("Output: %u, Duty: %lu, Raw value: 0x%04lx, voltage: %d mV\n", output, duty, result, millivoltage);
      sleep_ms(98);

//    auto in_str = GetLine(buf);                   // Read in some JSON if it is available
//    if (!in_str.empty()) {                        // If it is available, it will be in the string_view that sits within buf
//      if (json::accept(in_str)) {                 // Check if JSON is valid (if it isn't an exception is raised and core 0 crashes)
//        j = json::parse(in_str, nullptr, false);  // Parse the JSON in
//        if (j.contains("LED")) {                  // Check if LED is sent
//          gpio_put(led_pin, j["LED"]);
//        }
//      }
//    }
//    sleep_ms(1000);
//    j.clear();
//    j["Int"] = 24;
//    j["String"] = "Hello World!";
//    j["Float"] = 3.1415;
//    j["Bool"] = true;
//    auto str = j.dump();
//    puts(str.data());
  }
#pragma clang diagnostic pop
}
//  stdio_init_all();
//  pwm_manager.Initialize();
//  pwm_manager.DeadBand(1);
//
//  while (true) {
//    pwm_manager.Enable();
//    pwm_manager.Frequency(100'000);
//    for (int i = 0; i < 1'000'000; i += 5'000) {
//      pwm_manager.DutyCycle(i);  // 10%
//      sleep_us(10);
//    }
//    pwm_manager.Disable();
//    sleep_ms(1);
//    pwm_manager.Enable();
//    pwm_manager.Frequency(500'000);
//    for (int i = 0; i < 1'000'000; i += 5'000) {
//      pwm_manager.DutyCycle(i);  // 10%
//      sleep_us(10);
//    }
//    pwm_manager.Disable();
//    sleep_ms(10);
//  }
//
//  return 0;
//}
