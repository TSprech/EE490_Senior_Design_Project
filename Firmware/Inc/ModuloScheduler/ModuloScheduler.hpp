/**
 * @copyright 2023
 * @author TSprech
 * @date 2023/05/04
 * @brief Yet another task scheduler implementation, this time using modulo division.
 * @license Apache 2.0
 */

#ifndef MODULOSCHEDULER_HPP
#define MODULOSCHEDULER_HPP
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <span>

#include "TypedUnits.hpp"

namespace modsch {
  /**
   * @breif Represents a task callback function and how often it should be called, in terms of milliseconds.
   */
  struct ModuloTask {
    units::time::millisecond_u32_t period; // The amount of time that should pass between subsequent @ref callback calls
    void (*callback)();  // The function to be called every @ref period | Have to use C style function pointers instead of std::functional because std::functional is not constexpr
  };

  /**
   * @brief Represents a task and its modulo number which is how long it must wait relative to a base task before executing.
   */
  struct TaskPair {
    /**
     * @brief Constructor for a TaskPair which represents a task and its associated modulo number.
     * @param task The task which will be called when the scheduler runs.
     * @note This is explicitly not marked explicit to make it easier to use it in the variadic template initialization used later.
     */
    constexpr TaskPair(ModuloTask task)
        : task(task) {}

    /**
     * @brief Overload to allow an array of @ref TaskPair to be sorted easily.
     * @param task_pair The @ref TaskPair to compare against.
     * @returns The comparison result, for standard std::sort, it will sort smallest task period to largest.
     */
    constexpr auto operator<(TaskPair task_pair) const -> bool {
      return this->task.period < task_pair.task.period;
    }

    ModuloTask task;                 /**< The task being run once the scheduler conditions are correct. */
    uint_fast32_t modulo_number = 0; /**< This represents how many times the base (shortest period) task must run before this task runs. */
  };

  /**
   * @brief The scheduler that takes in a list of tasks and manages the execution of them.
   */
  template <uint32_t Number_Of_Tasks>
  class ModuloScheduler {
   public:
    /**
     * @brief Constructs a scheduler given an array of tasks.
     * @param tasks All the tasks which will be added to the scheduler.
     * @warning This should not be called directly and instead called through @ref CreateScheduler.
     */
    constexpr explicit ModuloScheduler(std::array<TaskPair, Number_Of_Tasks> tasks)
        : tasks_(tasks) {}

    /**
     * @brief Runs whatever tasks are ready to be run. Should be called with a period as specified in @ref Period.
     */
    auto Execute() -> void {
      static uint_fast32_t cycle_number_ = 0; // Represents how many times the first task has been called, which through modulo division, determines which other tasks should be called
      for (auto& task : tasks_) {
        if (cycle_number_ % task.modulo_number == 0) task.task.callback(); // This will always be true for the first task
      }
      ++cycle_number_;
    }

    [[nodiscard]] constexpr auto Period() const -> units::time::millisecond_u32_t {
      return tasks_.begin()->task.period;
    }

    /**
     * @returns The immutable list of tasks.
     */
    [[nodiscard]] constexpr auto Tasks() const -> std::span<const TaskPair> {
      return tasks_;
    }

   private:
    std::array<TaskPair, Number_Of_Tasks> tasks_; /**< The list of all tasks for this scheduler. */
  };

  template <typename T>  // Concept to type check the variadic parameters of the @ref CreateScheduler function to make sure they are all @ref ModuloTask type
  concept IsModuloTask = std::same_as<T, ModuloTask>;

  /**
   * @brief Creates a scheduler which is based on modulo division at compile time.
   * @tparam Tasks Variadic parameter which represents any number of tasks which can be used to create the scheduler.
   * @param tasks The tasks which will be called by the scheduler at the period specified in the task.
   * @returns A @ref ModuloScheduler object which contains all the logic to run the tasks.
   */
  template <IsModuloTask... Tasks>
  constexpr auto CreateScheduler(Tasks... tasks) -> ModuloScheduler<sizeof...(Tasks)> {
    constexpr auto number_of_tasks = sizeof...(Tasks);                               // Get the number of tasks which will be in this scheduler (fixed number)
    std::array<TaskPair, number_of_tasks> task_list = {tasks...};                    // Create an array of all the tasks passed in
    std::sort(task_list.begin(), task_list.end());                                   // Sort the list so that the task with the shortest period is first with callback time in increasing order after that
    auto min_period = task_list.begin()->task.period;                                // Get the period of the shortest task callback time (which is first after the above sort)
    for (auto& task_pair : task_list) {
      task_pair.modulo_number = task_pair.task.period.value() / min_period.value();  // The modulo number is found by taking a task's period and dividing it by the shortest period, which establishes how many times the shortest period task must run before the current task is called
    }
    return ModuloScheduler<number_of_tasks>(task_list);                              // Construct a scheduler based on the task list
  }
}  // namespace modsch

#endif  // MODULOSCHEDULER_HPP
