#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "../operations.h"
#include "../requests.h"
#include "../types.h"

namespace ProcessesManagement {

enum class StrategyType { ROUNDROBIN, FCFS };

/**
 * @brief Базовый класс для всех стратегий.
 */
class AbstractStrategy {
public:
  virtual ~AbstractStrategy(){};

  AbstractStrategy(StrategyType type) : type(type) {}

  const StrategyType type;

  /**
   *  Возвращает строковое обозначение стратегии.
   */
  virtual std::string toString() const = 0;

  /**
   *  @brief Обрабатывает заявку любого типа.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  ProcessesState processRequest(const Request &request,
                                const ProcessesState &state) const {
    return std::visit(
        [this, state](const auto &req) {
          return updateTimer(this->processRequest(req, state));
        },
        request);
  }

protected:
  /**
   *  @brief Обрабатывает заявку на создание нового процесса.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const CreateProcessReq &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку завершение существующего процесса.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TerminateProcessReq &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку на инициализацию ввода/вывода.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const InitIO &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку на завершение ввода/вывода.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TerminateIO &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку на передачу управления операционной системе.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TransferControl &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку "Истек квант вермени".
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TimeQuantumExpired &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Выбирает процесс, которому будет предоставлено процессорное время.
   *
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Кортеж из двух чисел: PID процесса и индекс очереди, в которой он
   *  находится; либо std::nullopt, если такового нет.
   */
  virtual std::optional<std::pair<int32_t, int32_t>>
  schedule(const ProcessesState &state) const = 0;

  /**
   * @brief Возвращает дескриптор процесса, который выполняется в данный момент.
   *
   * @param state Дескриптор состояния процессов.
   *
   * @return Дескриптор процесса или std::nullopt, если такового нет.
   */
  std::optional<Process> getCurrent(const ProcessesState &state) const {
    auto current =
        std::find_if(state.processes.begin(), state.processes.end(),
                     [](const auto &process) {
                       return process.state() == ProcState::EXECUTING;
                     });
    if (current == state.processes.end()) {
      return std::nullopt;
    } else {
      return {*current};
    };
  }

  /**
   * @brief Возвращает дескриптор процесса по его идентификатору.
   *
   * @param state Дескриптор состояния процессов.
   * @param pid Идентификатор процесса.
   *
   * @return Дескриптор процесса или std::nullopt, если такового нет.
   */
  std::optional<Process> getProcessByPid(const ProcessesState &state,
                                         int32_t pid) const {
    if (auto it = std::find_if(
            state.processes.begin(), state.processes.end(),
            [&pid](const auto &process) { return process.pid() == pid; });
        it != state.processes.end()) {
      return {*it};
    } else {
      return std::nullopt;
    }
  }
};

using StrategyPtr = std::shared_ptr<AbstractStrategy>;
} // namespace ProcessesManagement
