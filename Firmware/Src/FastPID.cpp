#include "FastPID.hpp"
#include <algorithm>

void FastPID::clear() {
  last_set_point_ = 0;
  sum_ = 0;
  last_error_ = 0;
}

bool FastPID::setOutputRange(int16_t min, int16_t max) {
  if (min >= max) {
    setCfgErr();
    return !cfg_err_;
  }
  outmin_ = static_cast<int64_t>(min) * parameter_multipier_;
  outmax_ = static_cast<int64_t>(max) * parameter_multipier_;
  return !cfg_err_;
}

int16_t FastPID::step(int16_t sp, int16_t fb) {
  // int16 + int16 = int17
  int32_t err = static_cast<int32_t>(sp) - static_cast<int32_t>(fb);
  int32_t P = 0, I = 0;
  int32_t D = 0;

  if (p_ != 0) {
    P = static_cast<int32_t>(p_) * static_cast<int32_t>(err); // uint16 * int16 = int32
  }

  if (i_) {
    sum_ += static_cast<int64_t>(err) * static_cast<int64_t>(i_); // int17 * int16 = int33
    sum_ = std::clamp(sum_, integral_min_, integral_max_); // Saturate integration
    I = static_cast<int32_t>(sum_);
  }

  if (d_) {
    // (int17 - int16) - (int16 - int16) = int19
    int32_t deriv = (err - last_error_) - static_cast<int32_t>(sp - last_set_point_);
    last_set_point_ = sp;
    last_error_ = err;

    deriv = std::clamp(deriv, derivative_min_, derivative_max_); // Saturate derivative
    D = static_cast<int32_t>(d_) * static_cast<int32_t>(deriv); // int16 * int16 = int32
  }

  // int32 (P) + int32 (I) + int32 (D) = int34
  int64_t out = static_cast<int64_t>(P) + static_cast<int64_t>(I) + static_cast<int64_t>(D);

  out = std::clamp(out, outmin_, outmax_); // Make the output saturate

  // Remove the integer scaling factor.
  auto rval = static_cast<int16_t>(out >> parameter_shift_);

  // Fair rounding.
  if (out & (0x1ULL << (parameter_shift_ - 1))) {
    rval++;
  }

  return rval;
}
