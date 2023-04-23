#ifndef FastPID_HPP
#define FastPID_HPP

#include <cstdint>

/*
  A fixed point PID controller with a 32-bit internal calculation pipeline.
*/
class FastPID {
 public:
  constexpr FastPID(float kp, float ki, float kd, float hz, int bits = 16, bool sign = false) {
    setCoefficients(kp, ki, kd, hz);
    setOutputConfig(bits, sign);
  }

  constexpr auto setCoefficients(float kp, float ki, float kd, float hz) -> bool {
    _p = floatToParam(kp);
    _i = floatToParam(ki / hz);
    _d = floatToParam(kd * hz);
    return !_cfg_err;
  }

  constexpr auto setOutputConfig(int bits, bool sign) -> bool {
    // Set output bits
    if (bits > 16 || bits < 1) {
      setCfgErr();
    } else {
      if (bits == 16) {
        _outmax = (0xFFFFULL >> (17 - bits)) * parameter_multipier_;
      } else {
        _outmax = (0xFFFFULL >> (16 - bits)) * parameter_multipier_;
      }
      if (sign) {
        _outmin = -((0xFFFFULL >> (17 - bits)) + 1) * parameter_multipier_;
      } else {
        _outmin = 0;
      }
    }
    return !_cfg_err;
  }

  bool setOutputRange(int16_t min, int16_t max);

  void clear();

  int16_t step(int16_t sp, int16_t fb);

  bool err() const { return _cfg_err; }

// private:
  constexpr auto floatToParam(float in) -> uint32_t {
    if (in > parameter_multipier_ || in < 0) {
      _cfg_err = true;
      return 0;
    }

    uint32_t param = in * parameter_multipier_;

    if (in != 0 && param == 0) {
      _cfg_err = true;
      return 0;
    }

    return param;
  }

  constexpr auto setCfgErr() -> void {
    _cfg_err = true;
    _p = _i = _d = 0;
  }

  // Configuration
  uint32_t _p = 0;
  uint32_t _i = 0;
  uint32_t _d = 0;
  int64_t _outmax = 0;
  int64_t _outmin = 0;
  bool _cfg_err = false;

  // State
  int16_t _last_sp = 0;
  int16_t _last_out = 0;
  int64_t _sum = 0;
  int32_t _last_err = 0;

  static constexpr int64_t integral_max_ = INT32_MAX;
  static constexpr int64_t integral_min_ = INT32_MIN;

  static constexpr int32_t derivative_max_ = INT16_MAX;
  static constexpr int32_t derivative_min_ = INT16_MIN;

  static constexpr auto parameter_shift_ = 8;
  static constexpr auto parameter_bits_ = 16;

  static constexpr auto parameter_max_ = ((0x1ULL << parameter_bits_) - 1) >> parameter_shift_;
  static constexpr int64_t parameter_multipier_ = ((0x1ULL << parameter_bits_)) >> (parameter_bits_ - parameter_shift_);
};

#endif  // FastPID_HPP
