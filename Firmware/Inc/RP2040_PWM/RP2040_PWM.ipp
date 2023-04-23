// TSprech 2023/04/03 13:02:20

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

template <typename T> // Thanks: https://stackoverflow.com/a/5732390
constexpr auto Map(T value, T in_min, T in_max, T out_min, T out_max) -> T {
  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template <uint8_t a_pin, uint8_t b_pin>
constexpr pwm::PWMManager<a_pin, b_pin>::PWMManager(bool invert_b)
    : slice_number_(PWMGPIOToSliceNum(a_pin)), a_pin_(a_pin), b_pin_(b_pin), invert_b_(invert_b) {
  static_assert(PWMGPIOToSliceNum(a_pin) == PWMGPIOToSliceNum(b_pin));  // Make sure A and B pins are part of the same slice
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::Initialize() -> void {
  gpio_set_function(a_pin_, GPIO_FUNC_PWM); // Set the pins to PWM
  gpio_set_function(b_pin_, GPIO_FUNC_PWM);

  gpio_disable_pulls(a_pin_); // Remove any pulls since they will never be used
  gpio_disable_pulls(b_pin_);

  gpio_set_slew_rate(a_pin_, GPIO_SLEW_RATE_FAST); // Switch the PWM as fast as possible
  gpio_set_slew_rate(b_pin_, GPIO_SLEW_RATE_FAST);

  gpio_set_drive_strength(a_pin_, GPIO_DRIVE_STRENGTH_12MA); // Highest drive strength to try to make switching happen as fast as possible
  gpio_set_drive_strength(b_pin_, GPIO_DRIVE_STRENGTH_12MA);

  pwm_set_phase_correct(slice_number_, true); // Centers the PWM and allows symmetric waveforms and deadbands
  pwm_set_output_polarity(slice_number_, false, invert_b_); // Invert the B channel accordingly
}

template <uint8_t a_pin, uint8_t b_pin>
constexpr auto pwm::PWMManager<a_pin, b_pin>::MaxDuty() {
  return max_duty_;
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::DutyCycle(uint32_t dutycycle) -> void {
  if (dutycycle > max_duty_) [[unlikely]] // If a value like 2,000,000 is given
    dutycycle = max_duty_; // Saturate to max duty
  uint16_t level = Map<uint32_t>(dutycycle, 0, max_duty_, 0, Top()); // Map the number range from 0-1,000,000 to 0-top, where top is the
  uint16_t a_level = 0; // Represents A channel wrap value
  uint16_t b_level = 0; // Represents B channel wrap value

  // https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
  auto a_overflow = __builtin_sub_overflow(level, deadband_, &a_level); // Performs subtraction while checking for uint16_t overflow
  auto b_overflow = __builtin_add_overflow(level, deadband_, &b_level); // Performs addition while checking for uint16_t overflow

  // Condition 0 & 1: Checks if either deadband calculation caused overflow, prevents wrap values of, for instance, 5 and 65530 when duty cycles close to 0 or UINT16_MAX are used
  // Condition 2 & 3: Checks if either is 0, in which case the channel does not change level and could cause shoot through
  // Condition 4 & 5: Same as 2 & 3 but opposite polarity
  if (a_overflow || b_overflow || a_level == 0 || b_level == 0 || a_level == Top() || b_level == Top()) {
    a_level = 0;
    b_level = invert_b_ ? Top() : 0; // If B is inverted, then it needs to be set to top to prevent shoot through
  }

  pwm_set_both_levels(slice_number_, a_level, b_level); // Set the wrap value of both A and B channel
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::DeadBand(uint16_t count) -> void {
  deadband_ = count;
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::Frequency(uint32_t frequency) {
  auto top = (CPUFrequency() / frequency / 2) - 1; // This only works for a clock divider of 1
  pwm_set_wrap(slice_number_, top); // Wrap number to set the frequency in terms of clock cycles
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::Enable() -> void {
  pwm_set_both_levels(slice_number_, 0, invert_b_ ? UINT16_MAX : 0);
  pwm_set_enabled(slice_number_, true);
  gpio_set_outover(a_pin_, GPIO_OVERRIDE_NORMAL);
  gpio_set_outover(b_pin_, GPIO_OVERRIDE_NORMAL);
  enabled_ = true;
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::Disable() -> void {
  gpio_set_outover(a_pin_, GPIO_OVERRIDE_LOW);
  gpio_set_outover(b_pin_, invert_b_ ? GPIO_OVERRIDE_LOW : GPIO_OVERRIDE_HIGH);
  pwm_set_both_levels(slice_number_, 0, invert_b_ ? UINT16_MAX : 0);
  pwm_set_enabled(slice_number_, false);
  enabled_ = false;
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::InvertB(bool invert_b) -> void {
  invert_b_ = invert_b;
  pwm_set_output_polarity(slice_number_, false, invert_b_);
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::Enabled() -> bool {
  return enabled_;
}

template <uint8_t a_pin, uint8_t b_pin>
constexpr auto pwm::PWMManager<a_pin, b_pin>::PWMGPIOToSliceNum(uint gpio) {
  return (gpio >> 1u) & 7u;
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::Top() const -> uint32_t {
  return pwm_hw->slice[slice_number_].top;
}

template <uint8_t a_pin, uint8_t b_pin>
auto pwm::PWMManager<a_pin, b_pin>::CPUFrequency() -> uint32_t {
  return uint32_t{clock_get_hz(clk_sys)};
}
