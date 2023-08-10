#include <algorithm>
#include <charconv>

#include "ADC.hpp"
#include "GetLine.hpp"
#include "RP2040_PWM.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "pico/stdlib.h"

#include "TypedUnits.hpp"

template<typename T>
class DeltaVariable {
public:
  constexpr explicit DeltaVariable(T initial_value) noexcept: value_(initial_value), previous_value_(initial_value) {}

  constexpr auto operator=(T value) noexcept -> DeltaVariable & {
    this->previous_value_ = value_;
    this->value_ = value;
    return *this;
  }

  [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]]
  constexpr auto Value() const noexcept -> T {
    return this->value_;
  }

  [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]]
  constexpr auto PreviousValue() const noexcept -> T {
    return this->previous_value_;
  }

  [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]]
  constexpr auto Delta() const noexcept -> T {
    return this->value_ - this->previous_value_;
  }

private:
  T value_;
  T previous_value_;
};

using namespace units::literals;

/**
 * @brief Incremental conductance MPPT algorithm.
 * @cite https://ww1.microchip.com/downloads/en/AppNotes/00001521A.pdf
 * @param v_panel The current panel voltage.
 * @param i_panel The current panel current.
 * @returns An offset to be applied to the current duty cycle.
 */
auto MPPTIC(units::voltage::millivolt_u32_t v_panel, units::current::milliampere_u32_t i_panel) -> int32_t {
  constexpr int32_t duty_step_size = 20; // How much to change the duty cycle per function call, range 0 - 1,000,000
  static DeltaVariable<units::voltage::millivolt_u32_t> panel_voltage{v_panel}; // Keeps track of the change in panel voltage per function call
  static DeltaVariable<units::current::milliampere_u32_t> panel_current{i_panel}; // Keeps track of the change in panel current per function call
  static DeltaVariable<units::power::milliwatt_u32_t> panel_power{v_panel * i_panel}; // Keeps track of the change in panel power per function call

  panel_voltage = v_panel; // Update the panel values
  panel_current = i_panel;
  panel_power = v_panel * i_panel;

  if (panel_voltage.Delta().value() != 0) {
    auto ratio = panel_power.Delta() / panel_voltage.Delta();
    if (ratio.value() > 0) return duty_step_size;
    else if (ratio.value() < 0) return -duty_step_size;
  } else {
    if (panel_current.Delta().value() > 0) return duty_step_size;
    else if (panel_current.Delta().value() < 0) return -duty_step_size;
  }
  return 0; // Case when no change is required
}

auto pwm_01 = pwm::PWMManager<0, 1>(true);

inline auto UARTPrint(std::string_view str) -> void {
  for (auto letter: str) putc(letter, stdout);
}

inline auto UARTVPrint(fmt::string_view format_str, fmt::format_args args) -> void {
  fmt::memory_buffer buffer;
  fmt::detail::vformat_to(buffer, format_str, args);
  UARTPrint({buffer.data(), buffer.size()});
}

template<typename... T>
inline auto FMTDebug(fmt::format_string<T...> fmt, T &&... args) -> void {
  const auto &vargs = fmt::make_format_args(args...);
  UARTVPrint(fmt, vargs);
}

auto random_float = [] { return static_cast<float>(rand()) / static_cast<float>(rand()); };

int32_t in_mV = 0;
int32_t in_mA = 0;
int32_t out_mV = 0;
int32_t out_mA = 0;
int32_t in_mW = 0;
int32_t out_mW = 0;
uint8_t run_state = 0;
uint8_t duty = 1;

auto PrintData(repeating_timer_t *rt) -> bool {
  FMTDebug("VPV={}, IPV={}, PPV={}, VBAT={}, IBAT={}, PBAT={}, QBAT={}, SOC={}, RS={}, D={}\n",
           static_cast<float>(in_mV) / 1000.0F, static_cast<float>(in_mA) / 1000.0F,
           static_cast<float>(in_mW) / 1000.0F, static_cast<float>(out_mV) / 1000.0F,
           static_cast<float>(out_mA) / 1000.0F, static_cast<float>(out_mW) / 1000.0F, random_float(), random_float(),
           run_state, duty);
  return true;
}

