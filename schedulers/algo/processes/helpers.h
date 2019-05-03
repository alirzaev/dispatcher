#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "types.h"

namespace ProcessesManagement {
/**
 *  @brief Находит дескриптор процесса с заданным идентификатором процесса.
 *
 *  @param processes Список дескрипторов процессов.
 *  @param pid Идентификатор процесса.
 *
 *  @return Итератор, указывающий на искомый дескриптор или @a processes.end(),
 *  если процесса с таким @a pid не существует.
 */
inline std::vector<Process>::iterator
getByPid(const std::vector<Process> &processes, int32_t pid) {
  auto &ref = const_cast<std::vector<Process> &>(processes);

  return std::find_if(ref.begin(), ref.end(), [pid](const auto &process) {
    return process.pid() == pid;
  });
}

/**
 *  @brief Находит дескриптор первого по списку процесса с заданным состоянием
 *  процесса.
 *
 *  @param processes Список дескрипторов процессов.
 *  @param state Состояние процесса.
 *
 *  @return Итератор, указывающий на искомый дескриптор или @a processes.end(),
 *  если процесса с таким @a state не найдено.
 */
inline std::vector<Process>::iterator
getByState(const std::vector<Process> &processes, ProcState state) {
  auto &ref = const_cast<std::vector<Process> &>(processes);

  return std::find_if(ref.begin(), ref.end(), [state](const auto &process) {
    return process.state() == state;
  });
}
} // namespace ProcessesManagement
