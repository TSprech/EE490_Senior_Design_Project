//
// Created by TSprech on 5/1/2023
//

#ifndef FASTPID_IPP
#define FASTPID_IPP

constexpr auto pid::FastPID::ChangeCoefficients(float kp, float ki, float kd, float hz) -> std::expected<void, FastPIDError> {
  TRY(auto p = FloatToFixed(kp)) p_ = p.value();
  CATCH return std::unexpected(p.error());

  TRY(auto i = FloatToFixed(ki / hz)) i_ = i.value();
  CATCH return std::unexpected(i.error());

  TRY(auto d = FloatToFixed(kd * hz)) d_ = d.value();
  CATCH return std::unexpected(d.error());
  return {};  // Everything succeeded
}

constexpr auto pid::FastPID::Initialize(float kp, float ki, float kd, float hz, bool sign) -> std::expected<void, FastPIDError> {
  TRY(auto value_error = ChangeCoefficients(kp, ki, kd, hz));
  CATCH return std::unexpected(value_error.error());
  ConfigureOutput(sign);
  return {};
}

constexpr auto pid::FastPID::ConfigureOutput(bool sign) -> void {
  outmax_ = (sign) ? (0xFFFFULL >> 1) * parameter_multiplier_ : 0xFFFFULL * parameter_multiplier_;
  outmin_ = (sign) ? -((0xFFFFULL >> 1) + 1) * parameter_multiplier_ : 0;
}

constexpr auto pid::FastPID::FloatToFixed(float in) -> std::expected<uint32_t, FastPIDError> {
  if (in > parameter_max_ || in < 0) {
    return std::unexpected(FastPIDError::invalid_range);
  }

  auto param = static_cast<uint32_t>(in * parameter_multiplier_);

  if (in != 0 && param == 0) {
    return std::unexpected(FastPIDError::zero_coefficient);
  }

  return param;
}

#endif  // FASTPID_IPP