auto UpdateADC(repeating_timer_t *rt) -> bool {
  constexpr int32_t voltage_gain = 13; // Voltage divider ratio
  constexpr int32_t current_offset = 1'024; // mV
  auto adc_26_mV = static_cast<int32_t>(rpp::adc::adc_26.ReadmV()); // Read all the ADC channels
  auto adc_27_mV = static_cast<int32_t>(rpp::adc::adc_27.ReadmV());
  auto adc_28_mV = static_cast<int32_t>(rpp::adc::adc_28.ReadmV());
  auto adc_29_mV = static_cast<int32_t>(rpp::adc::adc_29.ReadmV());
  in_mV = adc_26_mV * voltage_gain; // Do integer math to calculate the values
  in_mA = (adc_27_mV - current_offset) * 5 / 3; // This nice ratio is the result of simplifying the following:
  out_mV = adc_28_mV * voltage_gain;
  out_mA = (adc_29_mV - current_offset) * 5 / 3; // Gain 200 V/V * 1 / 6mΩ shunt * 0.5 divider ratio
  in_mW = in_mV * in_mA /
          1'000; // Then calculate power, dividing by 1,000 to remove the scaling ratio, keeping it in milliwatts
  out_mW = out_mV * out_mA / 1'000;
  return true;
}

auto main() -> int {
  stdio_init_all();

  constexpr auto led_pin = PICO_DEFAULT_LED_PIN;
  gpio_init(led_pin);
  gpio_set_dir(led_pin, GPIO_OUT);

//  for (int i = 0; i < 10; ++i) {
//  while (true) {
  gpio_put(led_pin, true);
  sleep_ms(1000);
  gpio_put(led_pin, false);
  sleep_ms(1000);
//  }

  rpp::adc::adc_26.Init().ReferenceVoltage(3300);  // Configure all the ADC pins
  rpp::adc::adc_27.Init().ReferenceVoltage(3300);
  rpp::adc::adc_28.Init().ReferenceVoltage(3300);
  rpp::adc::adc_29.Init().ReferenceVoltage(3300);

  repeating_timer_t timer_print;                                       // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(100, PrintData, nullptr,
                              &timer_print))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(10, UpdateADC, nullptr,
                              &timer_adc))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  srand(64);
  pwm_01.Initialize().Frequency(425'000).DutyCycle(duty * 10'000).DeadBand(1).Disable();
  std::array<char, 512> buf{};  // String buffer

  while (true) {
    auto in_str = GetLine(buf, '\r');                   // Read in some serial data if available
    if (!in_str.empty()) {                              // If it is available, it will be in the string_view that sits within buf
      std::string_view numbers = {&buf[1],
                                  buf.end()};  // Grab everything after the first character as a single string which contains a number
      int argument;                                     // Variable which the number sent over serial will be read into

      auto [ptr, error] = std::from_chars(numbers.data(), numbers.end(),
                                          argument);  // Attempt to extract a number from the string

      if (error == std::errc())
        [[likely]]  // If there were no issues
              FMTDebug("Attempting to change {} to {}\n", buf[0], argument);
      else
        [[unlikely]]  // There was some problem with parsing the number
              FMTDebug("Invalid argument, trouble parsing number\n");

      switch (buf[0]) {                                                                 // Decided what to do with the number based on the first character given
        case ('S'): {                                                                   // Change run state
          run_state = (argument == std::clamp(argument, 0, 3)) ? argument
                                                               : run_state;  // If the argument is within the valid range, change the run state to the argument, otherwise do nothing
          if (run_state >= 1) {
            pwm_01.Enable();
            pwm_01.DutyCycle(duty * 10'000);
          } else pwm_01.Disable();
          break;
        }
        case ('D'): {                                                          // Change duty cycle
          duty = (argument == std::clamp(argument, 1, 99)) ? argument
                                                           : duty;  // If the argument is within the valid range, change the duty to the argument, otherwise do nothing
          pwm_01.DutyCycle(duty * 10'000);
          break;
        }
      }
    }
  }
}