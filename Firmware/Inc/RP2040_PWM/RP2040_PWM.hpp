/****************************************************************************************************************************
  RP2040_PWM.h
  For RP2040 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/RP2040_PWM
  Licensed under MIT license

  Modified by Trey Sprecher

  Version: 1.8.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      21/09/2021 Initial coding for RP2040 using ArduinoCore-mbed or arduino-pico core
  1.0.1   K.Hoang      24/09/2021 Fix bug generating wrong frequency
  1.0.2   K.Hoang      04/10/2021 Fix bug not changing frequency dynamically
  1.0.3   K.Hoang      05/10/2021 Not reprogram if same PWM frequency. Add PIO strict `lib_compat_mode`
  1.0.4   K Hoang      22/10/2021 Fix platform in library.json for PIO
  1.0.5   K Hoang      06/01/2022 Permit changing dutyCycle and keep same frequency on-the-fly
  1.1.0   K Hoang      24/02/2022 Permit PWM output for both channels of PWM slice. Use float instead of double
  1.1.1   K Hoang      06/03/2022 Fix compiler warnings. Display informational warning when debug level > 3
  1.2.0   K Hoang      16/04/2022 Add manual setPWM function to use in wafeform creation
  1.3.0   K Hoang      16/04/2022 Add setPWM_Int function for optional uint32_t dutycycle = real_dutycycle * 1000
  1.3.1   K Hoang      11/09/2022 Add minimal example `PWM_Basic`
  1.4.0   K Hoang      15/10/2022 Fix glitch when changing dutycycle. Adjust MIN_PWM_FREQUENCY/MAX_PWM_FREQUENCY dynamically
  1.4.1   K Hoang      21/01/2023 Add `PWM_StepperControl` example
  1.5.0   K Hoang      24/01/2023 Add `PWM_manual` example and functions
  1.6.0   K Hoang      26/01/2023 Optimize speed with new `setPWM_manual_Fast` function
  1.7.0   K Hoang      31/01/2023 Add PushPull mode and related examples
  1.8.0   T Sprech     02/04/2023 Ported to pico-sdk and C++ style
*****************************************************************************************************************************/

#ifndef RP2040_PWM_H
#define RP2040_PWM_H

#include <stdint.h>
#define RP2040_PWM_VERSION "RP2040_PWM V1.8.0"
#define RP2040_PWM_VERSION_MAJOR 1
#define RP2040_PWM_VERSION_MINOR 8
#define RP2040_PWM_VERSION_PATCH 0
#define RP2040_PWM_VERSION_INT 1008000

#include <array>
#include <cmath>

#include "PWM_Generic_Debug.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

