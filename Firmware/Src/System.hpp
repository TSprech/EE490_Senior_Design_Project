//
// Created by treys on 2023/08/28
//

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <tuple>

#include "ConditionalFunction.hpp"
#include "RP2040Atomic.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "units.h"
#include "TypedUnits.hpp"
#include "GPIO.hpp"
#include "RP2040_PWM.hpp"
#include "MPPT.hpp"

inline auto UARTPrint(std::string_view str) -> void {
  for (auto letter : str) putc(letter, stdout);
}

inline auto UARTVPrint(fmt::string_view format_str, fmt::format_args args) -> void {
  fmt::memory_buffer buffer;
  fmt::detail::vformat_to(buffer, format_str, args);
  UARTPrint({buffer.data(), buffer.size()});
}

template <typename... T>
inline auto FMTDebug(fmt::format_string<T...> fmt, T &&...args) -> void {
  const auto &vargs = fmt::make_format_args(args...);
  UARTVPrint(fmt, vargs);
}

namespace sys {
  using namespace units::literals;

  namespace internal {
    template <typename Underlying, typename Units>  // TODO: Maybe use: units::traits::unit_t_traits<decltype(dist)>::underlying_type
    class VolatileStrongType {
     public:
      auto operator=(Units units_value) -> VolatileStrongType& {
        underlying_ = units_value.value();
        return *this;
      }
      auto Value() -> Units {
        return Units(underlying_.Load());
      }

     private:
      patom::PseudoAtomic<Underlying> underlying_;
    };

    template <typename T> requires std::integral<T>
    class LimitedInt {
     public:
      constexpr LimitedInt(T min, T max, T initial = 0, bool wrap = false)
          : min_(min), max_(max), number_(initial), wrap_(wrap) {}

      auto operator+(T value) -> void {
        if (this->number_ + value > this->max_) {
          this->number_ = (this->wrap_) ? this->min_ : this->max_;
        }
      }

      auto operator+=(T value) -> void {
        if (this->number_ + value > this->max_) {
          this->number_ = (this->wrap_) ? this->min_ : this->max_;
        }
      }

     private:
      const bool wrap_;
      T number_;
      const T min_;
      const T max_;
    };

    template <typename T> requires std::integral<T>
    class AtomicLimitedInt {
     public:
      constexpr AtomicLimitedInt(T min, T max, bool wrap = false)
          : min_(min), max_(max), wrap_(wrap) {}

      auto operator+(T value) -> void {
        if (this->number_.Load() + value > this->max_) {
          this->number_ = (this->wrap_) ? this->min_ : this->max_;
        }
      }

//      auto operator+=(T value) -> void {
//        if (this->number_.Load() + value > this->max_) {
//          this->number_ = (this->wrap_) ? this->min_ : this->max_;
//        } else if (value < this->min_) {
//          this->number_ = (this->wrap_) ? this->max_ : this->min_;
//        } else {
//          this->number_ = this->number_.Load() + value;
//        }
//      }

      auto operator=(T value) -> void {
        if (value > this->max_) {
          this->number_ = (this->wrap_) ? this->min_ : this->max_;
        } else if (value < this->min_) {
          this->number_ = (this->wrap_) ? this->max_ : this->min_;
        } else {
          this->number_ = value;
        }
      }

      [[nodiscard]] auto Load() -> T {
        return number_.Load();
      }

      [[nodiscard]] explicit operator T() {
        return number_.Load();
      }

     private:
      const bool wrap_;
      patom::PseudoAtomic<T> number_;
      const T min_;
      const T max_;
    };
  }  // namespace internal

  inline internal::VolatileStrongType<int32_t, units::voltage::millivolt_i32_t> panel_voltage;
  inline internal::VolatileStrongType<int32_t, units::current::milliampere_i32_t> panel_current;
  inline internal::VolatileStrongType<int32_t, units::voltage::millivolt_i32_t> battery_voltage;
  inline internal::VolatileStrongType<int32_t, units::current::milliampere_i32_t> battery_current;
  inline internal::VolatileStrongType<int32_t, units::power::milliwatt_i32_t> panel_power;
  inline internal::VolatileStrongType<int32_t, units::power::milliwatt_i32_t> battery_power;
  inline internal::AtomicLimitedInt<int32_t> duty(0, 1'000'000);  // TODO: Make this a LimitedInt

  inline auto pwm_smps = pwm::PWMManager<16, 17>(true);

