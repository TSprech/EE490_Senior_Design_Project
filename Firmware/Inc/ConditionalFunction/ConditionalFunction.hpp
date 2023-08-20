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
    constexpr explicit FunctionManager(Callable f) noexcept
        : function_(std::move(f)) {}

    void Enable() {
      is_enabled_ = true;
    }

    void Disable() {
      is_enabled_ = false;
    }

    //    // Operator() now takes arbitrary parameters, passed to the Callable
    //    template <typename... Args>
    //    auto operator()(Args&&... args) {
    //      if constexpr (IsVoidCallable<Callable>) {
    //        // If return type is void
    //        if (is_enabled_) {
    //          function_(std::forward<Args>(args)...);
    //        }
    //      } else {
    //        // If return type is non-void, returning an std::optional
    //        if (is_enabled_) {
    //          return std::make_optional(function_(std::forward<Args>(args)...));
    //        } else {
    //          // Creates an optional with value, not initialized
    //          return std::optional<std::invoke_result_t<Callable, Args...>>{};
    //        }
    //      }
    //    }

    // Operator() now takes arbitrary parameters, passed to the Callable
    template <typename... Args> requires std::is_void_v<std::invoke_result_t<Callable>>
    auto operator()(Args&&... args) {
        function_(std::forward<Args>(args)...);
      // If return type is non-void, returning an std::optional
//      if (is_enabled_) {
//        return std::make_optional(function_(std::forward<Args>(args)...));
//      } else {
//        // Creates an optional with value, not initialized
//        return std::optional<std::invoke_result_t<Callable, Args...>>{};
//      }
    }

//    template <typename... Args> requires (!std::is_void_v<std::invoke_result_t<Callable>>)
//auto operator()(Args&&... args) -> std::optional<std::invoke_result_t<Callable>> {
//        if (is_enabled_) {
//          return function_(std::forward<Args>(args)...);
//        } else{
//          return std::nullopt;
//        }
//    }
  };
}  // namespace cfunc

#endif  // CONDITIONALFUNCTION_HPP