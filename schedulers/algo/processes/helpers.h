#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <tl/optional.hpp>

#include "types.h"

namespace ProcessesManagement {
/**
 *  @brief Находит индекс дескриптора процесса с заданным идентификатором
 * процесса.
 *
 *  @param processes Список дескрипторов процессов.
 *  @param pid Идентификатор процесса.
 *
 *  @return Индекс (tl::optional) искомого дескриптора или tl::nullopt,
 *  если процесса с таким @a pid не существует.
 */
inline tl::optional<std::size_t>
getIndexByPid(const std::vector<Process> &processes, int32_t pid) {

  auto pos =
      std::find_if(processes.begin(),
                   processes.end(),
                   [pid](const auto &process) { return process.pid() == pid; });

  if (pos == processes.end()) {
    return tl::nullopt;
  }

  return static_cast<std::size_t>(pos - processes.begin());
}

/**
 *  @brief Находит индекс дескриптора процесса с заданным идентификатором
 * процесса.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param pid Идентификатор процесса.
 *
 *  @return Индекс (tl::optional) искомого дескриптора или tl::nullopt,
 *  если процесса с таким @a pid не существует.
 */
inline tl::optional<std::size_t> getIndexByPid(const ProcessesState &state,
                                               int32_t pid) {
  return getIndexByPid(state.processes, pid);
}

/**
 *  @brief Находит индекс первого по списку дескриптора процесса с заданным
 * состоянием процесса.
 *
 *  @param processes Список дескрипторов процессов.
 *  @param state Состояние процесса.
 *
 *  @return Индекс (tl::optional) искомого дескриптора или tl::nullopt,
 *  если процесса с таким @a pid не существует.
 */
inline tl::optional<std::size_t>
getIndexByState(const std::vector<Process> &processes, ProcState state) {
  auto pos = std::find_if(
      processes.begin(), processes.end(), [state](const auto &process) {
        return process.state() == state;
      });

  if (pos == processes.end()) {
    return tl::nullopt;
  }

  return static_cast<std::size_t>(pos - processes.begin());
}

/**
 *  @brief Находит индекс первого по списку дескриптора процесса с заданным
 * состоянием процесса.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param procState Состояние процесса.
 *
 *  @return Индекс (tl::optional) искомого дескриптора или tl::nullopt,
 *  если процесса с таким @a pid не существует.
 */
inline tl::optional<std::size_t> getIndexByState(const ProcessesState &state,
                                                 ProcState procState) {
  return getIndexByState(state.processes, procState);
}
} // namespace ProcessesManagement
