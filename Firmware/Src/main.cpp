#include <pico/time.h>
#include "RP2040_PWM.hpp"
#include "pico/stdlib.h"

auto pwm_manager = pwm::PWMManager<0, 1>(true);

int main() {
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
