#ifndef MODULOSCHEDULER_HPP
#define MODULOSCHEDULER_HPP
// 5/4/2023
#include <cstdint>
#include "../../Src/TypedUnits.hpp"
#include <functional>
#include <concepts>

template <uint32_t number_of_tasks>
class ModuloScheduler;

struct ModuloTask {
//  auto Execute(uint32_t current_mod_count) -> void {
//    if (current_mod_count % modulo_number_ == 0) callback_();
//  }

  constexpr ModuloTask(units::time::millisecond_u32_t interval, void (*callback)())
    : interval(interval), callback(callback) {}

//  constexpr auto operator<(ModuloTask task) const -> bool {
//    return this->interval < task.interval;
//  }

  units::time::millisecond_u32_t interval;
  void (*callback)();
};

template <typename T>
concept IsModuloTask = std::same_as<T, ModuloTask>;

struct TaskPair {
  constexpr TaskPair(ModuloTask task) : task(task) {} // Explicitly not marked explicit to allow simple array creation
  constexpr TaskPair(ModuloTask task, uint_fast32_t mod) : task(task), mod_number(mod) {}
  constexpr auto operator<(TaskPair task_pair) const -> bool {
    return this->task.interval < task_pair.task.interval;
  }

  ModuloTask task;
  uint_fast32_t mod_number = 0;
};

template <IsModuloTask... Tasks>
constexpr auto CreateScheduler(Tasks... tasks) {
  constexpr auto number_of_tasks = sizeof...(Tasks);
  std::array<TaskPair, number_of_tasks> task_list = {tasks...};
  std::sort(task_list.begin(), task_list.end());
  auto min_interval = task_list.begin()->task.interval;
  for (auto& task_pair : task_list) {
    task_pair.mod_number = task_pair.task.interval.value() / min_interval.value();
  }
  return ModuloScheduler<number_of_tasks>(task_list);
}

/**
 * @brief A fixed point, integer based PID algorithm.
 */
template <uint32_t number_of_tasks>
class ModuloScheduler {
 public:
  constexpr explicit ModuloScheduler(std::array<TaskPair, number_of_tasks> tasks) :
    tasks_(tasks) {}

  auto Execute() -> void {
    for (auto& task : tasks_) {
      if (cycle_number_ % task.mod_number == 0) task.task.callback();
    }
    ++cycle_number_;
  }

  std::array<TaskPair, number_of_tasks> tasks_;
  uint32_t cycle_number_ = 0;
};

#endif  // MODULOSCHEDULER_HPP
