#include "FastPID.hpp"
#include <algorithm>

void FastPID::clear() {
  _last_sp = 0;
  _last_out = 0;
  _sum = 0;
  _last_err = 0;
}

bool FastPID::setOutputRange(int16_t min, int16_t max) {
  if (min >= max) {
    setCfgErr();
    return !_cfg_err;
  }
  _outmin = static_cast<int64_t>(min) * parameter_multipier_;
  _outmax = static_cast<int64_t>(max) * parameter_multipier_;
  return !_cfg_err;
}

int16_t FastPID::step(int16_t sp, int16_t fb) {
  // int16 + int16 = int17
  int32_t err = static_cast<int32_t>(sp) - static_cast<int32_t>(fb);
  int32_t P = 0, I = 0;
  int32_t D = 0;

  if (_p) {
    // uint16 * int16 = int32
    P = static_cast<int32_t>(_p) * static_cast<int32_t>(err);
  }

  if (_i) {
    // int17 * int16 = int33
    _sum += static_cast<int64_t>(err) * static_cast<int64_t>(_i);

    // Limit sum to 32-bit signed value so that it saturates, never overflows.
    _sum = std::clamp(_sum, integral_min_, integral_max_);

    // int32
    I = static_cast<int32_t>(_sum);
  }

  if (_d) {
    // (int17 - int16) - (int16 - int16) = int19
    int32_t deriv = (err - _last_err) - static_cast<int32_t>(sp - _last_sp);
    _last_sp = sp;
    _last_err = err;

    // Limit the derivative to 16-bit signed value.
    deriv = std::clamp(deriv, derivative_min_, derivative_max_);

    // int16 * int16 = int32
    D = static_cast<int32_t>(_d) * static_cast<int32_t>(deriv);
  }

  // int32 (P) + int32 (I) + int32 (D) = int34
  int64_t out = static_cast<int64_t>(P) + static_cast<int64_t>(I) + static_cast<int64_t>(D);

  // Make the output saturate
  out = std::clamp(out, _outmin, _outmax);

  // Remove the integer scaling factor.
  auto rval = static_cast<int16_t>(out >> parameter_shift_);

  // Fair rounding.
  if (out & (0x1ULL << (parameter_shift_ - 1))) {
    rval++;
  }

  return rval;
}
