#include <algorithm>
#include <charconv>

#include "ADC.hpp"
#include "GPIO.hpp"
#include "GetLine.hpp"
#include "RP2040_PWM.hpp"
#include "MPPT.hpp"
#include "TypedUnits.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "pico/stdlib.h"
#include "tinyfsm.hpp"
#include <concepts>
#include <functional>

#include "ConditionalFunction.hpp"

using namespace units::literals;

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
    this->current_charge_ += current * period;
  }

 private:
  const units::charge::milliampere_hour_i32_t battery_rating_;
  const units::charge::millicoulomb_i32_t full_charge_;
  units::charge::millicoulomb_i32_t current_charge_;
};

int32_t in_mV = 0;
int32_t in_mA = 0;
int32_t out_mV = 0;
int32_t out_mA = 0;
int32_t in_mW = 0;
int32_t out_mW = 0;
uint32_t duty = 1 * 10'000;

constexpr auto led_pin = PICO_DEFAULT_LED_PIN;
auto pwm_01 = pwm::PWMManager<0, 1>(true);

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

BatteryManager bm(3_Ah_i32, 300_C_i32);

auto mppt_call = cfunc::FunctionManager(mppt::IncrementalConductance);
auto coulomb_counter_call = cfunc::FunctionManager([manager = &bm](auto current, auto period) -> void { manager->Current(current,period); });

struct RS0 : tinyfsm::Event {};
struct RS1 : tinyfsm::Event {};
struct RS2 : tinyfsm::Event {};
struct RS3 : tinyfsm::Event {};

/* Finite State Machine base class */
class RunStateFSM : public tinyfsm::Fsm<RunStateFSM> {
 public:
  /* react on all events and do nothing by default */
  virtual void react(RS0 const &) {}

  virtual void react(RS1 const &) {}

  virtual void react(RS2 const &) {}

  virtual void react(RS3 const &) {}

  virtual void react(tinyfsm::Event const &) {}

  virtual void entry(){};

  virtual void exit(){};

  static void reset(){};

  static auto current_state() -> uint8_t {
    return state;
  }

  static auto current_state(uint8_t new_state) -> void {
    state = new_state;
  }

  inline static uint8_t state = 0;
};

class RunState0;
class RunState1;
class RunState2;
class RunState3;

class RunState0 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 0\n");
    RunStateFSM::current_state(0);
    pwm_01.Disable();
    // TODO: Disable MOSFETs
  }

  void exit() override {
    FMTDebug("EXIT: Run State 0\n");
  }

  void react(RS0 const &) override {}

  void react(RS1 const &) override {
    transit<RunState1>();
  }

  void react(RS2 const &) override {
    transit<RunState2>();
  }

  void react(RS3 const &) override {
    transit<RunState3>();
  }
};

class RunState1 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 1\n");
    RunStateFSM::current_state(1);
  }

  void exit() override {
    FMTDebug("EXIT: Run State 1\n");
  }


  void react(RS0 const &) override {
    transit<RunState0>();
  }

  void react(RS1 const &) override {}

  void react(RS2 const &) override {
    transit<RunState2>();
  }

  void react(RS3 const &) override {
    transit<RunState3>();
  }
};

class RunState2 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 1\n");
    RunStateFSM::current_state(2);
    pwm_01.Enable();
    pwm_01.DutyCycle(duty * 10'000);
    coulomb_counter_call.Enable();
  }

  void exit() override {
    FMTDebug("EXIT: Run State 1\n");
    coulomb_counter_call.Disable();
  }

  void react(RS0 const &) override {
    transit<RunState0>();
  }

  void react(RS1 const &) override {
    transit<RunState1>();
  }

  void react(RS2 const &) override {}

  void react(RS3 const &) override {
    transit<RunState3>();
  }
};

class RunState3 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 1\n");
    RunStateFSM::current_state(3);
    pwm_01.Enable();
    pwm_01.DutyCycle(duty * 10'000);
    coulomb_counter_call.Enable();
    mppt_call.Enable();
  }

  void exit() override {
    FMTDebug("EXIT: Run State 1\n");
    mppt_call.Disable();
    coulomb_counter_call.Disable();
  }

  void react(RS0 const &) override {
    transit<RunState0>();
  }

  void react(RS1 const &) override {
    transit<RunState1>();
  }

  void react(RS2 const &) override {
    transit<RunState2>();
  }

  void react(RS3 const &) override {}
};

FSM_INITIAL_STATE(RunStateFSM, RunState0)

//static_assert(units::charge::millicoulomb_i32_t{units::charge::milliampere_hour_i32_t{3000}}.value() == 1);

auto random_float = [] { return static_cast<float>(rand()) / static_cast<float>(rand()); };