  inline auto& usb_right_led_pin = rpp::gpio::gpio_22;
  inline auto& usb_left_led_pin = rpp::gpio::gpio_23;
  inline auto& core_0_uart_led_pin = rpp::gpio::gpio_14;
  inline auto& core_1_uart_led_pin = rpp::gpio::gpio_19;
  inline auto& pwm_enable_led_pin = rpp::gpio::gpio_15;
  inline auto& debug_led_pin = rpp::gpio::gpio_18;

  inline auto& core_0_utilization = rpp::gpio::gpio_13;
  inline auto& core_1_utilization = rpp::gpio::gpio_21;

  inline auto& ovp_enable = rpp::gpio::gpio_7;
  inline auto& ovp_status = rpp::gpio::gpio_6;

//  inline auto& pwm_a = rpp::gpio::gpio_16;
//  inline auto& pwm_b = rpp::gpio::gpio_17;

  inline auto& adc_0_cipo = rpp::gpio::gpio_0;
  inline auto& adc_0_cs = rpp::gpio::gpio_1;
  inline auto& adc_0_sclk = rpp::gpio::gpio_2;

  inline auto& adc_1_cipo = rpp::gpio::gpio_3;
  inline auto& adc_1_cs = rpp::gpio::gpio_4;
  inline auto& adc_1_sclk = rpp::gpio::gpio_5;

  [[gnu::always_inline]]
  inline auto ADCDelay() -> void { // The goal is to take the clock rate for the ADC to 1/10 sys clock, who knows if this actually does it
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }

