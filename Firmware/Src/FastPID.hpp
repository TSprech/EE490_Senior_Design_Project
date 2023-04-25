#ifndef FastPID_HPP
#define FastPID_HPP

#include <cstdint>

namespace pid {
  /**
   * @brief A fixed point, integer based PID algorithm.
   */
  class FastPID {
   public:
    /**
     * @brief Constructor for a PID manager.
     * @param kp P term of the controller.
     * @param ki I term of the controller.
     * @param kd D term of the controller.
     * @param hz Frequency at which the @ref Evaluate method will be called at.
     * @warning It is the programmer's responsibility to call the @ref Evaluate function as well as ensure it is called at the rate specified in @ref hz.
     * @param sign Whether the output range will be signed or unsigned.
     * @note With sign = true, the range is -32, 768
     */
    constexpr FastPID(float kp, float ki, float kd, float hz, bool sign = false) {
      ChangeCoefficients(kp, ki, kd, hz);
      ConfigureOutput(sign);
    }

    /**
     * @brief Converts the floating point parameters to their fixed point counterparts.
     * @param kp P term of the controller.
     * @param ki I term of the controller.
     * @param kd D term of the controller.
     * @param hz Frequency at which the @ref Evaluate method will be called at.
     * @returns If there was a configuration error.
     */
    constexpr auto ChangeCoefficients(float kp, float ki, float kd, float hz) -> bool {
      p_ = FloatToFixed(kp);
      i_ = FloatToFixed(ki / hz);
      d_ = FloatToFixed(kd * hz);
      return !cfg_err_;
    }

    /**
     * @brief Sets the output range based on the sign.
     * @param sign True for signed output, false for unsigned output.
     * @returns If there was a configuration error.
     */
    constexpr auto ConfigureOutput(bool sign) -> bool {
      outmax_ = (sign) ? (0xFFFFULL >> 1) * parameter_multipier_ : 0xFFFFULL * parameter_multipier_;
      outmin_ = (sign) ? -((0xFFFFULL >> 1) + 1) * parameter_multipier_ : 0;
      return !cfg_err_;
    }

    /**
     * @brief Sets the limits at which the output will saturate at.
     * @param min The lower limit, any output calculated below this will be set to this value.
     * @param max The upper limit, any output calculated above this will be set to this value.
     * @returns If there was an error changing the limits.
     */
    auto OutputSaturationRange(int16_t min, int16_t max) -> bool;

    /**
     * @brief Clears the internal state of the PID controller.
     */
    auto Reset() -> void;

    /**
     * @brief Calculates the next output of the controller.
     * @param sp The set point trying to be reached, aka what value the system should be in.
     * @param fb The current value of the system being PID controlled, aka the system's current output.
     * @returns The calculated output of the controller, aka the system's next input.
     */
    [[nodiscard]] auto Evaluate(int16_t sp, int16_t fb) -> int16_t;

    /**
     * @returns If there has been an error.
     */
    [[nodiscard]] auto Error() const -> bool { return cfg_err_; }

   private:
    /**
     * @brief Converts a floating point number to a fixed point which can use integer math.
     * @param in The floating point value to convert.
     * @returns The converted value in a fixed point representation.
     */
    constexpr auto FloatToFixed(float in) -> uint32_t {
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

    /**
     * @brief Sets the configuration error and clears the P, I, and D values.
     */
    constexpr auto setCfgErr() -> void {
      cfg_err_ = true;
      p_ = i_ = d_ = 0;
    }

    // Configuration
    uint32_t p_ = 0;     /**< P constant */
    uint32_t i_ = 0;     /**< I constant */
    uint32_t d_ = 0;     /**< D constant */
    int64_t outmax_ = 0; /**< Upper saturation limit of the output */
    int64_t outmin_ = 0; /**< Lower saturation limit of the output */

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
}  // namespace pid

#endif  // FastPID_HPP
