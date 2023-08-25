#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstdio>
#include <tuple>

#define DOUT_PIN 0
#define SCLK_PIN 2
#define CS_PIN 1
#define DELAY 0


void spi_init() {
    gpio_init(DOUT_PIN);
  gpio_init(SCLK_PIN);
  gpio_init(CS_PIN);

    gpio_set_dir(DOUT_PIN, GPIO_IN);
  gpio_set_dir(SCLK_PIN, GPIO_OUT);
  gpio_set_dir(CS_PIN, GPIO_OUT);
}

auto spi_read() -> std::tuple<uint16_t, uint16_t> {
  uint16_t data1 = 0;

  gpio_put(CS_PIN, 1);
  sleep_us(DELAY);
  gpio_put(SCLK_PIN, 0);
  sleep_us(DELAY);
  gpio_put(CS_PIN,0);
  sleep_us(DELAY);

  for (int i = 0; i < 14; i++) {
    gpio_put(SCLK_PIN, 1);
    sleep_us(DELAY);
    data1 = (data1 << 1) | gpio_get(DOUT_PIN);
    sleep_us(DELAY);
    gpio_put(SCLK_PIN, 0);
    sleep_us(DELAY);
  }

  uint16_t data2 = 0;

  gpio_put(CS_PIN, 0);
  sleep_us(DELAY);
  gpio_put(SCLK_PIN, 0);
  sleep_us(DELAY);
  gpio_put(CS_PIN,1);
  sleep_us(DELAY);

  // Read 12 bits of data from ADS7042
  for (int i = 0; i < 14; i++) {
    gpio_put(SCLK_PIN, 1);
    sleep_us(DELAY);
        data2 = (data2 << 1) | gpio_get(DOUT_PIN);
    sleep_us(DELAY);
    gpio_put(SCLK_PIN, 0);
    sleep_us(DELAY);
  }

  return {data1, data2};
}

int main() {
  stdio_init_all();

  spi_init();
  gpio_put(CS_PIN, 1);
  sleep_us(1);

  while (true) {
    auto [data1, data2] = spi_read();
        printf("Voltage: %dmV Current: %dmA\n", (data1 >> 1) * 13, static_cast<int16_t>((data2 >> 1) - 1024) * 5 / 3);
//    sleep_ms(200);
//        sleep_us(4);
  }
}







///**
//* Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
//*
//* SPDX-License-Identifier: BSD-3-Clause
//*/
//
//#include <stdio.h>
//#include <math.h>
//
//#include "pico/stdlib.h"
//#include "hardware/pio.h"
//#include "apa102.pio.h"
//
//#define PIN_CLK 10
//#define PIN_DIN 11
//
//#define N_LEDS 10
//#define SERIAL_FREQ (5 * 1000 * 1000)
//
//// Global brightness value 0->31
//#define BRIGHTNESS 1
//
//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif
//
//void put_start_frame(PIO pio, uint sm) {
// pio_sm_put_blocking(pio, sm, 0u);
//}
//
//void put_end_frame(PIO pio, uint sm) {
// pio_sm_put_blocking(pio, sm, ~0u);
//}
//
//void put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b) {
// pio_sm_put_blocking(pio, sm,
//                     0x7 << 29 |                   // magic
//                         (BRIGHTNESS & 0x1f) << 24 |   // global brightness parameter
//                         (uint32_t) b << 16 |
//                         (uint32_t) g << 8 |
//                         (uint32_t) r << 0
// );
//}
//
//#define TABLE_SIZE (1 << 8)
//uint8_t wave_table[TABLE_SIZE];
//
//int main() {
// stdio_init_all();
//
// PIO pio = pio0;
// uint sm = 0;
// uint offset = pio_add_program(pio, &apa102_mini_program);
// apa102_mini_program_init(pio, sm, offset, SERIAL_FREQ, PIN_CLK, PIN_DIN);
//
// for (int i = 0; i < TABLE_SIZE; ++i)
//   wave_table[i] = (uint8_t) (powf(sinf(i * M_PI / TABLE_SIZE), 5.f) * 255);
//
// uint t = 0;
// while (true) {
//   put_start_frame(pio, sm);
//   for (int i = 0; i < N_LEDS; ++i) {
//     put_rgb888(pio, sm, 128, 128, 128);
////     put_rgb888(pio, sm,
////                wave_table[(i + t) % TABLE_SIZE],
////                wave_table[(2 * i + 3 * 2) % TABLE_SIZE],
////                wave_table[(3 * i + 4 * t) % TABLE_SIZE]
////     );
//   }
//   put_end_frame(pio, sm);
//   sleep_ms(10);
//   ++t;
// }
//}




