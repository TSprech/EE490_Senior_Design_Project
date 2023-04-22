#include "nlohmann/json.hpp"
#include "hardware/gpio.h"
#include "GetLine.hpp"
#include "pico/stdio.h"
#include "pico/time.h"

using json = nlohmann::json;

#include <pico/time.h>
#include "RP2040_PWM.hpp"
#include "pico/stdlib.h"

auto pwm_manager = pwm::PWMManager<0, 1>(true);

int main() {
  stdio_init_all();

  constexpr auto led_pin = PICO_DEFAULT_LED_PIN;
  gpio_init(led_pin);
  gpio_set_dir(led_pin, GPIO_OUT);

  gpio_put(led_pin, true);
  sleep_ms(250);
  gpio_put(led_pin, false);
  sleep_ms(250);

  std::array<char, 512> buf{};
  json j;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    auto in_str = GetLine(buf);  // Read in some JSON if it is available
    if (!in_str.empty()) {                       // If it is available, it will be in the string_view that sits within buf
      if (json::accept(in_str)) {                // Check if JSON is valid (if it isn't an exception is raised and core 0 crashes)
        j = json::parse(in_str, nullptr, false); // Parse the JSON in
        if (j.contains("LED")) {                 // Check if LED is sent
          gpio_put(led_pin, j["LED"]);
        }
      }
    }
    sleep_ms(1000);
    j.clear();
    j["Int"] = 24;
    j["String"] = "Hello World!";
    j["Float"] = 3.1415;
    j["Bool"] = true;
    auto str = j.dump();
    puts(str.data());
  }
#pragma clang diagnostic pop
  stdio_init_all();
  pwm_manager.Initialize();
  pwm_manager.DeadBand(1);

  while (true) {
    pwm_manager.Enable();
    pwm_manager.Frequency(100'000);
    for (int i = 0; i < 1'000'000; i += 5'000) {
      pwm_manager.DutyCycle(i);  // 10%
      sleep_us(10);
    }
    pwm_manager.Disable();
    sleep_ms(1);
    pwm_manager.Enable();
    pwm_manager.Frequency(500'000);
    for (int i = 0; i < 1'000'000; i += 5'000) {
      pwm_manager.DutyCycle(i);  // 10%
      sleep_us(10);
    }
    pwm_manager.Disable();
    sleep_ms(10);
  }

  return 0;
}
