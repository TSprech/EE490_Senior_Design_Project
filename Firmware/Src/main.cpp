#include "nlohmann/json.hpp"
#include "hardware/gpio.h"
#include "GetLine.hpp"
#include "pico/stdio.h"
#include "pico/time.h"

using json = nlohmann::json;

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
    sleep_ms(10);
  }
#pragma clang diagnostic pop
}
