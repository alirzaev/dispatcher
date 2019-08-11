#pragma once

#include <utils/tasks.h>

class TaskGetter {
public:
  virtual Utils::Task task() const = 0;

  virtual ~TaskGetter() = default;
};
