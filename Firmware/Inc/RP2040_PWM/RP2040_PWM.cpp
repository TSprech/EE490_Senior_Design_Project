//// TSprech 2023/04/03 13:02:20
//
//#include "RP2040_PWM.hpp"
//
//pwm::RP2040_PWM::RP2040_PWM(uint8_t pin, float frequency, float dutycycle, bool phaseCorrect) {
//#if defined(F_CPU)  // TODO: Read this from clocks?
//  freq_CPU = F_CPU;
//#else
//  freq_CPU = 125000000;
//#endif
//
//  _pin = pin;
//  _frequency = frequency;
//  _dutycycle = dutycycle * 1000;
//
//  _phaseCorrect = phaseCorrect;
//
//  if (!calc_TOP_and_DIV(frequency)) {
//    _frequency = 0;
//  } else {
//    _frequency = frequency;
//  }
//
//  _enabled = false;
//}
//
//bool pwm::RP2040_PWM::setPWM() {
//  return setPWM_Int(_pin, _frequency, _dutycycle, _phaseCorrect);
//}
//
//bool pwm::RP2040_PWM::setPWM_manual(uint8_t pin, uint16_t level) {
//  _pin = pin;
//
//  // Limit level <= _PWM_config.top
//  if (level > _PWM_config.top)
//    level = _PWM_config.top;
//
//  _dutycycle = ((uint32_t) level * 100000 / _PWM_config.top);
//
//  gpio_set_function(_pin, GPIO_FUNC_PWM);
//
//  _slice_num = pwm_gpio_to_slice_num(_pin);
//
//  if (!PWM_slice_manual_data[_slice_num].initialized) {
//    //      PWM_LOGERROR1("Error, not initialized for PWM pin = ", _pin);
//
//    return false;
//  }
//
//  pwm_set_gpio_level(_pin, level);
//
//  // From v1.1.0
//  ////////////////////////////////
//
//  if ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_A) {
//    PWM_slice_manual_data[_slice_num].channelA_div = level;
//    PWM_slice_manual_data[_slice_num].channelA_Active = true;
//
//    // If B is active, set the data now
//    if (PWM_slice_manual_data[_slice_num].channelB_Active) {
//      pwm_set_chan_level(_slice_num, PWM_CHAN_B, PWM_slice_manual_data[_slice_num].channelB_div);
//    }
//  } else if ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_B) {
//    PWM_slice_manual_data[_slice_num].channelB_div = level;
//    PWM_slice_manual_data[_slice_num].channelB_Active = true;
//
//    // If A is active, set the data now
//    if (PWM_slice_manual_data[_slice_num].channelA_Active) {
//      pwm_set_chan_level(_slice_num, PWM_CHAN_A, PWM_slice_manual_data[_slice_num].channelA_div);
//    }
//  } else {
//    //      PWM_LOGERROR1("Error, not correct PWM pin = ", _pin);
//
//    return false;
//  }
//
//  pwm_set_enabled(_slice_num, true);
//
//  //    PWM_LOGINFO3("pin = ", _pin, ", PWM_CHAN =", pwm_gpio_to_channel(_pin));
//
//  ////////////////////////////////
//
//  _enabled = true;
//
//  //    PWM_LOGINFO7("PWM enabled, slice =", _slice_num, ", top =", _PWM_config.top, ", div =", _PWM_config.div, ", level =", level);
//
//  return true;
//}
//
//bool pwm::RP2040_PWM::setPWM_manual_Fast(uint8_t pin, uint16_t level) {
//  static uint16_t prev_level = 0;
//
//  if (prev_level != level) {
//    prev_level = level;
//    _dutycycle = ((uint32_t) level * 100000 / _PWM_config.top);
//  }
//
//  // Better @ 1597ns, reducing nearly 1.3us out of setPWM_manual() => 2889ns
//  hw_write_masked(&pwm_hw->slice[pwm_gpio_to_slice_num(pin)].cc,
//                  ((uint) level) << (pwm_gpio_to_channel(pin) ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB),
//                  pwm_gpio_to_channel(pin) ? PWM_CH0_CC_B_BITS : PWM_CH0_CC_A_BITS);
//
//  //    PWM_LOGINFO3("pin = ", _pin, ", PWM_CHAN =", pwm_gpio_to_channel(_pin));
//
//  ////////////////////////////////
//
//  //    PWM_LOGINFO7("PWM enabled, slice =", _slice_num, ", top =", _PWM_config.top, ", div =", _PWM_config.div, ", level =", level);
//
//  return true;
//}
//
//bool pwm::RP2040_PWM::setPWM_DCPercentage_manual(const uint8_t pin, float DCPercentage) {
//  uint16_t dutycycle_level = (DCPercentage * _PWM_config.top) / 100.0f;
//
//  // Convert to DCValue based on _PWM_config.top
//  return setPWM_manual(pin, dutycycle_level);
//}
//
//bool pwm::RP2040_PWM::setPWM_manual(const uint8_t pin, const uint16_t top, const uint8_t div, uint16_t level,
//                                    bool phaseCorrect) {
//  _pin = pin;
//
//  _phaseCorrect = phaseCorrect;
//
//  _PWM_config.top = top;
//  _PWM_config.div = div;
//
//  // Limit level <= top
//  if (level > top)
//    level = top;
//
//  _dutycycle = ((uint32_t) level * 100000 / top);
//
//  gpio_set_function(_pin, GPIO_FUNC_PWM);
//
//  _slice_num = pwm_gpio_to_slice_num(_pin);
//
//  pwm_config config = pwm_get_default_config();
//
//  // Set phaseCorrect
//  pwm_set_phase_correct(_slice_num, phaseCorrect);
//
//  pwm_config_set_clkdiv_int(&config, _PWM_config.div);
//  pwm_config_set_wrap(&config, _PWM_config.top);
//
//  // auto start running once configured
//  pwm_init(_slice_num, &config, true);
//  pwm_set_gpio_level(_pin, level);
//
//  // Store and flag so that simpler setPWM_manual() can be called without top and div
//  PWM_slice_manual_data[_slice_num].initialized = true;
//
//  // From v1.1.0
//  ////////////////////////////////
//  // Update PWM_slice_manual_data[]
//
//  if ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_A) {
//    PWM_slice_manual_data[_slice_num].channelA_div = level;
//    PWM_slice_manual_data[_slice_num].channelA_Active = true;
//
//    // If B is active, set the data now
//    if (PWM_slice_manual_data[_slice_num].channelB_Active) {
//      pwm_set_chan_level(_slice_num, PWM_CHAN_B, PWM_slice_manual_data[_slice_num].channelB_div);
//    }
//  } else if ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_B) {
//    PWM_slice_manual_data[_slice_num].channelB_div = level;
//    PWM_slice_manual_data[_slice_num].channelB_Active = true;
//
//    // If A is active, set the data now
//    if (PWM_slice_manual_data[_slice_num].channelA_Active) {
//      pwm_set_chan_level(_slice_num, PWM_CHAN_A, PWM_slice_manual_data[_slice_num].channelA_div);
//    }
//  } else {
//    //      PWM_LOGERROR1("Error, not correct PWM pin = ", _pin);
//
//    return false;
//  }
//
//  pwm_set_enabled(_slice_num, true);
//
//  //    PWM_LOGINFO3("pin = ", _pin, ", PWM_CHAN =", pwm_gpio_to_channel(_pin));
//
//  ////////////////////////////////
//
//  _enabled = true;
//
//  //    PWM_LOGINFO7("PWM enabled, slice =", _slice_num, ", top =", _PWM_config.top, ", div =", _PWM_config.div, ", level =", level);
//
//  return true;
//}
//
//bool pwm::RP2040_PWM::setPWMPushPull_Int(const uint8_t pinA, const uint8_t pinB, const float frequency,
//                                         const uint32_t dutycycle) {
//  bool newFreq = false;
//  bool newDutyCycle = false;
//
//  _pin = pinA;
//
//  _slice_num = pwm_gpio_to_slice_num(pinA);
//
//  if (pwm_gpio_to_slice_num(pinB) != _slice_num) {
//    //      PWM_LOGERROR3("Error, not correct PWM push-pull pair of pins = ", pinA, "and", pinB);
//
//    return false;
//  }
//
//  if ((frequency <= ((float) MAX_PWM_FREQUENCY * freq_CPU / 125000000))
//      && (frequency >= ((float) MIN_PWM_FREQUENCY * freq_CPU / 125000000))) {
//    if ((_frequency != frequency) || !_phaseCorrect) {
//      // Must change before calling calc_TOP_and_DIV()
//      _phaseCorrect = true;
//
//      // To compensate phasecorrect half frequency
//      if (!calc_TOP_and_DIV(frequency)) {
//        _frequency = 0;
//      } else {
//        _frequency = frequency;
//        _dutycycle = dutycycle;
//
//        newFreq = true;
//
//        //          PWM_LOGINFO3("Changing PWM frequency to", frequency, "and dutyCycle =", (float) _dutycycle / 1000);
//      }
//    } else if (_enabled) {
//      if (_dutycycle != dutycycle) {
//        _dutycycle = dutycycle;
//        newDutyCycle = true;
//
//        //          PWM_LOGINFO3("Changing PWM DutyCycle to", (float) _dutycycle / 1000, "and keeping frequency =", _frequency);
//
//      } else {
//        //          PWM_LOGINFO3("No change, same PWM frequency =", frequency, "and dutyCycle =", (float) _dutycycle / 1000);
//      }
//    }
//
//    if ((!_enabled) || newFreq || newDutyCycle) {
//      gpio_set_function(pinA, GPIO_FUNC_PWM);
//      gpio_set_function(pinB, GPIO_FUNC_PWM);
//
//      pwm_config config = pwm_get_default_config();
//
//      pwm_config_set_clkdiv_int(&config, _PWM_config.div);
//      pwm_config_set_wrap(&config, _PWM_config.top);
//
//      if (newDutyCycle) {
//        // KH, to fix glitch when changing dutycycle from v1.4.0
//        // Check https://github.com/khoih-prog/RP2040_PWM/issues/10
//        // From pico-sdk/src/rp2_common/hardware_pwm/include/hardware/pwm.h
//        // Only take effect after the next time the PWM slice wraps
//        // (or, in phase-correct mode, the next time the slice reaches 0).
//        // If the PWM is not running, the write is latched in immediately
//        //pwm_set_wrap(uint slice_num, uint16_t wrap)
//        pwm_set_wrap(_slice_num, _PWM_config.top);
//      } else {
//        // auto start running once configured
//        pwm_init(_slice_num, &config, true);
//      }
//
//      uint32_t PWM_level = (_PWM_config.top * (_dutycycle / 2)) / 50000;
//
//      // To avoid uint32_t overflow and still keep accuracy as _dutycycle max = 100,000 > 65536 of uint16_t
//      pwm_set_gpio_level(pinA, PWM_level);
//      pwm_set_gpio_level(pinB, PWM_level);
//
//      // From v1.1.0
//      ////////////////////////////////
//      // Update PWM_slice_data[]
//      PWM_slice_data[_slice_num].freq = _frequency;
//
//      // Set phaseCorrect
//      pwm_set_phase_correct(_slice_num, true);
//
//      pwm_set_output_polarity(_slice_num, false, true);
//
//      if (((pwm_gpio_to_channel(_pin)) == PWM_CHAN_A) || ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_B)) {
//        PWM_slice_data[_slice_num].channelA_div = PWM_level;
//        PWM_slice_data[_slice_num].channelB_div = _PWM_config.top - PWM_level;
//
//        PWM_slice_data[_slice_num].channelA_Active = true;
//        PWM_slice_data[_slice_num].channelB_Active = true;
//
//        // If A is active, set the data now
//        if (PWM_slice_data[_slice_num].channelA_Active) {
//          pwm_set_chan_level(_slice_num, PWM_CHAN_A, PWM_slice_data[_slice_num].channelA_div);
//        }
//
//        // If B is active, set the data now
//        if (PWM_slice_data[_slice_num].channelB_Active) {
//          pwm_set_chan_level(_slice_num, PWM_CHAN_B, PWM_slice_data[_slice_num].channelB_div);
//        }
//      } else {
//        //          PWM_LOGERROR1("Error, not correct PWM pin = ", _pin);
//
//        return false;
//      }
//
//      pwm_set_enabled(_slice_num, true);
//
//      //        PWM_LOGINFO5("pinA = ", pinA, ", pinB = ", pinB, ", PWM_CHAN =", pwm_gpio_to_channel(_pin));
//
//      ////////////////////////////////
//
//      _enabled = true;
//
//      //        PWM_LOGINFO3("PWM enabled, slice = ", _slice_num, ", _frequency = ", _frequency);
//    }
//
//    return true;
//  } else
//    return false;
//}
//
//bool pwm::RP2040_PWM::setPWM_Int(uint8_t pin, float frequency, uint32_t dutycycle, bool phaseCorrect) {
//  bool newFreq = false;
//  bool newDutyCycle = false;
//
//  _phaseCorrect = phaseCorrect;
//
//  if ((frequency <= ((float) MAX_PWM_FREQUENCY * freq_CPU / 125000000))
//      && (frequency >= ((float) MIN_PWM_FREQUENCY * freq_CPU / 125000000))) {
//    _pin = pin;
//
//    if (_frequency != frequency) {
//      if (!calc_TOP_and_DIV(frequency)) {
//        _frequency = 0;
//      } else {
//        _frequency = frequency;
//        _dutycycle = dutycycle;
//
//        newFreq = true;
//
//        //          PWM_LOGINFO3("Changing PWM frequency to", frequency, "and dutyCycle =", (float) _dutycycle / 1000);
//      }
//    } else if (_enabled) {
//      if (_dutycycle != dutycycle) {
//        _dutycycle = dutycycle;
//        newDutyCycle = true;
//
//        //          PWM_LOGINFO3("Changing PWM DutyCycle to", (float) _dutycycle / 1000, "and keeping frequency =", _frequency);
//
//      } else {
//        //          PWM_LOGINFO3("No change, same PWM frequency =", frequency, "and dutyCycle =", (float) _dutycycle / 1000);
//      }
//    }
//
//    if ((!_enabled) || newFreq || newDutyCycle) {
//      gpio_set_function(_pin, GPIO_FUNC_PWM);
//
//      _slice_num = pwm_gpio_to_slice_num(_pin);
//
//      pwm_config config = pwm_get_default_config();
//
//      // Set phaseCorrect
//      pwm_set_phase_correct(_slice_num, phaseCorrect);
//
//      pwm_config_set_clkdiv_int(&config, _PWM_config.div);
//      pwm_config_set_wrap(&config, _PWM_config.top);
//
//      if (newDutyCycle) {
//        // KH, to fix glitch when changing dutycycle from v1.4.0
//        // Check https://github.com/khoih-prog/RP2040_PWM/issues/10
//        // From pico-sdk/src/rp2_common/hardware_pwm/include/hardware/pwm.h
//        // Only take effect after the next time the PWM slice wraps
//        // (or, in phase-correct mode, the next time the slice reaches 0).
//        // If the PWM is not running, the write is latched in immediately
//        //pwm_set_wrap(uint slice_num, uint16_t wrap)
//        pwm_set_wrap(_slice_num, _PWM_config.top);
//      } else {
//        // auto start running once configured
//        pwm_init(_slice_num, &config, true);
//      }
//
//      uint32_t PWM_level = (_PWM_config.top * (_dutycycle / 2)) / 50000;
//
//      // To avoid uint32_t overflow and still keep accuracy as _dutycycle max = 100,000 > 65536 of uint16_t
//      pwm_set_gpio_level(_pin, PWM_level);
//
//      // From v1.1.0
//      ////////////////////////////////
//      // Update PWM_slice_data[]
//      PWM_slice_data[_slice_num].freq = _frequency;
//
//      if ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_A) {
//        PWM_slice_data[_slice_num].channelA_div = PWM_level;
//        PWM_slice_data[_slice_num].channelA_Active = true;
//
//        // If B is active, set the data now
//        if (PWM_slice_data[_slice_num].channelB_Active) {
//          pwm_set_chan_level(_slice_num, PWM_CHAN_B, PWM_slice_data[_slice_num].channelB_div);
//        }
//      } else if ((pwm_gpio_to_channel(_pin)) == PWM_CHAN_B) {
//        PWM_slice_data[_slice_num].channelB_div = PWM_level;
//        PWM_slice_data[_slice_num].channelB_Active = true;
//
//        // If A is active, set the data now
//        if (PWM_slice_data[_slice_num].channelA_Active) {
//          pwm_set_chan_level(_slice_num, PWM_CHAN_A, PWM_slice_data[_slice_num].channelA_div);
//        }
//      } else {
//        //          PWM_LOGERROR1("Error, not correct PWM pin = ", _pin);
//
//        return false;
//      }
//
//      pwm_set_enabled(_slice_num, true);
//
//      //        PWM_LOGINFO3("pin = ", _pin, ", PWM_CHAN =", pwm_gpio_to_channel(_pin));
//
//      ////////////////////////////////
//
//      _enabled = true;
//
//      //        PWM_LOGINFO3("PWM enabled, slice = ", _slice_num, ", _frequency = ", _frequency);
//    }
//
//    return true;
//  } else
//    return false;
//}
//
//bool pwm::RP2040_PWM::calc_TOP_and_DIV(float freq) {
//  if (freq > 2000.0) {
//    _PWM_config.div = 1;
//  } else if (freq >= 200.0) {
//    _PWM_config.div = 10;
//  } else if (freq >= 20.0) {
//    _PWM_config.div = 100;
//  } else if (freq >= 10.0) {
//    _PWM_config.div = 200;
//  } else if (freq >= ((float) MIN_PWM_FREQUENCY * freq_CPU / 125000000)) {
//    _PWM_config.div = 255;
//  } else {
//    //      PWM_LOGERROR1("Error, freq must be >=", ((float) MIN_PWM_FREQUENCY * freq_CPU / 125000000));
//
//    return false;
//  }
//
//  // Formula => PWM_Freq = ( F_CPU ) / [ ( TOP + 1 ) * ( DIV + DIV_FRAC/16) ]
//  _PWM_config.top = (freq_CPU / freq / _PWM_config.div) - 1;
//
//  _actualFrequency = (freq_CPU) / ((_PWM_config.top + 1) * _PWM_config.div);
//
//  // Compensate half freq if _phaseCorrect
//  if (_phaseCorrect)
//    _PWM_config.top /= 2;
//
//  //    PWM_LOGINFO3("_PWM_config.top =", _PWM_config.top, ", _actualFrequency =", _actualFrequency);
//
//  return true;
//}