//#include "pico/stdlib.h"
//
//int main() {
//  const uint UPIN1 = 22;
//  const uint UPIN2 = 23;
//  const uint C0PIN = 14;
//  const uint C1PIN = 19;
//  const uint UPIN = 18;
//  const uint PWM = 15;
//  gpio_init(UPIN1);
//  gpio_init(UPIN2);
//  gpio_init(C0PIN);
//  gpio_init(C1PIN);
//  gpio_init(UPIN);
//  gpio_init(PWM);
//  gpio_set_dir(UPIN1, GPIO_OUT);
//  gpio_set_dir(UPIN2, GPIO_OUT);
//  gpio_set_dir(C0PIN, GPIO_OUT);
//  gpio_set_dir(C1PIN, GPIO_OUT);
//  gpio_set_dir(UPIN, GPIO_OUT);
//  gpio_set_dir(PWM, GPIO_OUT);
//  while (true) {
//    gpio_put(UPIN1, 1);
//    sleep_ms(250);
//    gpio_put(UPIN2, 1);
//    sleep_ms(250);
//    gpio_put(C0PIN, 1);
//    sleep_ms(250);
//    gpio_put(C1PIN, 1);
//    sleep_ms(250);
//    gpio_put(UPIN, 1);
//    sleep_ms(250);
//    gpio_put(PWM, 1);
//    gpio_put(UPIN1, 0);
//    sleep_ms(250);
//    gpio_put(UPIN2, 0);
//    sleep_ms(250);
//    gpio_put(C0PIN, 0);
//    sleep_ms(250);
//    gpio_put(C1PIN, 0);
//    sleep_ms(250);
//    gpio_put(UPIN, 0);
//    sleep_ms(250);
//    gpio_put(PWM, 0);
//    sleep_ms(250);
//  }
//}



