//
// Created by treys on 5/24/2023.
//

#ifndef EE490_FIRMWARE_ADC_HPP
#define EE490_FIRMWARE_ADC_HPP

#include <utility>

#include "hardware/adc.h"

namespace rpp::adc {
  /**
   * @brief Constants for all the ADC pins on the RP2040.
   */
  enum class Pins : uint8_t {
    adc_0 = 0, // GPIO 26
    adc_1 = 1, // GPIO 27
    adc_2 = 2, // GPIO 28
    adc_3 = 3, // GPIO 29
    adc_temp = 4
  };

  /**
   * @brief Describes a ADC pin and allows for easily interacting with it.
   */
  struct ADC {
    Pins pin; /**< The actual ADC pin being modified by the methods */
    inline static uint16_t adc_ref_mV_ = 3'300;
    inline static bool adc_system_initialized_ = false;

    auto Init() -> ADC& {
      constexpr uint32_t adc_gpio_offset_ = 26;
      if (!adc_system_initialized_)
        adc_init();
      if (pin != Pins::adc_temp) [[likely]]
        adc_gpio_init(adc_gpio_offset_ + std::to_underlying(pin));
      else [[unlikely]]
        adc_set_temp_sensor_enabled(true);
      return *this;
    }

    auto DeInit() -> ADC& {
      if (pin == Pins::adc_temp)
        adc_set_temp_sensor_enabled(false);
      return *this;
    }

    auto ReferenceVoltage(uint16_t voltage_mV) -> ADC& {
      adc_ref_mV_ = voltage_mV;
      return *this;
    }

    [[nodiscard]] auto ReferenceVoltagemV() const -> uint16_t {
      return adc_ref_mV_;
    }

    [[nodiscard]] auto ReadmV() const -> uint32_t {
      adc_select_input(std::to_underlying(pin));
      const auto raw = static_cast<uint32_t>(adc_read());
      auto voltage_scaled_raw = static_cast<uint32_t>(adc_ref_mV_) * raw;
      uint32_t result_mV = voltage_scaled_raw / (1 << 12);
      return result_mV;
    }

    [[nodiscard]] auto ReadTemperaturemC() const -> uint32_t {
      const auto raw_mV = ReadmV();
      return 27000 - ((raw_mV * 1000 - 706000) / 1721 / 1000);
    }
  };

  // Global instances for each ADC pin
  inline constinit ADC adc_26{Pins::adc_0};
  inline constinit ADC adc_27{Pins::adc_1};
  inline constinit ADC adc_28{Pins::adc_2};
  inline constinit ADC adc_29{Pins::adc_3};
  inline constinit ADC temperature{Pins::adc_temp};
}  // namespace rpp::adc

#endif  //EE490_FIRMWARE_ADC_HPP