//#define MAX_PWM_FREQUENCY (62500000.0f)
//
//// For 125MHz CPU. To adjust according to actual CPU Frequency
//#define MIN_PWM_FREQUENCY (7.5f)
//
//#if !defined(NUM_PWM_SLICES)
//#define NUM_PWM_SLICES 8
//#endif
//
//namespace pwm {  //! Changed
//  struct PWM_slice {
//    float freq = 0;
//    float channelA_div = 0;
//    float channelB_div = 0;
//    bool channelA_Active = false;
//    bool channelB_Active = false;
//  };
//
//  //! Changed
//  constinit std::array<PWM_slice, NUM_PWM_SLICES> PWM_slice_data = {};
//
//  ////////////////////////////////////////
//
//  // Not using float for waveform creating
//  struct PWM_slice_manual {
//    uint16_t channelA_div = 0;
//    uint16_t channelB_div = 0;
//    bool channelA_Active = false;
//    bool channelB_Active = false;
//    bool initialized = false;
//  };
//
//  // Default to 0% PWM
//  constinit std::array<PWM_slice_manual, NUM_PWM_SLICES> PWM_slice_manual_data = {};
//
//  ////////////////////////
//
//  class RP2040_PWM {
//   public:
//    RP2040_PWM(uint8_t pin, float frequency, float dutycycle, bool phaseCorrect = false);
//
//    bool setPWM();
//
//    // To be called only after previous complete setPWM_manual with top and div params
//    // by checking PWM_slice_manual_data[_slice_num].initialized == true;
//    bool setPWM_manual(uint8_t pin, uint16_t level);
//
//    // To be called only after previous complete setPWM_manual with top and div params
//    // No checking of PWM_slice_manual_data[_slice_num].initialized == true;
//    // No more output to both channels
//    bool setPWM_manual_Fast(uint8_t pin, uint16_t level);
//
//    // To be called only after previous complete setPWM_manual with top and div params
//    // by checking PWM_slice_manual_data[_slice_num].initialized == true;
//    bool setPWM_DCPercentage_manual(uint8_t pin, float DCPercentage);
//
//    bool setPWM_manual(uint8_t pin, uint16_t top, uint8_t div, uint16_t level, bool phaseCorrect = false);
//
//    // dutycycle from 0-100,000 for 0%-100% to make use of 16-bit top register
//    // dutycycle = real_dutycycle * 1000 for better accuracy
//    // pinA and pinB must belong to the same channel. Check https://github.com/khoih-prog/RP2040_PWM#programmers-model
//    // Must use phasecorrect mode here
//    bool setPWMPushPull_Int(uint8_t pinA, uint8_t pinB, float frequency, uint32_t dutycycle);
//
//    // dutycycle from 0-100,000 for 0%-100% to make use of 16-bit top register
//    // dutycycle = real_dutycycle * 1000 for better accuracy
//    bool setPWM_Int(uint8_t pin, float frequency, uint32_t dutycycle, bool phaseCorrect = false);
//
//    bool setPWM(uint8_t pin, float frequency, float dutycycle, bool phaseCorrect = false) {
//      return setPWM_Int(pin, frequency, dutycycle * 1000, phaseCorrect);
//    }
//
//    bool setPWM_Period(uint8_t pin, float period_us, float dutycycle, bool phaseCorrect = false) {
//      return setPWM_Int(pin, 1000000.0f / period_us, dutycycle * 1000, phaseCorrect);
//    }
//
//    bool setPWMPushPull(uint8_t pinA, uint8_t pinB, float frequency, float dutycycle) {
//      return setPWMPushPull_Int(pinA, pinB, frequency, dutycycle * 1000);
//    }
//
//    bool ssetPWMPushPull_Period(uint8_t pinA, uint8_t pinB, float period_us, float dutycycle) {
//      return setPWMPushPull_Int(pinA, pinB, 1000000.0f / period_us, dutycycle * 1000);
//    }
//
//    void enablePWM() {
//      pwm_set_enabled(_slice_num, true);
//      _enabled = true;
//    }
//
//    void disablePWM() {
//      pwm_set_enabled(_slice_num, false);
//      _enabled = false;
//    }
//
//    inline uint32_t get_TOP() {
//      return _PWM_config.top;
//    }
//
//    inline uint32_t get_DIV() {
//      return _PWM_config.div;
//    }
//
//    inline float getActualFreq() {
//      return _actualFrequency;
//    }
//
//    inline uint32_t get_freq_CPU() {
//      return freq_CPU;
//    }
//
//    inline uint32_t getActualDutyCycle() {
//      // From 0-100,000
//      return _dutycycle;
//    }
//
//    inline uint32_t getPin() {
//      return _pin;
//    }
//
//    ////////////////////////
//
//   private:
//    pwm_config _PWM_config;
//    uint32_t freq_CPU;
//
//    float _actualFrequency;
//    float _frequency;
//
//    // dutycycle from 0-100,000 for 0%-100% to make use of 16-bit top register
//    // dutycycle = real_dutycycle * 1000 for better accuracy
//    uint32_t _dutycycle;
//    //////////
//
//    uint8_t _pin;
//    uint8_t _slice_num;
//    bool _phaseCorrect;
//    bool _enabled;
//
//    // https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf, page 549
//    // https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__pwm.html
//
//    bool calc_TOP_and_DIV(float freq);
//  };
//}  // namespace pwm
//
//



