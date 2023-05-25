//
// Created by treys on 5/24/2023.
//

#ifndef EE490_FIRMWARE_GPIO_HPP
#define EE490_FIRMWARE_GPIO_HPP

#include <utility>

#include "hardware/gpio.h"

namespace rpp::gpio {
  /**
   * @brief Constants for all the GPIO pins on the RP2040.
   */
  enum class Pins : uint8_t {
    gpio_0 = 0,
    gpio_1 = 1,
    gpio_2 = 2,
    gpio_3 = 3,
    gpio_4 = 4,
    gpio_5 = 5,
    gpio_6 = 6,
    gpio_7 = 7,
    gpio_8 = 8,
    gpio_9 = 9,
    gpio_10 = 10,
    gpio_11 = 11,
    gpio_12 = 12,
    gpio_13 = 13,
    gpio_14 = 14,
    gpio_15 = 15,
    gpio_16 = 16,
    gpio_17 = 17,
    gpio_18 = 18,
    gpio_19 = 19,
    gpio_20 = 20,
    gpio_21 = 21,
    gpio_22 = 22,
    gpio_23 = 23,
    gpio_24 = 24,
    gpio_25 = 25,
    gpio_26 = 26,
    gpio_27 = 27,
    gpio_28 = 28,
    gpio_29 = 29
  };

  /**
   * @brief Constants for the GPIO direction, determines which pins are inputs and outputs.
   */
  enum class Directions : bool {
    input = GPIO_IN,
    output = GPIO_OUT
  };

  /**
   * @brief Constants for the alternate functions a pin can have.
   */
  enum class Functions : uint8_t {
    xip = GPIO_FUNC_XIP,
    spi = GPIO_FUNC_SPI,
    uart = GPIO_FUNC_UART,
    i2c = GPIO_FUNC_I2C,
    pwm = GPIO_FUNC_PWM,
    sio = GPIO_FUNC_SIO,
    pio0 = GPIO_FUNC_PIO0,
    pio1 = GPIO_FUNC_PIO1,
    gpck = GPIO_FUNC_GPCK,
    usb = GPIO_FUNC_USB,
    null = GPIO_FUNC_NULL,
  };

  /**
   * @brief Constants for the different internal pull configurations each pin can be in.
   */
  enum class Pulls : uint8_t {
    disable = 0b00, /**< No pull up or down */
    up = 0b01,
    down = 0b10,
    bus_keep = 0b11 /**< Pulled up and down, weak pull to whatever the current state of the GPIO is */
  };

  /**
   * @brief Constants describing the different pin events that will trigger an interrupt.
   */
  enum class IRQTriggers : uint8_t {
    level_low = GPIO_IRQ_LEVEL_LOW,
    level_high = GPIO_IRQ_LEVEL_HIGH,
    edge_falling = GPIO_IRQ_EDGE_FALL,
    edge_rising = GPIO_IRQ_EDGE_RISE,
  };

  /**
   * @brief Constants for what type of underlying state should be overriden.
   */
  enum class OverrideTypes {
    irq,
    output,
    input,
    output_enable
  };

  /**
   * @brief Constants describing how the given pin will be overriden.
   */
  enum class Overrides {
    none = GPIO_OVERRIDE_NORMAL,    /**< peripheral signal selected via \ref gpio_set_function */
    invert = GPIO_OVERRIDE_INVERT,  /**< invert peripheral signal selected via \ref gpio_set_function */
    low = GPIO_OVERRIDE_LOW,        /**< drive low/disable output */
    high = GPIO_OVERRIDE_HIGH,      /**< drive high/enable output */
  };

  /**
   * @brief Constants for if slew rate limiting should be used.
   */
  enum class SlewRates : bool {
    slow = GPIO_SLEW_RATE_SLOW,  /**< Slew rate limiting enabled */
    fast = GPIO_SLEW_RATE_FAST   /**< Slew rate limiting disabled */
  };

  /**
   * @brief Constants for the drive strength of the GPIO pin in mA.
   */
  enum class DriveStrengths {
    ds_2mA = GPIO_DRIVE_STRENGTH_2MA,   /**< 2 mA nominal drive strength */
    ds_4mA = GPIO_DRIVE_STRENGTH_4MA,   /**< 4 mA nominal drive strength */
    ds_8mA = GPIO_DRIVE_STRENGTH_8MA,   /**< 8 mA nominal drive strength */
    ds_12mA = GPIO_DRIVE_STRENGTH_12MA  /**< 12 mA nominal drive strength */
  };

  enum class States : bool {
    disabled = false,
    enabled = true
  };

  /**\
   * @brief Constants describing the two states an output pin can be set to.
   */
  enum class Levels : bool {
    low = false,
    high = true
  };

  /**
   * @brief Describes a GPIO pin and allows for easily interacting with it.
   */
  struct GPIO {
    Pins pin; /**< The actual GPIO pin being modified by the methods */

    auto Init() -> GPIO& {
      gpio_init(std::to_underlying(pin));
      return *this;
    }

    auto DeInit() -> GPIO& {
      gpio_deinit(std::to_underlying(pin));
      return *this;
    }

    auto Direction(Directions direction) -> GPIO& {
      gpio_set_dir(std::to_underlying(pin), std::to_underlying(direction));
      return *this;
    }

    [[nodiscard]] auto Direction() const -> Directions {
      return static_cast<Directions>(gpio_get_dir(std::to_underlying(pin)));
    }

