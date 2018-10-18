#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <variant>
#include <utility>
#include <QDebug>

#include "views.h"
#include "models.h"
#include "../schedulers/utils/io.h"
#include "../schedulers/utils/tasks.h"

namespace Presenters {
	class MemoryTaskPresenter {

	};

	class ProcessTaskPresenter {

	};

	class MainWindowPresenter {
	private:
		Views::MainWindowView* _view;

        std::vector<Models::TaskModel> _taskModels;

        void loadTasks(const std::string& path)
        {
            std::ifstream file(path);

            auto tasks = Utils::IO::loadTasks(file);

            std::vector<Models::TaskModel> newTaskModels;
            for (const auto& task : tasks)
            {
                if (auto* p = std::get_if<Utils::Tasks::MemoryTask>(&task))
                {
                    newTaskModels.push_back(Models::MemoryModel{p->state(), *p});
                }
            }
            _taskModels = std::move(newTaskModels);
        }

        void saveTasks(const std::string& path)
        {
            std::ofstream file(path);

            std::vector<Utils::Tasks::Task> tasks;

            for (const auto& model : _taskModels)
            {
                if (auto* p = std::get_if<Models::MemoryModel>(&model))
                {
                    tasks.push_back(p->task);
                }
            }

            Utils::IO::saveTasks(tasks, file);
        }

	public:
		MainWindowPresenter(Views::MainWindowView* view) :
			_view(view)
		{
            view->onOpenTaskListener([=](const std::string& path) {
                this->loadTasks(path);
            });

            view->onSaveTaskListener([=](const std::string& path) {
                this->saveTasks(path);
			});
		}
	};
}
