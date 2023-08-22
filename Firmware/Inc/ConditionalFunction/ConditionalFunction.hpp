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
  template <typename Callable, typename... Args>
  concept IsVoidFunction = std::is_void_v<std::invoke_result_t<Callable, Args...>>;

  template <typename Callable>
  class FunctionManager {
   public:
    // Using explicit keyword to avoid unintentional implicit conversion
    // Ensuring the callable target is perfectly forwarded with std::forward
    constexpr explicit FunctionManager(Callable f) noexcept
        : function_(std::move(f)) {}

    void Enable() noexcept {
      is_enabled_ = true;
    }

    void Disable() noexcept {
      is_enabled_ = false;
    }

    template <typename... Args> requires IsVoidFunction<Callable, Args...>
    auto operator()(Args &&...args) const -> void {
      if (is_enabled_) {
        function_(std::forward<Args>(args)...);
      }
    }

    template <typename... Args> requires (!IsVoidFunction<Callable, Args...>)
    auto operator()(Args &&...args) const -> std::optional<std::invoke_result_t<Callable, Args...>> {
      if (is_enabled_) {
        return function_(std::forward<Args>(args)...);
      } else {
        return std::nullopt;
      }
    }

   private:
    bool is_enabled_ = false;
    Callable function_;
  };
}  // namespace cfunc

#endif  // CONDITIONALFUNCTION_HPP