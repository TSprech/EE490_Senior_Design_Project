// TSprech 2023/04/02

#ifndef RP2040_PWM_H
#define RP2040_PWM_H

#include <cstdint>

namespace pwm {
  /**
 * @brief Manages a single PWM slice to control the A and B channel for SMPS operation.
 * @tparam a_pin The GPIO number representing A pin.
 * @tparam b_pin The GPIO number representing B pin.
 */
  template <uint8_t a_pin, uint8_t b_pin>
  struct PWMManager {
    /**
   * @brief Constructor for a single PWM slice manager.
   * @param invert_b If the B channel of the slice should be inverted.
   */
    constexpr explicit PWMManager(bool invert_b = false);

    /**
   * @brief Configures the PWM pins and enables phase correct for the PWM slice.
   */
    auto Initialize() -> void;

    /**
   * @returns The integer value which represents 100% duty cycle for the @ref DutyCycle method.
   */
    [[nodiscard]] static constexpr auto MaxDuty();

    /**
   * @brief Changes the duty cycle of the A and B channels of the PWM channel.
   * @param duty_cycle The duty cycle represented as a values between 0 and 1,000,000 representing 0 to 1 range.
   */
    auto DutyCycle(uint32_t dutycycle) -> void;

    /**
   * @brief Changes the deadband between A and B channels.
   * @param count The number of PWM cycles to delay.
   * @note Count is in PWM cycles, not any time unit, due to the fine resolution required for it. For example, a system running at 125MHz, each cycle is 1/125MHz = 8ns.
   */
    auto DeadBand(uint16_t count) -> void;

    /**
   * @brief Changes the frequency
   * @param frequency
   * @return
   */
    auto Frequency(uint32_t frequency);

    /**
   * @brief Enables both channel's PWM output.
   */
    auto Enable() -> void;

    /**
   * @brief Disables both channel's PWM output.
   */
    auto Disable() -> void;

    /**
   * @brief Changes whether the B channel is inverted.
   * @param invert_b True to invert the channel, false to return it to non-inverted state.
   */
    auto InvertB(bool invert_b) -> void;

    /**
   * @returns Whether the PWM slice is enabled.
   */
    auto Enabled() -> bool;

   private:
    /**
   * @brief A constexpr version of pwm_gpio_to_slice_num.
   * @param gpio The GPIO pin for which the slice is unknown.
   * @returns The slice number that corresponds to the GPIO pin.
   */
    static constexpr auto PWMGPIOToSliceNum(uint32_t gpio);

    /**
   * @returns The value of top for the current PWM slice
   */
    [[nodiscard]] auto Top() const -> uint32_t;

    /**
   * @returns The current CPU frequency in Hertz.
   */
    static auto CPUFrequency() -> uint32_t;

    static constexpr uint32_t max_duty_ = 1'000'000;
    const uint8_t slice_number_;
    const uint8_t a_pin_;
    const uint8_t b_pin_;
    bool invert_b_ = true;
    bool enabled_ = false;
    uint16_t deadband_ = 50;
  };
}

#include "RP2040_PWM.ipp"

#endif  // RP2040_PWM_H
