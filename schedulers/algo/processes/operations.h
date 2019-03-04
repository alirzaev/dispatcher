#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <map>
#include <set>

#include "exceptions.h"
#include "types.h"

namespace ProcessesManagement {
/*
 * Операция изменения состояния процесса
 *
 * @param state состояние процессов
 * @param pid идентификатор процесса
 * @param newState новое состояние процесса
 *
 * @return новое состояние процессов
 */
inline ProcessesState changeProcessState(const ProcessesState &state,
                                         int32_t pid, ProcState newState) {
  auto [processes, queues] = state;

  auto it =
      std::find_if(processes.begin(), processes.end(),
                   [pid](const auto &process) { return process.pid() == pid; });
  if (it == processes.end()) {
    throw OperationException("NO_SUCH_PROCESS");
  } else {
    *it = it->state(newState);
    return {processes, queues};
  }
}

/*
 * Операция добавления процесса в очередь
 *
 * @param state состояние процессов
 * @param queueIndex индекс очереди (0..15)
 * @param pid идентификатор процесса
 *
 * @return новое состояние процессов
 */
inline ProcessesState pushToQueue(const ProcessesState &state,
                                  int32_t queueIndex, int32_t pid) {
  auto [processes, queues] = state;

  auto it =
      std::find_if(processes.begin(), processes.end(),
                   [pid](const auto &process) { return process.pid() == pid; });

  if (it == processes.end()) {
    throw OperationException("NO_SUCH_PROCESS");
  } else {
    for (const auto &queue : queues) {
      if (std::find(queue.begin(), queue.end(), pid) != queue.end()) {
        throw OperationException("ALREADY_IN_QUEUE");
      }
    }
    queues.at(static_cast<size_t>(queueIndex)).push_back(pid);
    *it = it->priority(queueIndex);
    return {processes, queues};
  }
}

/*
 * Операция извлечения процесса из очереди
 *
 * @param state состояние процессов
 * @param queueIndex индекс очереди (0..15)
 *
 * @return новое состояние процессов
 */
inline ProcessesState popFromQueue(const ProcessesState &state,
                                   int32_t queueIndex) {
  auto [processes, queues] = state;

  auto &queue = queues.at(static_cast<size_t>(queueIndex));
  if (queue.empty()) {
    throw OperationException("EMPTY_QUEUE");
  }
  auto pid = queue.front();
  auto it =
      std::find_if(processes.begin(), processes.end(),
                   [pid](const auto &process) { return process.pid() == pid; });
  if (it == processes.end()) {
    throw OperationException("NO_SUCH_PROCESS");
  }
  queue.pop_front();
  return {processes, queues};
}

/*
 * Операция переключения системы на другой процесс
 *
 * @param state состояние процессов
 * @param nextPid идентификатор процесса
 *
 * @return новое состояние процессов
 */
inline ProcessesState switchTo(const ProcessesState &state, int32_t nextPid) {
  auto [processes, queues] = state;

  auto prev =
      std::find_if(processes.begin(), processes.end(), [](const auto &process) {
        return process.state() == ProcState::EXECUTING;
      });
  auto next = std::find_if(
      processes.begin(), processes.end(),
      [nextPid](const auto &process) { return process.pid() == nextPid; });

  if (next == processes.end()) {
    throw OperationException("NO_SUCH_PROCESS");
  }
  if (prev != processes.end() && *prev == *next) {
    return state;
  }
  if (next->state() != ProcState::ACTIVE) {
    throw OperationException("INVALID_STATE");
  }
  if (prev != processes.end()) {
    *prev = prev->state(ProcState::ACTIVE);
  }
  *next = next->state(ProcState::EXECUTING);

  return {processes, queues};
}

/*
 * Операция завершения процесса (в т.ч. и дочерних)
 *
 * @param state состояние процессов
 * @param pid идентификатор процесса
 *
 * @return новое состояние процессов
 */
inline ProcessesState terminateProcess(const ProcessesState &state,
                                       int32_t pid) {
  auto [processes, queues] = state;

  auto it =
      std::find_if(processes.begin(), processes.end(),
                   [pid](const auto &process) { return process.pid() == pid; });

  if (it == processes.end()) {
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

/*
 * Операция добавления нового процесса в список процессов
 *
 * @param state состояние процессов
 * @param process дескриптор процесса
 *
 * @return новое состояние процессов
 */
inline ProcessesState addProcess(const ProcessesState &state, Process process) {
  auto [processes, queues] = state;

  auto it = std::find_if(processes.begin(), processes.end(),
                         [&process](const auto &current) {
                           return current.pid() == process.pid();
                         });

  if (it != processes.end()) {
    throw OperationException("PROCESS_EXISTS");
  }

  auto parent = std::find_if(processes.begin(), processes.end(),
                             [&process](const auto &current) {
                               return current.pid() == process.ppid();
                             });
  if (process.ppid() != -1 && parent == processes.end()) {
    throw OperationException("NO_SUCH_PPID");
  }

  processes.push_back(process);
  std::sort(processes.begin(), processes.end());

  return {processes, queues};
}

/*
 * Операция обновления таймера
 *
 * @param state состояние процессов
 *
 * @return новое состояние процессов
 */
inline ProcessesState updateTimer(const ProcessesState &state) {
  auto [processes, queues] = state;

  auto it =
      std::find_if(processes.begin(), processes.end(), [](const auto &current) {
        return current.state() == ProcState::EXECUTING;
      });

  if (it != processes.end()) {
    *it = it->timer(it->timer() + 1);
  }

  return {processes, queues};
}
} // namespace ProcessesManagement