#include <cstdint>

template <typename T> // Thanks: https://stackoverflow.com/a/5732390
constexpr auto Map(T value, T in_min, T in_max, T out_min, T out_max) -> T {
//  auto slope = (out_max - out_min) / (in_max - in_min);
//  return out_min + slope * (value - in_min);
return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class PWMSlice {
 public:
  constexpr PWMSlice(uint8_t pin_a, uint8_t pin_b) : pin_a_(pin_a), pin_b_(pin_b) {}

  auto Initialize() -> void {
    cpu_frequency_hz_ = clock_get_hz(clk_sys);

    gpio_set_function(pin_a_, GPIO_FUNC_PWM);
    gpio_set_function(pin_b_, GPIO_FUNC_PWM);

    _slice_num = pwm_gpio_to_slice_num(pin_a_);

    _enabled = false;
  }

  void Enable() {
    pwm_set_enabled(_slice_num, true);
    _enabled = true;
  }

  void Disable() {
    pwm_set_enabled(_slice_num, false);
    _enabled = false;
  }

  [[nodiscard]] auto Top() const -> uint32_t {
    return pwm_hw->slice[_slice_num].top;
  }

  bool Frequency(uint32_t frequency_hz) {
    _PWM_config.div = 1;

    // Formula => PWM_Freq = ( F_CPU ) / [ ( TOP + 1 ) * ( DIV + DIV_FRAC/16) ]
    _PWM_config.top = (cpu_frequency_hz_ / frequency_hz / _PWM_config.div) - 1;

    _actualFrequency = cpu_frequency_hz_ / ((_PWM_config.top + 1) * _PWM_config.div);

    // Compensate half freq if _phaseCorrect
//    if (_phaseCorrect)
      _PWM_config.top /= 2;

      pwm_hw->slice[_slice_num].top = _PWM_config.top;
      pwm_hw->slice[_slice_num].div = _PWM_config.div;
    //    PWM_LOGINFO3("_PWM_config.top =", _PWM_config.top, ", _actualFrequency =", _actualFrequency);
    return true;
  }

  static constexpr uint32_t max_duty = 1'000'000;

  // Duty cycle from 0 to 1,000,000 to represent 0 to 1
  bool DutyCycle(uint32_t dutycycle, uint8_t deadband = 1) {
      if (dutycycle > max_duty) [[unlikely]] dutycycle = max_duty;
      auto level = Map<uint32_t>(dutycycle, 0, max_duty, 0, Top());

      uint16_t a_level = level - deadband;
//      uint16_t b_level = level + deadband;
      uint16_t b_level = deadband; // TODO: Uhhhhh?

      // This makes sure there isn't a condition where there is no PWM happening (one channel stuck at 100% or 0% duty)
      if ((a_level == 0 || b_level == 0 || a_level == Top() || b_level == Top())) {
        a_level = 0;
        b_level = 0;
      }

      pwm_set_both_levels(_slice_num, a_level, b_level);
      return true;
    }

 private:
  const uint8_t pin_a_;
  const uint8_t pin_b_;

  static constexpr uint32_t min_frequency_hz = 8;

  pwm_config _PWM_config;
  uint32_t cpu_frequency_hz_;

  uint32_t _actualFrequency;
  float _frequency;

  // dutycycle from 0-100,000 for 0%-100% to make use of 16-bit top register
  // dutycycle = real_dutycycle * 1000 for better accuracy
  uint32_t _dutycycle;

  uint8_t _pin;
  uint8_t _slice_num;
  bool _phaseCorrect;
  bool _enabled;
};

#endif  // RP2040_PWM_H
