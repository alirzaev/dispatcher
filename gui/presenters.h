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
    private:
        Views::MemoryTaskView* _view;

        Models::MemoryModel* _model;

    public:
        MemoryTaskPresenter(Views::MemoryTaskView* view, Models::MemoryModel* model) :
            _view(view), _model(model)
        {
            refreshView();
        }

        void refreshView()
        {
            auto [blocks, freeBlocks] = _model->state;
            _view->setMemoryBlocks(blocks);
            _view->setFreeMemoryBlocks(freeBlocks);
            auto index = _model->task.completed();
            _view->setRequest(_model->task.requests()[index]);
        }
	};

	class ProcessTaskPresenter {
    private:
        Views::ProcessTaskView* _view;

        Models::ProcessesModel* _model;

    public:
        ProcessTaskPresenter(Views::ProcessTaskView* view, Models::ProcessesModel* model) :
            _view(view), _model(model)
        {}
	};

    using TaskPresenter = std::variant<MemoryTaskPresenter, ProcessTaskPresenter>;

	class MainWindowPresenter {
	private:
		Views::MainWindowView* _view;

        std::vector<Models::TaskModel> _taskModels;

        std::vector<TaskPresenter> _taskPresenters;

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

            auto views = _view->createTaskViews(tasks);
            std::vector<TaskPresenter> newTaskPresenters;
            for (size_t i = 0; i < views.size(); ++i)
            {
                if (auto* view = std::get_if<Views::MemoryTaskView*>(&views[i]))
                {
                    auto* model = std::get_if<Models::MemoryModel>(&newTaskModels[i]);
                    newTaskPresenters.push_back(MemoryTaskPresenter(*view, model));
                }
            }
            _taskModels.swap(newTaskModels);
            _taskPresenters.swap(newTaskPresenters);
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
            _view->onOpenTaskListener([=](const std::string& path) {
                this->loadTasks(path);
            });

            _view->onSaveTaskListener([=](const std::string& path) {
                this->saveTasks(path);
			});
		}
	};
}
