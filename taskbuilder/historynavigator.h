#pragma once

#include <cstddef>
#include <vector>

#include <utils/tasks.h>

class HistoryNavigator {
public:
  HistoryNavigator() = delete;

  HistoryNavigator(Utils::Task initial) : _history({initial}), _pos(0) {}

  HistoryNavigator &operator=(const HistoryNavigator &rhs) = default;

  void forward() {
    if (!hasNext()) {
      return;
    }

    _pos++;
    loadTaskFromHistory(_history[_pos]);
  }

  void backward() {
    if (!hasPrevious()) {
      return;
    }

    _pos--;
    loadTaskFromHistory(_history[_pos]);
  }

  bool hasNext() const { return _pos < _history.size() - 1; }

  bool hasPrevious() const { return _pos > 0; }

  virtual ~HistoryNavigator() {}

protected:
  virtual void loadTaskFromHistory(const Utils::Task &task) = 0;

  void push(Utils::Task task) {
    _history.resize(_pos + 2, _history[0]);
    _pos++;
    _history[_pos] = task;
  }

private:
  std::vector<Utils::Task> _history;

  size_t _pos;
};
