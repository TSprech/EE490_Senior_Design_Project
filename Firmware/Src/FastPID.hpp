#ifndef FastPID_HPP
#define FastPID_HPP

#include <cstdint>

/*
  A fixed point PID controller with a 32-bit internal calculation pipeline.
*/
class FastPID {
 public:
  constexpr FastPID(float kp, float ki, float kd, float hz, bool sign = false) {
    ChangeCoefficients(kp, ki, kd, hz);
    ConfigureOutput(sign);
  }

  constexpr auto ChangeCoefficients(float kp, float ki, float kd, float hz) -> bool {
    p_ = floatToParam(kp);
    i_ = floatToParam(ki / hz);
    d_ = floatToParam(kd * hz);
    return !cfg_err_;
  }

  constexpr auto ConfigureOutput(bool sign) -> bool {
    outmin_ = (sign) ? -((0xFFFFULL >> 1) + 1) * parameter_multipier_ : 0;
    return !cfg_err_;
  }

  auto OutputSaturationRange(int16_t min, int16_t max) -> bool;

  auto Reset() -> void;

  auto Evaluate(int16_t sp, int16_t fb) -> int16_t;

  [[nodiscard]]
  auto Error() const -> bool { return cfg_err_; }

 private:
  constexpr auto floatToParam(float in) -> uint32_t {
    if (in > parameter_max_ || in < 0) {
        cfg_err_ = true;
      return 0;
    }

    auto param = static_cast<uint32_t>(in * parameter_multipier_);

    if (in != 0 && param == 0) {
      cfg_err_ = true;
      return 0;
    }

    return param;
  }

  constexpr auto setCfgErr() -> void {
    cfg_err_ = true;
    p_ = i_ = d_ = 0;
  }

  // Configuration
  uint32_t p_ = 0;
  uint32_t i_ = 0;
  uint32_t d_ = 0;
  int64_t outmax_ = (0xFFFFULL >> 1) * parameter_multipier_;
  int64_t outmin_ = 0;

  // State
  int64_t sum_ = 0;
  int32_t last_error_ = 0;
  int16_t last_set_point_ = 0;
  bool cfg_err_ = false;

  static constexpr int64_t integral_max_ = INT32_MAX;
  static constexpr int64_t integral_min_ = INT32_MIN;

  static constexpr int32_t derivative_max_ = INT16_MAX;
  static constexpr int32_t derivative_min_ = INT16_MIN;

  static constexpr uint8_t parameter_shift_ = 8;
  static constexpr uint8_t parameter_bits_ = 16;

  static constexpr auto parameter_max_ = ((0x1ULL << parameter_bits_) - 1) >> parameter_shift_;
  static constexpr int64_t parameter_multipier_ = ((0x1ULL << parameter_bits_)) >> (parameter_bits_ - parameter_shift_);
};

#endif  // FastPID_HPP