    auto Function(Functions function) -> GPIO& {
      gpio_set_function(std::to_underlying(pin), static_cast<enum gpio_function>(function));
      return *this;
    }

    [[nodiscard]] auto Function() const -> enum Functions {
      return static_cast<enum Functions>(gpio_get_function(std::to_underlying(pin)));
    }

    auto Pull(Pulls pull) -> GPIO& {
      gpio_set_pulls(std::to_underlying(pin), std::to_underlying(pull) & 0b01, std::to_underlying(pull) & 0b10);
      return *this;
    }

    [[nodiscard]] auto Pull() const -> enum Pulls {
      auto is_pulled_up = gpio_is_pulled_up(std::to_underlying(pin));
      auto is_pulled_down = gpio_is_pulled_down(std::to_underlying(pin)) << 1;  // Shift 1 to offset for the following and
      return static_cast<enum Pulls>(is_pulled_down | is_pulled_up);
    }

    auto Override(OverrideTypes type, Overrides override) -> GPIO& {
      switch (type) {
        case OverrideTypes::irq:
          gpio_set_irqover(std::to_underlying(pin), static_cast<enum gpio_override>(override));
          break;
        case OverrideTypes::output:
          gpio_set_outover(std::to_underlying(pin), static_cast<enum gpio_override>(override));
          break;
        case OverrideTypes::input:
          gpio_set_inover(std::to_underlying(pin), static_cast<enum gpio_override>(override));
          break;
        case OverrideTypes::output_enable:
          gpio_set_oeover(std::to_underlying(pin), static_cast<enum gpio_override>(override));
          break;
      }
      return *this;
    }

    auto Schmitt(States enable) -> GPIO& {
      gpio_set_input_hysteresis_enabled(std::to_underlying(pin), std::to_underlying(enable));
      return *this;
    }

    [[nodiscard]] auto Schmitt() const -> enum States {
      return static_cast<enum States>(gpio_is_input_hysteresis_enabled(std::to_underlying(pin)));
    }

    auto SlewRate(SlewRates slew_rate) -> GPIO& {
      gpio_set_slew_rate(std::to_underlying(pin), static_cast<gpio_slew_rate>(slew_rate));
      return *this;
    }

    [[nodiscard]] auto SlewRate() const -> enum SlewRates {
      return static_cast<enum SlewRates>(gpio_get_slew_rate(std::to_underlying(pin)));
    }

    auto DriveStrength(DriveStrengths drive_strength) -> GPIO& {
      gpio_set_drive_strength(std::to_underlying(pin), static_cast<gpio_drive_strength>(drive_strength));
      return *this;
    }

    [[nodiscard]] auto DriveStrength() const -> enum DriveStrengths {
      return static_cast<enum DriveStrengths>(gpio_get_drive_strength(std::to_underlying(pin)));
    }

    auto Write(Levels level) const -> void {
      gpio_put(std::to_underlying(pin), std::to_underlying(level));
    }

    [[nodiscard]] auto Read() const -> Levels {
      return static_cast<Levels>(gpio_get(std::to_underlying(pin)));
    }

    [[nodiscard]] auto OutLevel() const -> Levels {
      return static_cast<Levels>(gpio_get(std::to_underlying(pin)));
    }
  };

  // Global instances for each GPIO pin
  inline constinit GPIO gpio_0{Pins::gpio_0};
  inline constinit GPIO gpio_1{Pins::gpio_1};
  inline constinit GPIO gpio_2{Pins::gpio_2};
  inline constinit GPIO gpio_3{Pins::gpio_3};
  inline constinit GPIO gpio_4{Pins::gpio_4};
  inline constinit GPIO gpio_5{Pins::gpio_5};
  inline constinit GPIO gpio_6{Pins::gpio_6};
  inline constinit GPIO gpio_7{Pins::gpio_7};
  inline constinit GPIO gpio_8{Pins::gpio_8};
  inline constinit GPIO gpio_9{Pins::gpio_9};
  inline constinit GPIO gpio_10{Pins::gpio_10};
  inline constinit GPIO gpio_11{Pins::gpio_11};
  inline constinit GPIO gpio_12{Pins::gpio_12};
  inline constinit GPIO gpio_13{Pins::gpio_13};
  inline constinit GPIO gpio_14{Pins::gpio_14};
  inline constinit GPIO gpio_15{Pins::gpio_15};
  inline constinit GPIO gpio_16{Pins::gpio_16};
  inline constinit GPIO gpio_17{Pins::gpio_17};
  inline constinit GPIO gpio_18{Pins::gpio_18};
  inline constinit GPIO gpio_19{Pins::gpio_19};
  inline constinit GPIO gpio_20{Pins::gpio_20};
  inline constinit GPIO gpio_21{Pins::gpio_21};
  inline constinit GPIO gpio_22{Pins::gpio_22};
  inline constinit GPIO gpio_23{Pins::gpio_23};
  inline constinit GPIO gpio_24{Pins::gpio_24};
  inline constinit GPIO gpio_25{Pins::gpio_25};
  inline constinit GPIO gpio_26{Pins::gpio_26};
  inline constinit GPIO gpio_27{Pins::gpio_27};
  inline constinit GPIO gpio_28{Pins::gpio_28};
  inline constinit GPIO gpio_29{Pins::gpio_29};
}  // namespace rpp::gpio

#endif  //EE490_FIRMWARE_GPIO_HPP
