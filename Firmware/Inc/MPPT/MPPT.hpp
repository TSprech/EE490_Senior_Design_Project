//
// Created by treys on 2023/08/11
//

#ifndef MPPT_HPP
#define MPPT_HPP

#include "TypedUnits.hpp"
#include "pico/divider.h"

namespace mppt {
  namespace internal {
    /**
     * @tparam T The type of value that the DeltaVariable handles.
     * The `DeltaVariable` is a template class that maintains an record of a value and its previous value. The purpose of the class is to calculate the 'delta', i.e., the difference between the current value and the previous value.
     */
    template <typename T>
    class DeltaVariable {
     public:
      /**
       * @param initial_value The initial value to set for the `DeltaVariable`.
       * Constructor that takes an initial value and sets up the DeltaVariable.
       */
      constexpr explicit DeltaVariable(T initial_value) noexcept
          : value_(initial_value), previous_value_(initial_value) {}

      /**
       * @param value The value to set.
       * Overloaded assignment operator to set a new value. It also updates the previous value with the old one.
       * @return A reference to the current `DeltaVariable` object.
       */
      constexpr auto operator=(T value) noexcept -> DeltaVariable & {
        this->previous_value_ = value_;
        this->value_ = value;
        return *this;
      }

      /**
       * @return The current value.
       *
       * Function to get the current value. It is marked `always_inline`, `nodiscard`, and `maybe_unused`.
       */
      [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]] constexpr auto Value() const noexcept -> T {
        return this->value_;
      }

      /**
       * @return The previous value before the current one was set.
       *
       * Function to get the previous value. It is marked `always_inline`, `nodiscard`, and `maybe_unused`.
       */
      [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]] constexpr auto PreviousValue() const noexcept -> T {
        return this->previous_value_;
      }

      /**
       * @return The difference (or delta) between the current value and the previous one.
       *
       * Function to calculate the delta i.e., the difference between the current value and the previous value. It is marked `always_inline`, `nodiscard`, and `maybe_unused`.
       */
      [[gnu::always_inline]] [[nodiscard]] [[maybe_unused]] constexpr auto Delta() const noexcept -> T {
        return this->value_ - this->previous_value_;
      }

     private:
      T value_; // The current value
      T previous_value_; // The previous value
    };
  }

  /**
 * @brief Incremental conductance MPPT algorithm.
 * @cite https://ww1.microchip.com/downloads/en/AppNotes/00001521A.pdf
 * @param v_panel The current panel voltage.
 * @param i_panel The current panel current.
 * @returns An offset to be applied to the current duty cycle.
 */
  inline auto IncrementalConductance(units::voltage::millivolt_u32_t v_panel, units::current::milliampere_u32_t i_panel) -> int32_t {
    constexpr int32_t duty_step_size = 1;                                               // How much to change the duty cycle per function call, range 0 - 1,000,000
    static internal::DeltaVariable<units::voltage::millivolt_i32_t> panel_voltage{v_panel};        // Keeps track of the change in panel voltage per function call
    static internal::DeltaVariable<units::current::milliampere_i32_t> panel_current{i_panel};      // Keeps track of the change in panel current per function call
    static internal::DeltaVariable<units::power::milliwatt_i32_t> panel_power{v_panel * i_panel};  // Keeps track of the change in panel power per function call

    panel_voltage = v_panel;  // Update the panel values
    panel_current = i_panel;
    panel_power = v_panel * i_panel;

    // Panel current ↑↑↑ panel voltage ↓
    if (panel_voltage.Delta().value() != 0) {
      auto ratio = panel_power.Delta().to<float>() / panel_voltage.Delta().to<float>();
      if (ratio > 0)
        return -duty_step_size;
      else if (ratio < 0)
        return duty_step_size;
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