//#include <algorithm>
//#include <charconv>
//
//#include "ADC.hpp"
//#include "GPIO.hpp"
//#include "GetLine.hpp"
//#include "RP2040_PWM.hpp"
//#include "MPPT.hpp"
//#include "TypedUnits.hpp"
//#include "fmt/core.h"
//#include "fmt/format.h"
//#include "pico/stdlib.h"
//#include "tinyfsm.hpp"
//#include <concepts>
//#include <functional>
//
//#include "ConditionalFunction.hpp"
//#include "RP2040Atomic.hpp"
//
//using namespace units::literals;
//
//class BatteryManager {
// public:
//  constexpr explicit BatteryManager(units::charge::milliampere_hour_i32_t battery_rating, units::charge::millicoulomb_i32_t current_charge = 0_mC_i32)
//      : battery_rating_(battery_rating), full_charge_(battery_rating), current_charge_(current_charge) {}
//
//  [[nodiscard]] auto ChargeState() const -> float {
//    //    return current_charge_ * 100 / full_charge_;
//    //    return units::charge::coulomb_i32_t{current_charge_} * 100 / units::charge::coulomb_i32_t{full_charge_};
//    return units::charge::coulomb_f_t{current_charge_} * 100.0F / units::charge::coulomb_f_t{full_charge_};
//  }
//
//  [[nodiscard]] auto Charge() const -> units::charge::millicoulomb_i32_t {
//    return current_charge_;
//  }
//
//  [[nodiscard]] auto Capacity() const -> units::charge::millicoulomb_i32_t {
//    return full_charge_;
//  }
//
//  auto Current(units::current::milliampere_u32_t current, units::time::millisecond_i32_t period) {
//    //    FMTDebug("Period: {}\n", period.value());
//    //    FMTDebug("New Charge: {}\n", (current * period).value());
//    this->current_charge_ += current * period;
//  }
//
// private:
//  const units::charge::milliampere_hour_i32_t battery_rating_;
//  const units::charge::millicoulomb_i32_t full_charge_;
//  units::charge::millicoulomb_i32_t current_charge_;
//};
//
//
////#define UNIT_ADD_WITH_METRIC_PREFIXES_STANDARD_TYPES_1(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...) \
////  UNIT_ADD_WITH_METRIC_PREFIXES_CUSTOM_TYPE(namespaceName, nameSingular ## _vu32, namePlural ## _vu32, abbreviation ## _vu32, patom::types::patomic_uint32_t, __VA_ARGS__) \
////
////namespace units {
//////  UNIT_ADD_WITH_METRIC_PREFIXES_STANDARD_TYPES_1(voltage, volt, volts, V, unit<std::ratio<1>, units::category::voltage_unit>)
////
////  namespace voltage {
////    typedef unit<std::ratio<1>, units::category::voltage_unit> volts_vu32;
////    typedef volts_vu32 volt_vu32;
////    typedef volts_vu32 V_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<volt_vu32, patom::types::patomic_uint32_t> volt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::volt_vu32_t operator""_V_vu32(long double d) {
////      return voltage::volt_vu32_t(static_cast<voltage::volt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::volt_vu32_t operator""_V_vu32(unsigned long long d) {
////      return voltage::volt_vu32_t(static_cast<voltage::volt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef femto<volts_vu32> femtovolts_vu32;
////    typedef femtovolts_vu32 femtovolt_vu32;
////    typedef femtovolts_vu32 fV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<femtovolt_vu32, patom::types::patomic_uint32_t> femtovolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::femtovolt_vu32_t operator""_fV_vu32(long double d) {
////      return voltage::femtovolt_vu32_t(static_cast<voltage::femtovolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::femtovolt_vu32_t operator""_fV_vu32(unsigned long long d) {
////      return voltage::femtovolt_vu32_t(static_cast<voltage::femtovolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef pico<volts_vu32> picovolts_vu32;
////    typedef picovolts_vu32 picovolt_vu32;
////    typedef picovolts_vu32 pV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<picovolt_vu32, patom::types::patomic_uint32_t> picovolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::picovolt_vu32_t operator""_pV_vu32(long double d) {
////      return voltage::picovolt_vu32_t(static_cast<voltage::picovolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::picovolt_vu32_t operator""_pV_vu32(unsigned long long d) {
////      return voltage::picovolt_vu32_t(static_cast<voltage::picovolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef nano<volts_vu32> nanovolts_vu32;
////    typedef nanovolts_vu32 nanovolt_vu32;
////    typedef nanovolts_vu32 nV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<nanovolt_vu32, patom::types::patomic_uint32_t> nanovolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::nanovolt_vu32_t operator""_nV_vu32(long double d) {
////      return voltage::nanovolt_vu32_t(static_cast<voltage::nanovolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::nanovolt_vu32_t operator""_nV_vu32(unsigned long long d) {
////      return voltage::nanovolt_vu32_t(static_cast<voltage::nanovolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef micro<volts_vu32> microvolts_vu32;
////    typedef microvolts_vu32 microvolt_vu32;
////    typedef microvolts_vu32 uV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<microvolt_vu32, patom::types::patomic_uint32_t> microvolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::microvolt_vu32_t operator""_uV_vu32(long double d) {
////      return voltage::microvolt_vu32_t(static_cast<voltage::microvolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::microvolt_vu32_t operator""_uV_vu32(unsigned long long d) {
////      return voltage::microvolt_vu32_t(static_cast<voltage::microvolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef milli<volts_vu32> millivolts_vu32;
////    typedef millivolts_vu32 millivolt_vu32;
////    typedef millivolts_vu32 mV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<millivolt_vu32, patom::types::patomic_uint32_t> millivolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::millivolt_vu32_t operator""_mV_vu32(long double d) {
////      return voltage::millivolt_vu32_t(static_cast<voltage::millivolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::millivolt_vu32_t operator""_mV_vu32(unsigned long long d) {
////      return voltage::millivolt_vu32_t(static_cast<voltage::millivolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef centi<volts_vu32> centivolts_vu32;
////    typedef centivolts_vu32 centivolt_vu32;
////    typedef centivolts_vu32 cV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<centivolt_vu32, patom::types::patomic_uint32_t> centivolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::centivolt_vu32_t operator""_cV_vu32(long double d) {
////      return voltage::centivolt_vu32_t(static_cast<voltage::centivolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::centivolt_vu32_t operator""_cV_vu32(unsigned long long d) {
////      return voltage::centivolt_vu32_t(static_cast<voltage::centivolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef deci<volts_vu32> decivolts_vu32;
////    typedef decivolts_vu32 decivolt_vu32;
////    typedef decivolts_vu32 dV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<decivolt_vu32, patom::types::patomic_uint32_t> decivolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::decivolt_vu32_t operator""_dV_vu32(long double d) {
////      return voltage::decivolt_vu32_t(static_cast<voltage::decivolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::decivolt_vu32_t operator""_dV_vu32(unsigned long long d) {
////      return voltage::decivolt_vu32_t(static_cast<voltage::decivolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef deca<volts_vu32> decavolts_vu32;
////    typedef decavolts_vu32 decavolt_vu32;
////    typedef decavolts_vu32 daV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<decavolt_vu32, patom::types::patomic_uint32_t> decavolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::decavolt_vu32_t operator""_daV_vu32(long double d) {
////      return voltage::decavolt_vu32_t(static_cast<voltage::decavolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::decavolt_vu32_t operator""_daV_vu32(unsigned long long d) {
////      return voltage::decavolt_vu32_t(static_cast<voltage::decavolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef hecto<volts_vu32> hectovolts_vu32;
////    typedef hectovolts_vu32 hectovolt_vu32;
////    typedef hectovolts_vu32 hV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<hectovolt_vu32, patom::types::patomic_uint32_t> hectovolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::hectovolt_vu32_t operator""_hV_vu32(long double d) {
////      return voltage::hectovolt_vu32_t(static_cast<voltage::hectovolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::hectovolt_vu32_t operator""_hV_vu32(unsigned long long d) {
////      return voltage::hectovolt_vu32_t(static_cast<voltage::hectovolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef kilo<volts_vu32> kilovolts_vu32;
////    typedef kilovolts_vu32 kilovolt_vu32;
////    typedef kilovolts_vu32 kV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<kilovolt_vu32, patom::types::patomic_uint32_t> kilovolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::kilovolt_vu32_t operator""_kV_vu32(long double d) {
////      return voltage::kilovolt_vu32_t(static_cast<voltage::kilovolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::kilovolt_vu32_t operator""_kV_vu32(unsigned long long d) {
////      return voltage::kilovolt_vu32_t(static_cast<voltage::kilovolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef mega<volts_vu32> megavolts_vu32;
////    typedef megavolts_vu32 megavolt_vu32;
////    typedef megavolts_vu32 MV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<megavolt_vu32, patom::types::patomic_uint32_t> megavolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::megavolt_vu32_t operator""_MV_vu32(long double d) {
////      return voltage::megavolt_vu32_t(static_cast<voltage::megavolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::megavolt_vu32_t operator""_MV_vu32(unsigned long long d) {
////      return voltage::megavolt_vu32_t(static_cast<voltage::megavolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef giga<volts_vu32> gigavolts_vu32;
////    typedef gigavolts_vu32 gigavolt_vu32;
////    typedef gigavolts_vu32 GV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<gigavolt_vu32, patom::types::patomic_uint32_t> gigavolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::gigavolt_vu32_t operator""_GV_vu32(long double d) {
////      return voltage::gigavolt_vu32_t(static_cast<voltage::gigavolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::gigavolt_vu32_t operator""_GV_vu32(unsigned long long d) {
////      return voltage::gigavolt_vu32_t(static_cast<voltage::gigavolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef tera<volts_vu32> teravolts_vu32;
////    typedef teravolts_vu32 teravolt_vu32;
////    typedef teravolts_vu32 TV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<teravolt_vu32, patom::types::patomic_uint32_t> teravolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::teravolt_vu32_t operator""_TV_vu32(long double d) {
////      return voltage::teravolt_vu32_t(static_cast<voltage::teravolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::teravolt_vu32_t operator""_TV_vu32(unsigned long long d) {
////      return voltage::teravolt_vu32_t(static_cast<voltage::teravolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////  namespace voltage {
////    typedef peta<volts_vu32> petavolts_vu32;
////    typedef petavolts_vu32 petavolt_vu32;
////    typedef petavolts_vu32 PV_vu32;
////  }  // namespace voltage
////  namespace voltage {
////    typedef unit_t<petavolt_vu32, patom::types::patomic_uint32_t> petavolt_vu32_t;
////  }
////  namespace literals {
////    inline constexpr voltage::petavolt_vu32_t operator""_PV_vu32(long double d) {
////      return voltage::petavolt_vu32_t(static_cast<voltage::petavolt_vu32_t::underlying_type>(d));
////    }
////    inline constexpr voltage::petavolt_vu32_t operator""_PV_vu32(unsigned long long d) {
////      return voltage::petavolt_vu32_t(static_cast<voltage::petavolt_vu32_t::underlying_type>(d));
////    }
////  }  // namespace literals
////
////
////}
//
//namespace sys123 {
//  volatile patom::PseudoAtomic<units::voltage::millivolt_i32_t> panel_voltage;
//  patom::PseudoAtomic<units::current::milliampere_i32_t> panel_current;
//  patom::PseudoAtomic<units::voltage::millivolt_i32_t> battery_voltage;
//  patom::PseudoAtomic<units::current::milliampere_i32_t> battery_current;
//  patom::types::patomic_uint32_t duty;
//}
//
//constexpr auto led_pin = PICO_DEFAULT_LED_PIN;
//auto pwm_01 = pwm::PWMManager<0, 1>(true);
//
//inline auto UARTPrint(std::string_view str) -> void {
//  for (auto letter : str) putc(letter, stdout);
//}
//
//inline auto UARTVPrint(fmt::string_view format_str, fmt::format_args args) -> void {
//  fmt::memory_buffer buffer;
//  fmt::detail::vformat_to(buffer, format_str, args);
//  UARTPrint({buffer.data(), buffer.size()});
//}
//
//template <typename... T>
//inline auto FMTDebug(fmt::format_string<T...> fmt, T &&...args) -> void {
//  const auto &vargs = fmt::make_format_args(args...);
//  UARTVPrint(fmt, vargs);
//}
//
//BatteryManager bm(3_Ah_i32, 300_C_i32);
//
//auto mppt_call = cfunc::FunctionManager(mppt::IncrementalConductance);
//auto coulomb_counter_call = cfunc::FunctionManager([manager = &bm](auto current, auto period) -> void { manager->Current(current,period); });
//
//struct RS0 : tinyfsm::Event {};
//struct RS1 : tinyfsm::Event {};
//struct RS2 : tinyfsm::Event {};
//struct RS3 : tinyfsm::Event {};
//
///* Finite State Machine base class */
//class RunStateFSM : public tinyfsm::Fsm<RunStateFSM> {
// public:
//  /* react on all events and do nothing by default */
//  virtual void react(RS0 const &) {}
//
//  virtual void react(RS1 const &) {}
//
//  virtual void react(RS2 const &) {}
//
//  virtual void react(RS3 const &) {}
//
//  virtual void react(tinyfsm::Event const &) {}
//
//  virtual void entry(){};
//
//  virtual void exit(){};
//
//  static void reset(){};
//
//  static auto current_state() -> uint8_t {
//    return state;
//  }
//
//  static auto current_state(uint8_t new_state) -> void {
//    state = new_state;
//  }
//
//  inline static uint8_t state = 0;
//};
//
//class RunState0;
//class RunState1;
//class RunState2;
//class RunState3;
//
//class RunState0 : public RunStateFSM {
//  void entry() override {
//    FMTDebug("ENTER: Run State 0\n");
//    RunStateFSM::current_state(0);
//    pwm_01.Disable();
//    // TODO: Disable MOSFETs
//  }
//
//  void exit() override {
//    FMTDebug("EXIT: Run State 0\n");
//  }
//
//  void react(RS0 const &) override {}
//
//  void react(RS1 const &) override {
//    transit<RunState1>();
//  }
//
//  void react(RS2 const &) override {
//    transit<RunState2>();
//  }
//
//  void react(RS3 const &) override {
//    transit<RunState3>();
//  }
//};
//
//class RunState1 : public RunStateFSM {
//  void entry() override {
//    FMTDebug("ENTER: Run State 1\n");
//    RunStateFSM::current_state(1);
//  }
//
//  void exit() override {
//    FMTDebug("EXIT: Run State 1\n");
//  }
//
//
//  void react(RS0 const &) override {
//    transit<RunState0>();
//  }
//
//  void react(RS1 const &) override {}
//
//  void react(RS2 const &) override {
//    transit<RunState2>();
//  }
//
//  void react(RS3 const &) override {
//    transit<RunState3>();
//  }
//};
//
//class RunState2 : public RunStateFSM {
//  void entry() override {
//    FMTDebug("ENTER: Run State 1\n");
//    RunStateFSM::current_state(2);
//    pwm_01.Enable();
//    pwm_01.DutyCycle(sys123::duty.Load() * 10'000);
//    coulomb_counter_call.Enable();
//  }
//
//  void exit() override {
//    FMTDebug("EXIT: Run State 1\n");
//    coulomb_counter_call.Disable();
//  }
//
//  void react(RS0 const &) override {
//    transit<RunState0>();
//  }
//
//  void react(RS1 const &) override {
//    transit<RunState1>();
//  }
//
//  void react(RS2 const &) override {}
//
//  void react(RS3 const &) override {
//    transit<RunState3>();
//  }
//};
//
//class RunState3 : public RunStateFSM {
//  void entry() override {
//    FMTDebug("ENTER: Run State 1\n");
//    RunStateFSM::current_state(3);
//    pwm_01.Enable();
//    pwm_01.DutyCycle(sys123::duty.Load() * 10'000);
//    coulomb_counter_call.Enable();
//    mppt_call.Enable();
//  }
//
//  void exit() override {
//    FMTDebug("EXIT: Run State 1\n");
//    mppt_call.Disable();
//    coulomb_counter_call.Disable();
//  }
//
//  void react(RS0 const &) override {
//    transit<RunState0>();
//  }
//
//  void react(RS1 const &) override {
//    transit<RunState1>();
//  }
//
//  void react(RS2 const &) override {
//    transit<RunState2>();
//  }
//
//  void react(RS3 const &) override {}
//};
//
//FSM_INITIAL_STATE(RunStateFSM, RunState0)
//
////static_assert(units::charge::millicoulomb_i32_t{units::charge::milliampere_hour_i32_t{3000}}.value() == 1);
//
//auto random_float = [] { return static_cast<float>(rand()) / static_cast<float>(rand()); };
//
//auto PrintData(repeating_timer_t *rt) -> bool {
//  FMTDebug("VPV={}, IPV={}, PPV={}, VBAT={}, IBAT={}, PBAT={}, QBAT={}, SOC={}, RS={}, D={}\n",
//           static_cast<float>(in_mV) / 1000.0F, static_cast<float>(in_mA) / 1000.0F,
//           static_cast<float>(in_mW) / 1000.0F, static_cast<float>(out_mV) / 1000.0F,
//           static_cast<float>(out_mA) / 1000.0F, static_cast<float>(out_mW) / 1000.0F, bm.Charge().value(), static_cast<float>(bm.ChargeState()),
//           RunStateFSM::current_state(), static_cast<float>(duty) / 10'000.0F);
//  return true;
//}
//
//template <typename T> requires std::integral<T>
//class LimitedInt {
// public:
//  constexpr LimitedInt(T min, T max, T initial = 0, bool wrap = false)
//      : min_(min), max_(max), number_(initial), wrap_(wrap) {}
//
//  auto operator+(T value) -> void {
//    if (this->number_ + value > this->max_) {
//      this->number_ = (this->wrap_) ? this->min_ : this->max_;
//    }
//  }
//
//  auto operator+=(T value) -> void {
//    if (this->number_ + value > this->max_) {
//      this->number_ = (this->wrap_) ? this->min_ : this->max_;
//    }
//  }
//
// private:
//  const bool wrap_;
//  T number_;
//  const T min_;
//  const T max_;
//};
//
//auto duty_cycle = LimitedInt<int32_t>(0, 1'000'000);
//
//#include <atomic>
//
//std::atomic<units::voltage::millivolt_i32_t> temp;
//
//auto UpdateADC(repeating_timer_t *rt) -> bool {
//  rpp::gpio::gpio_2.Write(rpp::gpio::Levels::high);
//  constexpr int32_t voltage_gain = 13;                               // Voltage divider ratio
//  constexpr int32_t current_offset = 1'024;                          // mV
//  auto adc_26_mV = units::voltage::millivolt_i32_t{static_cast<int32_t>(rpp::adc::adc_26.ReadmV())};  // Read all the ADC channels
//  auto adc_27_mV = units::voltage::millivolt_i32_t{static_cast<int32_t>(rpp::adc::adc_27.ReadmV())};
//  auto adc_28_mV = units::voltage::millivolt_i32_t{static_cast<int32_t>(rpp::adc::adc_28.ReadmV())};
//  auto adc_29_mV = units::voltage::millivolt_i32_t{static_cast<int32_t>(rpp::adc::adc_29.ReadmV())};
//  temp = adc_26_mV * voltage_gain;
//  sys123::panel_voltage = adc_26_mV * voltage_gain;              // Do integer math to calculate the values
//  in_mA = (adc_27_mV - current_offset) * 5 / 3;  // This nice ratio is the result of simplifying the following:
//  out_mV = adc_28_mV * voltage_gain;
//  out_mA = (adc_29_mV - current_offset) * 5 / 3;  // Gain 200 V/V * 1 / 6mΩ shunt * 0.5 divider ratio
//  in_mW = in_mV * in_mA / 1'000;                  // Then calculate power, dividing by 1,000 to remove the scaling ratio, keeping it in milliwatts
//  out_mW = out_mV * out_mA / 1'000;
//  coulomb_counter_call(units::current::milliampere_i32_t{1'000}, 10_ms_i32);
//  auto change = mppt_call(units::voltage::millivolt_i32_t{in_mV}, units::current::milliampere_i32_t{in_mA});
//  duty += change.value_or(0) * 20;
//  rpp::gpio::gpio_2.Write(rpp::gpio::Levels::low);
//  return true;
//}
//
//auto DispatchRunState(uint8_t state) -> void {
//  switch (state) {
//    case 0:
//      RunStateFSM::dispatch(RS0());
//      break;
//    case 1:
//      RunStateFSM::dispatch(RS1());
//      break;
//    case 2:
//      RunStateFSM::dispatch(RS2());
//      break;
//    case 3:
//      RunStateFSM::dispatch(RS3());
//      break;
//    default:
//      break;
//  }
//}
//
//auto main() -> int {
//  patom::PseudoAtomicInit();
//  duty = 0;
//  stdio_init_all();
//  gpio_init(led_pin);
//  gpio_set_dir(led_pin, GPIO_OUT);
//
//  rpp::gpio::gpio_2.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);
//
//  //  RunStateFSM::start();
//  //  while (true) {
//  //    RunStateFSM::dispatch(RS1());
//  //    sleep_ms(1000);
//  //    RunStateFSM::dispatch(RS0());
//  //    sleep_ms(1000);
//  //  }
//
//  rpp::adc::adc_26.Init().ReferenceVoltage(3300);  // Configure all the ADC pins
//  rpp::adc::adc_27.Init().ReferenceVoltage(3300);
//  rpp::adc::adc_28.Init().ReferenceVoltage(3300);
//  rpp::adc::adc_29.Init().ReferenceVoltage(3300);
//
//  repeating_timer_t timer_print;                                       // Timer used to manage the feedback loop callback
//  if (!add_repeating_timer_ms(100, PrintData, nullptr, &timer_print))  // Create a timer which calls the print function at the specified rate
//    FMTDebug("Failed to create feedback loop timer\n");
//
//  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
//  if (!add_repeating_timer_ms(10, UpdateADC, nullptr, &timer_adc))  // Create a timer which calls the print function at the specified rate
//    FMTDebug("Failed to create feedback loop timer\n");
//
//  srand(64);
//  pwm_01.Initialize().Frequency(425'000).DutyCycle(duty).DeadBand(1).Disable();
//  std::array<char, 512> buf{};  // String buffer
//
//  while (true) {
//    auto in_str = GetLine(buf, '\r');  // Read in some serial data if available
//    if (!in_str.empty()) {             // If it is available, it will be in the string_view that sits within buf
//      std::string_view numbers = {&buf[1], buf.end()};  // Grab everything after the first character as a single string which contains a number
//      int argument;                            // Variable which the number sent over serial will be read into
//
//      auto [ptr, error] = std::from_chars(numbers.data(), numbers.end(), argument);  // Attempt to extract a number from the string
//
//      if (error == std::errc()) [[likely]]  // If there were no issues
//        FMTDebug("Attempting to change {} to {}\n", buf[0], argument);
//      else [[unlikely]]  // There was some problem with parsing the number
//        FMTDebug("Invalid argument, trouble parsing number\n");
//
//      switch (buf[0]) {                                                                 // Decided what to do with the number based on the first character given
//        case ('S'): {                                                                   // Change run state
//          run_state = (argument == std::clamp(argument, 0, 3)) ? argument : run_state;  // If the argument is within the valid range, change the run state to the argument, otherwise do nothing
//          DispatchRunState(run_state);
//          break;
//        }
//        case ('D'): {                                                          // Change duty cycle
//          duty = (argument == std::clamp(argument, 1, 99)) ? argument : duty;  // If the argument is within the valid range, change the duty to the argument, otherwise do nothing
//          duty *= 10'000;
//          pwm_01.DutyCycle(duty);
//          break;
//        }
//        case ('T'): {
//          run_state = 0;
//          pwm_01.Disable();
//        }
//      }
//    }
//  }
//}