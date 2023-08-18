//
// Created by treys on 2023/08/11
//

#ifndef CONDITIONALFUNCTION_HPP
#define CONDITIONALFUNCTION_HPP

#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>

namespace cfunc {

  // Predicate that checks if a given callable type is void
  template <typename Callable>
  concept IsVoidCallable = std::is_void_v<std::invoke_result_t<Callable>>;

  template <typename Callable>
  class FunctionManager {
   private:
    bool is_enabled_ = false;
    Callable function_;

   public:

    // Using explicit keyword to avoid unintentional implicit conversion
    // Ensuring the callable target is perfectly forwarded with std::forward
    constexpr explicit FunctionManager(Callable f) : function_(std::move(f)) {}

    void Enable() {
      is_enabled_ = true;
    }

    void Disable() {
      is_enabled_ = false;
    }

    // Result type for non-void callables
//    using ResultType = std::conditional_t<IsVoidCallable<Callable>, void, std::optional<std::invoke_result_t<Callable>>>;

//    ResultType operator()() {
    auto operator()() {
      if constexpr (IsVoidCallable<Callable>) {
        // If return type is void
        if (is_enabled_) {
          function_();
        }
      } else {
        // If return type is non-void, returning an std::optional
        return is_enabled_ ? std::make_optional(function_()) : std::optional<decltype(function_())>{};
      }
    }
  };
}  // namespace cfunction_

#endif  // CONDITIONALFUNCTION_HPP