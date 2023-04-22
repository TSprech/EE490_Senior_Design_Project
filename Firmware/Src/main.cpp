#include <pico/time.h>

#include <cstdint>
#include <cstdio>
#include <iostream>

#include "RP2040_PWM.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

inline auto CPUFrequency() -> uint32_t {
  return uint32_t{clock_get_hz(clk_sys)};
}

template <uint8_t a_pin, uint8_t b_pin>
struct PWMManager {
  constexpr explicit PWMManager(bool invert_b = false)
      : slice_number_(PWMGPIOToSliceNum(a_pin)), a_pin_(a_pin), b_pin_(b_pin), invert_b_(invert_b) {
    static_assert(PWMGPIOToSliceNum(a_pin) == PWMGPIOToSliceNum(b_pin)); // Make sure A and B pins are part of the same slice
  }

  /**
   * @returns The value of top for the current PWM slice
   */
  [[nodiscard]] auto Top() const -> uint32_t {
    return pwm_hw->slice[slice_number_].top;
  }

  /**
   * @brief Changes the deadband between A and B channels.
   * @param count The number of PWM cycles to delay.
   * @note Count is in PWM cycles, not any time unit, due to the fine resolution required for it. For example, a system running at 125MHz, each cycle is 1/125MHz = 8ns.
   */
  auto DeadBand(uint16_t count) -> void {
    deadband_ = count;
  }

  /**
   * @brief Changes the frequency
   * @param frequency
   * @return
   */
  auto Frequency(uint32_t frequency) {
    auto top = (CPUFrequency() / frequency / 2) - 1;
    pwm_set_wrap(slice_number_, top);
  }

  [[nodiscard]]
  static auto MaxDuty() {
    return max_duty;
  }

  /**
   * @brief Changes the duty cycle of the A and B channels of the PWM channel.
   * @param duty_cycle The duty cycle represented as a values between 0 and 1.
   */
  // Duty cycle from 0 to 1,000,000 to represent 0 to 1
  bool DutyCycle(uint32_t dutycycle) {
    if (dutycycle > max_duty) [[unlikely]]
      dutycycle = max_duty;
    uint16_t level = Map<uint32_t>(dutycycle, 0, max_duty, 0, Top());
    uint16_t a_level = 0;
    uint16_t b_level = 0;

    // https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
    auto a_overflow = __builtin_sub_overflow(level, deadband_, &a_level);
    auto b_overflow = __builtin_add_overflow(level, deadband_, &b_level);

    if (a_overflow || b_overflow || a_level == 0 || b_level == 0 || a_level == Top() || b_level == Top()) {
      a_level = 0;
      b_level = invert_b_ ? UINT16_MAX : 0;
    }

    pwm_set_both_levels(slice_number_, a_level, b_level);
    return true;
  }

  void Enable() {
    InvertB(invert_b_);
    pwm_set_both_levels(slice_number_, 0, invert_b_ ? UINT16_MAX : 0);
    pwm_set_enabled(slice_number_, true);
    gpio_set_outover(a_pin_, GPIO_OVERRIDE_NORMAL);
    gpio_set_outover(b_pin_, GPIO_OVERRIDE_NORMAL);
    enabled_ = true;
  }

  void Disable() {
    InvertB(false);
    gpio_set_outover(a_pin_, GPIO_OVERRIDE_LOW);
    gpio_set_outover(b_pin_, invert_b_ ? GPIO_OVERRIDE_LOW : GPIO_OVERRIDE_HIGH);
    pwm_set_both_levels(slice_number_, 0, invert_b_ ? UINT16_MAX : 0);
    pwm_set_enabled(slice_number_, false);
    enabled_ = false;
  }

  auto InvertB(bool invert_b) -> void {
    invert_b_ = invert_b;
    pwm_set_output_polarity(slice_number_, false, invert_b_);
  }

  bool Enabled() {
    return enabled_;
  }

  auto Initialize() -> void {
    gpio_set_function(a_pin_, GPIO_FUNC_PWM);
    gpio_set_function(b_pin_, GPIO_FUNC_PWM);

    gpio_disable_pulls(a_pin_);
    gpio_disable_pulls(b_pin_);

    gpio_set_slew_rate(a_pin_, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(b_pin_, GPIO_SLEW_RATE_FAST);

    gpio_set_drive_strength(a_pin_, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(b_pin_, GPIO_DRIVE_STRENGTH_12MA);

    pwm_set_phase_correct(slice_number_, true);
    pwm_set_output_polarity(slice_number_, false, invert_b_);
  }

 private:
  /**
   * @brief A constexpr version of pwm_gpio_to_slice_num.
   * @param gpio The GPIO pin for which the slice is unknown.
   * @returns The slice number that corresponds to the GPIO pin.
   */
  static constexpr auto PWMGPIOToSliceNum(uint gpio) {
    return (gpio >> 1u) & 7u;
  }

  static constexpr uint32_t max_duty = 1'000'000;
  const uint8_t slice_number_;
  const uint8_t a_pin_;
  const uint8_t b_pin_;
  bool invert_b_ = true;
  bool enabled_ = false;
  uint16_t deadband_ = 50;
};

auto pwm_manager = PWMManager<0, 1>(true);

int main() {
  stdio_init_all();
  pwm_manager.Initialize();
  pwm_manager.DeadBand(1);

  while (true) {
    pwm_manager.InvertB(true);
    pwm_manager.Enable();
    pwm_manager.Frequency(100'000);
    for (int i = 0; i < 1'000'000; i += 5'000) {
      pwm_manager.DutyCycle(i);  // 10%
      sleep_us(10);
    }
    pwm_manager.Disable();
    sleep_ms(1);
    pwm_manager.InvertB(false);
    pwm_manager.Enable();
    for (int i = 0; i < 1'000'000; i += 5'000) {
      pwm_manager.DutyCycle(i);  // 10%
      sleep_us(10);
    }
    pwm_manager.Disable();
//    pwm_manager.Enable();
//    pwm_manager.Frequency(500'000);
//    for (int i = 0; i < 1'000'000; i += 5'000) {
//      pwm_manager.DutyCycle(i);  // 10%
//      sleep_us(10);
//    }
//    pwm_manager.Disable();
    sleep_ms(10);
  }

  return 0;
}
