#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include "exceptions.h"
#include "helpers.h"
#include "types.h"

namespace ProcessesManagement {
/**
 *  @brief Операция изменения состояния процесса.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param pid Идентификатор процесса.
 *  @param newState Новое состояние процесса.
 *
 *  @return Новое состояние процессов.
 *
 *  @throws ProcessesManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "NO_SUCH_PROCESS" - процесса с таким @a pid не существует.
 */
inline ProcessesState changeProcessState(const ProcessesState &state,
                                         int32_t pid, ProcState newState) {
  auto [processes, queues] = state;

  if (auto index = getIndexByPid(processes, pid); index.has_value()) {
    processes.at(*index) = processes.at(*index).state(newState);
    return {processes, queues};
  } else {
    throw OperationException("NO_SUCH_PROCESS");
  }
}

/**
 *  @brief Операция добавления процесса в очередь.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param queueIndex Индекс очереди.
 *  @param pid Идентификатор процесса.
 *
 *  @return Новое состояние процессов.
 *
 *  @throws ProcessesManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "NO_SUCH_PROCESS" - процесса с таким @a pid не существует;
 *  "ALREADY_IN_QUEUE" - процесс уже добавлен в одну из очередей.
 */
inline ProcessesState pushToQueue(const ProcessesState &state,
                                  size_t queueIndex, int32_t pid) {
  auto [processes, queues] = state;

  if (auto index = getIndexByPid(processes, pid); index.has_value()) {
    for (const auto &queue : queues) {
      if (std::find(queue.begin(), queue.end(), pid) != queue.end()) {
        throw OperationException("ALREADY_IN_QUEUE");
      }
    }
    queues.at(queueIndex).push_back(pid);
    processes.at(*index) = processes.at(*index).priority(queueIndex);
    return {processes, queues};
  } else {
    throw OperationException("NO_SUCH_PROCESS");
  }
}

/**
 *  @brief Операция извлечения процесса из очереди.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param queueIndex Индекс очереди.
 *
 *  @return Новое состояние процессов.
 *
 *  @throws ProcessesManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "EMPTY_QUEUE" - очередь с индексом @a queueIndex пуста;
 *  "NO_SUCH_PROCESS" - процесса с @a pid нет в очереди с индексом @a
 *  queueIndex.
 */
inline ProcessesState popFromQueue(const ProcessesState &state,
                                   size_t queueIndex) {
  auto [processes, queues] = state;

  auto &queue = queues.at(queueIndex);
  if (queue.empty()) {
    throw OperationException("EMPTY_QUEUE");
  }

  auto pid = queue.front();

  if (auto index = getIndexByPid(processes, pid); !index.has_value()) {
    throw OperationException("NO_SUCH_PROCESS");
  }
  queue.pop_front();
  return {processes, queues};
}

/**
 *  @brief Операция переключения системы на другой процесс.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param nextPid Идентификатор процесса, на который нужно переключиться.
 *
 *  @return Новое состояние процессов.
 *
 *  @throws ProcessesManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "NO_SUCH_PROCESS" - процесса с таким @a newPid не существует;
 *  "INVALID_STATE" - процесс с @a newPid не находится в состоянии
 *  ProcState::ACTIVE.
 */
inline ProcessesState switchTo(const ProcessesState &state, int32_t nextPid) {
  auto [processes, queues] = state;

  auto prevIndex = getIndexByState(processes, ProcState::EXECUTING);
  auto nextIndex = getIndexByPid(processes, nextPid);

  if (!nextIndex.has_value()) {
    throw OperationException("NO_SUCH_PROCESS");
  }

  auto next = processes.at(*nextIndex);
  if (prevIndex.has_value()) {
    auto prev = processes.at(*prevIndex);
    if (prev == next) {
      return state;
    } else {
      processes.at(*prevIndex) = prev.state(ProcState::ACTIVE);
    }

    if (next.state() != ProcState::ACTIVE) {
      throw OperationException("INVALID_STATE");
    }
  }
  processes.at(*nextIndex) = next.state(ProcState::EXECUTING);

  return {processes, queues};
}

/**
 *  @brief Операция завершения процесса (в т.ч. и дочерних).
 *
 *  @param state Дескриптор состояния процессов.
 *  @param pid Идентификатор процесса.
 *
 *  @return Новое состояние процессов.
 *
 *  @throws ProcessesManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "NO_SUCH_PROCESS" - процесса с таким @a pid не существует.
 */
inline ProcessesState terminateProcess(const ProcessesState &state,
                                       int32_t pid) {
  auto [processes, queues] = state;

  if (auto index = getIndexByPid(processes, pid); !index.has_value()) {
    throw OperationException("NO_SUCH_PROCESS");
  }

  std::map<int32_t, std::vector<int32_t>> parents;
  for (const auto &process : processes) {
    if (process.ppid() != -1) {
      parents[process.ppid()].push_back(process.pid());
    }
  }
  std::set<int32_t> toTerminate;
  std::function<void(int32_t)> rec = [&rec, &toTerminate,
                                      &parents](int32_t pid) {
    toTerminate.insert(pid);
    for (const auto &child : parents[pid]) {
      rec(child);
    }
  };
  rec(pid);

  decltype(processes) newProcesses;
  decltype(queues) newQueues;

  for (const auto &process : processes) {
    if (toTerminate.find(process.pid()) == toTerminate.end()) {
      newProcesses.push_back(process);
    }
  }
  for (size_t i = 0; i < queues.size(); ++i) {
    const auto &queue = queues[i];
    auto &newQueue = newQueues[i];
    for (const auto &pid : queue) {
      if (toTerminate.find(pid) == toTerminate.end()) {
        newQueue.push_back(pid);
      }
    }
  }
  return {newProcesses, newQueues};
}

/**
 *  @brief Операция добавления нового процесса в список процессов.
 *
 *  @param state Дескриптор состояния процессов.
 *  @param process Дескриптор процесса.
 *
 *  @return Новое состояние процессов.
 *
 *  @throws ProcessesManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "PROCESS_EXISTS" - процесс с таким идентификатором уже есть;
 *  "NO_SUCH_PPID" - родительского процесса не существует.
 */
inline ProcessesState addProcess(const ProcessesState &state, Process process) {
  auto [processes, queues] = state;

  if (auto index = getIndexByPid(processes, process.pid()); index.has_value()) {
    throw OperationException("PROCESS_EXISTS");
  }

  auto parentIndex = getIndexByPid(processes, process.ppid());
  if (process.ppid() != -1 && !parentIndex.has_value()) {
    throw OperationException("NO_SUCH_PPID");
  }

  processes.push_back(process);
  std::sort(processes.begin(), processes.end());

  return {processes, queues};
}

/**
 *  @brief Операция обновления таймера.
 *
 *  @param state Дескриптор состояния процессов.
 *
 *  @return Новое состояние процессов.
 */
inline ProcessesState updateTimer(const ProcessesState &state) {
  auto [processes, queues] = state;

  if (auto index = getIndexByState(processes, ProcState::EXECUTING);
      index.has_value()) {
    auto current = processes.at(*index);
    processes.at(*index) = current.timer(current.timer() + 1);
  }

  return {processes, queues};
}
} // namespace ProcessesManagement