auto PrintData(repeating_timer_t *rt) -> bool {
  FMTDebug("VPV={}, IPV={}, PPV={}, VBAT={}, IBAT={}, PBAT={}, QBAT={}, SOC={}, RS={}, D={}\n",
           static_cast<float>(in_mV) / 1000.0F, static_cast<float>(in_mA) / 1000.0F,
           static_cast<float>(in_mW) / 1000.0F, static_cast<float>(out_mV) / 1000.0F,
           static_cast<float>(out_mA) / 1000.0F, static_cast<float>(out_mW) / 1000.0F, bm.Charge().value(), static_cast<float>(bm.ChargeState()),
           RunStateFSM::current_state(), static_cast<float>(duty) / 10'000.0F);
  return true;
}

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

auto duty_cycle = LimitedInt<int32_t>(0, 1'000'000);

auto UpdateADC(repeating_timer_t *rt) -> bool {
  rpp::gpio::gpio_2.Write(rpp::gpio::Levels::high);
  constexpr int32_t voltage_gain = 13;                               // Voltage divider ratio
  constexpr int32_t current_offset = 1'024;                          // mV
  auto adc_26_mV = static_cast<int32_t>(rpp::adc::adc_26.ReadmV());  // Read all the ADC channels
  auto adc_27_mV = static_cast<int32_t>(rpp::adc::adc_27.ReadmV());
  auto adc_28_mV = static_cast<int32_t>(rpp::adc::adc_28.ReadmV());
  auto adc_29_mV = static_cast<int32_t>(rpp::adc::adc_29.ReadmV());
  in_mV = adc_26_mV * voltage_gain;              // Do integer math to calculate the values
  in_mA = (adc_27_mV - current_offset) * 5 / 3;  // This nice ratio is the result of simplifying the following:
  out_mV = adc_28_mV * voltage_gain;
  out_mA = (adc_29_mV - current_offset) * 5 / 3;  // Gain 200 V/V * 1 / 6mΩ shunt * 0.5 divider ratio
  in_mW = in_mV * in_mA / 1'000;                  // Then calculate power, dividing by 1,000 to remove the scaling ratio, keeping it in milliwatts
  out_mW = out_mV * out_mA / 1'000;
  coulomb_counter_call(units::current::milliampere_i32_t{1'000}, 10_ms_i32);
  if (run_state == 3) {
    auto change = mppt::IncrementalConductance(units::voltage::millivolt_i32_t{in_mV}, units::current::milliampere_i32_t{in_mA});
    duty += change * 20;
  }
  rpp::gpio::gpio_2.Write(rpp::gpio::Levels::low);
  return true;
}

auto DispatchRunState(uint8_t state) -> void {
  switch (state) {
    case 0:
      RunStateFSM::dispatch(RS0());
      break;
    case 1:
      RunStateFSM::dispatch(RS1());
      break;
    case 2:
      RunStateFSM::dispatch(RS2());
      break;
    case 3:
      RunStateFSM::dispatch(RS3());
      break;
    default:
      break;
  }
}

auto main() -> int {
  stdio_init_all();
  gpio_init(led_pin);
  gpio_set_dir(led_pin, GPIO_OUT);

  rpp::gpio::gpio_2.Init().Direction(rpp::gpio::Directions::output).Pull(rpp::gpio::Pulls::disable).Write(rpp::gpio::Levels::low);

  //  RunStateFSM::start();
  //  while (true) {
  //    RunStateFSM::dispatch(RS1());
  //    sleep_ms(1000);
  //    RunStateFSM::dispatch(RS0());
  //    sleep_ms(1000);
  //  }

  rpp::adc::adc_26.Init().ReferenceVoltage(3300);  // Configure all the ADC pins
  rpp::adc::adc_27.Init().ReferenceVoltage(3300);
  rpp::adc::adc_28.Init().ReferenceVoltage(3300);
  rpp::adc::adc_29.Init().ReferenceVoltage(3300);

  repeating_timer_t timer_print;                                       // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(100, PrintData, nullptr, &timer_print))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  repeating_timer_t timer_adc;                                      // Timer used to manage the feedback loop callback
  if (!add_repeating_timer_ms(10, UpdateADC, nullptr, &timer_adc))  // Create a timer which calls the print function at the specified rate
    FMTDebug("Failed to create feedback loop timer\n");

  srand(64);
  pwm_01.Initialize().Frequency(425'000).DutyCycle(duty).DeadBand(1).Disable();
  std::array<char, 512> buf{};  // String buffer

  while (true) {
    auto in_str = GetLine(buf, '\r');  // Read in some serial data if available
    if (!in_str.empty()) {             // If it is available, it will be in the string_view that sits within buf
      std::string_view numbers = {&buf[1], buf.end()};  // Grab everything after the first character as a single string which contains a number
      int argument;                            // Variable which the number sent over serial will be read into

      auto [ptr, error] = std::from_chars(numbers.data(), numbers.end(), argument);  // Attempt to extract a number from the string

      if (error == std::errc()) [[likely]]  // If there were no issues
        FMTDebug("Attempting to change {} to {}\n", buf[0], argument);
      else [[unlikely]]  // There was some problem with parsing the number
        FMTDebug("Invalid argument, trouble parsing number\n");

      switch (buf[0]) {                                                                 // Decided what to do with the number based on the first character given
        case ('S'): {                                                                   // Change run state
          run_state = (argument == std::clamp(argument, 0, 3)) ? argument : run_state;  // If the argument is within the valid range, change the run state to the argument, otherwise do nothing
          DispatchRunState(run_state);
          break;
        }
        case ('D'): {                                                          // Change duty cycle
          duty = (argument == std::clamp(argument, 1, 99)) ? argument : duty;  // If the argument is within the valid range, change the duty to the argument, otherwise do nothing
          duty *= 10'000;
          pwm_01.DutyCycle(duty);
          break;
        }
        case ('T'): {
          run_state = 0;
          pwm_01.Disable();
        }
      }
    }
  }
}