  inline auto Initialize() -> void {
    sys::usb_right_led_pin.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);
    sys::usb_left_led_pin.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);
    sys::core_0_uart_led_pin.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);
    sys::core_1_uart_led_pin.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);
    sys::pwm_enable_led_pin.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::high);
    sys::debug_led_pin.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);

    sys::ovp_enable.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::high);
    sys::ovp_status.Init().Direction(rpp::gpio::Directions::input).Pull(rpp::gpio::Pulls::disable).Schmitt(rpp::gpio::States::enabled);  // TODO: Maybe Schmitt?

    sys::adc_0_cipo.Init().Direction(rpp::gpio::Directions::input).Pull(rpp::gpio::Pulls::disable).Schmitt(rpp::gpio::States::enabled);  // TODO: Maybe Schmitt?
    sys::adc_0_cs.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::high);
    sys::adc_0_sclk.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);
    sys::adc_1_cipo.Init().Direction(rpp::gpio::Directions::input).Pull(rpp::gpio::Pulls::disable).Schmitt(rpp::gpio::States::enabled);  // TODO: Maybe Schmitt?
    sys::adc_1_cs.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::high);
    sys::adc_1_sclk.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);

    sys::pwm_smps.Initialize().Frequency(425'000).DutyCycle(0).DeadBand(1).Disable();

      uint16_t voltage_raw = 0;

      sys::adc_0_cs.Write(rpp::gpio::Levels::high);
      ADCDelay();
      sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
      ADCDelay();
      sys::adc_0_cs.Write(rpp::gpio::Levels::low);
      ADCDelay();

      for (int i = 0; i < 16; i++) {
        sys::adc_0_sclk.Write(rpp::gpio::Levels::high);
        ADCDelay();
        sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
        ADCDelay();
      }

      uint16_t current_raw = 0;

      sys::adc_0_cs.Write(rpp::gpio::Levels::low);
      ADCDelay();
      sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
      ADCDelay();
      sys::adc_0_cs.Write(rpp::gpio::Levels::high);
      ADCDelay();

      // Read 12 bits of data from ADS7042
      for (int i = 0; i < 16; i++) {
        sys::adc_0_sclk.Write(rpp::gpio::Levels::high);
        ADCDelay();
        sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
        ADCDelay();
      }
  }

  inline auto ReadADC0() -> std::tuple<uint16_t, uint16_t> { // Yes, I know there are better ways to do this, just give me a break
    uint16_t voltage_raw = 0;

    sys::adc_0_cs.Write(rpp::gpio::Levels::high);
    ADCDelay();
    sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
    ADCDelay();
    sys::adc_0_cs.Write(rpp::gpio::Levels::low);
    ADCDelay();

    for (int i = 0; i < 14; i++) {
      sys::adc_0_sclk.Write(rpp::gpio::Levels::high);
      ADCDelay();
      voltage_raw = (voltage_raw << 1) | std::to_underlying(sys::adc_0_cipo.Read());
      ADCDelay();
      sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
      ADCDelay();
    }

    uint16_t current_raw = 0;

    sys::adc_0_cs.Write(rpp::gpio::Levels::low);
    ADCDelay();
    sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
    ADCDelay();
    sys::adc_0_cs.Write(rpp::gpio::Levels::high);
    ADCDelay();

    // Read 12 bits of data from ADS7042
    for (int i = 0; i < 14; i++) {
      sys::adc_0_sclk.Write(rpp::gpio::Levels::high);
      ADCDelay();
      current_raw = (current_raw << 1) | std::to_underlying(sys::adc_0_cipo.Read());
      ADCDelay();
      sys::adc_0_sclk.Write(rpp::gpio::Levels::low);
      ADCDelay();
    }

    return {voltage_raw >> 1, current_raw >> 1};
  }

  inline auto ReadADC1() -> std::tuple<uint16_t, uint16_t> { // Yes, I know there are better ways to do this, just give me a break
    uint16_t voltage_raw = 0;

    sys::adc_1_cs.Write(rpp::gpio::Levels::high);
    ADCDelay();
    sys::adc_1_sclk.Write(rpp::gpio::Levels::low);
    ADCDelay();
    sys::adc_1_cs.Write(rpp::gpio::Levels::low);
    ADCDelay();

    for (int i = 0; i < 14; i++) {
      sys::adc_1_sclk.Write(rpp::gpio::Levels::high);
      ADCDelay();
      voltage_raw = (voltage_raw << 1) | std::to_underlying(sys::adc_1_cipo.Read());
      ADCDelay();
      sys::adc_1_sclk.Write(rpp::gpio::Levels::low);
      ADCDelay();
    }

    uint16_t current_raw = 0;

    sys::adc_1_cs.Write(rpp::gpio::Levels::low);
    ADCDelay();
    sys::adc_1_sclk.Write(rpp::gpio::Levels::low);
    ADCDelay();
    sys::adc_1_cs.Write(rpp::gpio::Levels::high);
    ADCDelay();

    // Read 12 bits of data from ADS7042
    for (int i = 0; i < 14; i++) {
      sys::adc_1_sclk.Write(rpp::gpio::Levels::high);
      ADCDelay();
      current_raw = (current_raw << 1) | std::to_underlying(sys::adc_1_cipo.Read());
      ADCDelay();
      sys::adc_1_sclk.Write(rpp::gpio::Levels::low);
      ADCDelay();
    }

    return {voltage_raw >> 1, current_raw >> 1};
  }

  class BatteryManager {
   public:
    constexpr explicit BatteryManager(units::charge::milliampere_hour_i32_t battery_rating, units::charge::millicoulomb_i32_t current_charge = 0_mC_i32)
        : battery_rating_(battery_rating), full_charge_(battery_rating), current_charge_(current_charge) {}

    [[nodiscard]] auto ChargeState() const -> float {
      //    return current_charge_ * 100 / full_charge_;
      //    return units::charge::coulomb_i32_t{current_charge_} * 100 / units::charge::coulomb_i32_t{full_charge_};
      return units::charge::coulomb_f_t{current_charge_} * 100.0F / units::charge::coulomb_f_t{full_charge_};
    }

    [[nodiscard]] auto Charge() const -> units::charge::millicoulomb_i32_t {
      return current_charge_;
    }

    [[nodiscard]] auto Capacity() const -> units::charge::millicoulomb_i32_t {
      return full_charge_;
    }

    auto Current(units::current::milliampere_u32_t current, units::time::millisecond_i32_t period) {
      //    FMTDebug("Period: {}\n", period.value());
      //    FMTDebug("New Charge: {}\n", (current * period).value());
      this->current_charge_ = this->current_charge_ + units::charge::millicoulomb_i32_t(current * period); // TODO: Workaround for strong typed units math bug
    }

   private:
    const units::charge::milliampere_hour_i32_t battery_rating_;
    const units::charge::millicoulomb_i32_t full_charge_;
    units::charge::millicoulomb_i32_t current_charge_;
  };

  inline BatteryManager bm(3_Ah_i32, 300_C_i32);

  inline auto mppt_call = cfunc::FunctionManager(mppt::IncrementalConductance);
  inline auto coulomb_counter_call = cfunc::FunctionManager([manager = &bm](auto current, auto period) -> void { manager->Current(current,period); });
}  // namespace sys

#endif  // SYSTEM_HPP
