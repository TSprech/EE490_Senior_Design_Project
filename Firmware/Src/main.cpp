#include "RP2040_PWM.hpp"

#include <pico/time.h>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include "pico/stdlib.h"

/**
 * @brief A constexpr version of pwm_gpio_to_slice_num.
 * @param gpio
 * @returns
 */
inline constexpr auto PWMGPIOToSliceNum(uint gpio) {
  return (gpio >> 1u) & 7u;
}

inline auto CPUFrequency() -> uint32_t {
  return uint32_t{clock_get_hz(clk_sys)};
}


struct PWMManager {
  //  explicit PWMManager(uint8_t slice_number)
  //      : slice_number_(slice_number) {}

  constexpr PWMManager(uint8_t a_pin, uint8_t b_pin)
      : slice_number_(PWMGPIOToSliceNum(a_pin)), a_pin_(a_pin), b_pin_(b_pin) {}

  [[nodiscard]] auto Top() const -> uint32_t {
    return pwm_hw->slice[slice_number_].top;
  }

  auto DeadBand(uint16_t count) {
    //    deadband = count;
    deadband_ = count;
  }

  //  auto DeadBandUs() {
  //    return ;
  //  }

  auto Frequency(uint32_t frequency) {
    auto top = (CPUFrequency() / frequency / 2) - 1;
    pwm_set_wrap(slice_number_, top);
  }

  //  auto SMPSFrequencyKHz() -> uint16_t {
  //    constexpr auto f_sys_khz = 125000000 / 1000;
  //    return 1 / ((Top() + 1) * 2 / f_sys_khz);
  //  }

  auto SimpleDuty(uint16_t duty_cycle) {
    auto top = Top();
    uint16_t a_level = 0;
    uint16_t b_level = 0;
    if (duty_cycle == 0) {
      a_level = 0;
      b_level = top;
    }

    a_level -= deadband_ / 2;
    b_level += deadband_ / 2;

    pwm_set_both_levels(slice_number_, a_level, b_level);
  }

  /**
   * @brief Changes the duty cycle of the A and B channels of the PWM channel.
   * @param duty_cycle The duty cycle represented as a values between 0 and 1.
   */
  static constexpr uint32_t max_duty = 1'000'000;

  // Duty cycle from 0 to 1,000,000 to represent 0 to 1
  bool DutyCycle(uint32_t dutycycle, uint16_t deadband = 1) {
    if (dutycycle > max_duty) [[unlikely]] dutycycle = max_duty;
    uint16_t level = Map<uint32_t>(dutycycle, 0, max_duty, 0, Top());
    uint16_t a_level = 0;
    uint16_t b_level = 0;

    // https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
    auto a_overflow = __builtin_sub_overflow(level, deadband, &a_level);
    auto b_overflow = __builtin_add_overflow(level, deadband, &b_level);

    if (a_overflow || b_overflow || a_level == 0 || b_level == 0 || a_level == Top() || b_level == Top()) {
      a_level = 0;
      b_level = invert_b_ ? UINT16_MAX : 0;
    }

//    printf("A: %s, B: %s\n", a_overflow ? "OVERFLOW" : "", b_overflow ? "OVERFLOW" : "");

//    uint16_t b_level = level; // TODO: Uhhhhh?

//    // This makes sure there isn't a condition where there is no PWM happening (one channel stuck at 100% or 0% duty)
//    if ((a_level == 0 || b_level == 0 || a_level == Top() || b_level == Top())) {
//      a_level = 0;
//      b_level = 0;
//    }

//    printf("Top: %lu, A: %u, B: %u\n", Top(), a_level, b_level);
    pwm_set_both_levels(slice_number_, a_level, b_level);
    return true;
  }

  void Enable() {
    pwm_set_enabled(slice_number_, true);
//    _enabled = true;
  }

  auto Initialize() -> void {
    if (PWMGPIOToSliceNum(a_pin_) != PWMGPIOToSliceNum(b_pin_)) {
//      FMTDebug("A and B PWM pins are not part of the same slice, this means they will not switch synchronously\n");
    }

    // Tell GPIO 0 and 1 they are allocated to the PWM
    gpio_set_function(a_pin_, GPIO_FUNC_PWM);
    gpio_set_function(b_pin_, GPIO_FUNC_PWM);

    gpio_disable_pulls(a_pin_);
    gpio_disable_pulls(b_pin_);

    gpio_set_slew_rate(a_pin_, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(b_pin_, GPIO_SLEW_RATE_FAST);

//    gpio_set_drive_strength(a_pin_, GPIO_DRIVE_STRENGTH_12MA);
//    gpio_set_drive_strength(b_pin_, GPIO_DRIVE_STRENGTH_12MA);

//    pwm_set_wrap(slice_number_, 625 - 1);
    //    //  pwm_set_both_levels(slice, 5000, 1250);
//    pwm_set_both_levels(slice_number_, 295, 335);
    pwm_set_phase_correct(slice_number_, true);
    pwm_set_output_polarity(slice_number_, false, invert_b_);
    //      SMPSFrequencyKHz(100);
    //      SMPSDuty(50);
//    pwm_set_enabled(slice_number_, true);  // Start PWM running
  }

  const uint8_t slice_number_;
  const uint8_t a_pin_;
  const uint8_t b_pin_;
  bool invert_b_ = true;
  uint16_t deadband_ = 24;
};

constexpr uint16_t level = 0;
constexpr uint16_t deadband = 0;
uint16_t a_level = 0;

auto pwm_manager = PWMManager(0, 1);

int main() {
  stdio_init_all();
//  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  pwm_manager.Initialize();
  pwm_manager.Enable();
  pwm_manager.Frequency(100'000);
  while (true) {
    for (int i = 0; i < 1'000'000 ; i+=5'000) {
      pwm_manager.DutyCycle(i); // 10%
      sleep_us(10);
    }
  }

  return 0;
}
