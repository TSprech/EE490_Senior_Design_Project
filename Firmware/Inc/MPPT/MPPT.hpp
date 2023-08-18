//
// Created by treys on 2023/08/11
//

#ifndef MPPT_HPP
#define MPPT_HPP

#include "TypedUnits.hpp"

using namespace units::literals;

namespace mppt {
  namespace internal {
    template <typename T>
    class DeltaVariable {
     public:
      constexpr explicit DeltaVariable(T initial_value) noexcept
          : value_(initial_value), previous_value_(initial_value) {}

      constexpr auto operator=(T value) noexcept -> DeltaVariable & {
        this->previous_value_ = value_;
        this->value_ = value;
        return *this;
      }

      [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]] constexpr auto Value() const noexcept -> T {
        return this->value_;
      }

      [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]] constexpr auto PreviousValue() const noexcept -> T {
        return this->previous_value_;
      }

      [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]] constexpr auto Delta() const noexcept -> T {
        return this->value_ - this->previous_value_;
      }

     private:
      T value_;
      T previous_value_;
    };
  }

  /**
 * @brief Incremental conductance MPPT algorithm.
 * @cite https://ww1.microchip.com/downloads/en/AppNotes/00001521A.pdf
 * @param v_panel The current panel voltage.
 * @param i_panel The current panel current.
 * @returns An offset to be applied to the current duty cycle.
 */
  auto IncrementalConductance(units::voltage::millivolt_u32_t v_panel, units::current::milliampere_u32_t i_panel) -> int32_t {
    constexpr int32_t duty_step_size = 1;                                               // How much to change the duty cycle per function call, range 0 - 1,000,000
    static internal::DeltaVariable<units::voltage::millivolt_u32_t> panel_voltage{v_panel};        // Keeps track of the change in panel voltage per function call
    static internal::DeltaVariable<units::current::milliampere_u32_t> panel_current{i_panel};      // Keeps track of the change in panel current per function call
    static internal::DeltaVariable<units::power::milliwatt_u32_t> panel_power{v_panel * i_panel};  // Keeps track of the change in panel power per function call

    panel_voltage = v_panel;  // Update the panel values
    panel_current = i_panel;
    panel_power = v_panel * i_panel;

    if (panel_voltage.Delta().value() != 0) {
      auto ratio = panel_power.Delta() / panel_voltage.Delta();
      if (ratio.value() > 0)
        return duty_step_size;
      else if (ratio.value() < 0)
        return -duty_step_size;
    } else {
      if (panel_current.Delta().value() > 0)
        return duty_step_size;
      else if (panel_current.Delta().value() < 0)
        return -duty_step_size;
    }
    return 0;  // Case when no change is required
  }

}

#endif  // MPPT_HPP
