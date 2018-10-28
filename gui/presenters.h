#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <variant>
#include <utility>
#include <memory>
#include <tuple>
#include <exception>
#include <stdexcept>
#include <QDebug>

#include "views.h"
#include "models.h"
#include "../schedulers/utils/io.h"
#include "../schedulers/utils/tasks.h"
#include "../schedulers/algo/memory/operations.h"
#include "../schedulers/algo/memory/types.h"

namespace Presenters
{
class AbstractTaskPresenter
{ };

class MemoryTaskPresenter : public AbstractTaskPresenter
{
private:
    Views::MemoryTaskView* _view;

    Models::MemoryModel* _model;

public:
    MemoryTaskPresenter(Views::MemoryTaskView* view, Models::MemoryModel* model) :
        AbstractTaskPresenter (),
        _view(view), _model(model)
    {
        _view->onAllocateAction([=](const auto& data, const auto& state) {
            this->allocateMemory(data, state);
        });

        _view->onFreeAction([=](const auto& data, const auto& state) {
            this->freeMemory(data, state);
        });

        _view->onDefragmentAction([=](const auto& state) {
            this->defragmentMemory(state);
        });

        _view->onCompressAction([=](const auto& data, const auto& state) {
            this->compressMemory(data, state);
        });

        _view->onNextRequestListener([=](const auto& state) {
            this->nextRequest(state);
        });

        _view->onResetStateListener([=]() {
            this->resetState();
        });

        refreshView();
    }

    void refreshView()
    {
        auto [blocks, freeBlocks] = _model->state;
        _view->setMemoryBlocks(blocks);
        _view->setFreeMemoryBlocks(freeBlocks);
        if (_model->task.done())
        {
            _view->setRequest(_model->task.requests().back());
            _view->showInfoMessage("Вы успешно выполнили данное задание");
        }
        else
        {
            auto index = _model->task.completed();
            _view->setRequest(_model->task.requests()[index]);
        }
    }

    void allocateMemory(
        const std::tuple<int32_t, int32_t, uint32_t>& data,
        const MemoryManagement::Types::MemoryState& state
    )
    {
        using namespace MemoryManagement;
        using namespace std::literals;

        auto [pid, size, blockIndex] = data;
        _model->state = state;
        try
        {
            _model->state = Operations::allocateMemory(_model->state, blockIndex, pid, size);
            refreshView();
        }
        catch (const Exceptions::OperationException& ex)
        {
            if (ex.what() == "BLOCK_IS_USED"s)
            {
                _view->showErrorMessage("Данный блок памяти уже выделен другому процессу");
            }
            else if (ex.what() == "TOO_SMALL"s)
            {
                _view->showErrorMessage(
                    "Процесс нельзя поместить в свободный блок, содержащий столько же "
                    "парагрфов, сколько требуется данному процессу"
                );
            }
            else
            {
                _view->showErrorMessage("Неизвестная ошибка: "s + ex.what());
            }
        }
        catch (const std::exception& ex)
        {
            _view->showErrorMessage("Неизвестная ошибка: "s + ex.what());
        }
    }

    void freeMemory(
        const std::tuple<int32_t, uint32_t>& data,
        const MemoryManagement::Types::MemoryState& state
    )
    {
        using namespace MemoryManagement;
        using namespace std::literals;

        auto [pid, blockIndex] = data;
        _model->state = state;
        try
        {
            _model->state = Operations::freeMemory(_model->state, pid, blockIndex);
            refreshView();
        }
        catch (const std::exception& ex)
        {
            _view->showErrorMessage("Неизвестная ошибка: "s + ex.what());
        }
    }

    void defragmentMemory(const MemoryManagement::Types::MemoryState& state)
    {
        using namespace MemoryManagement;
        using namespace std::literals;

        _model->state = state;
        try
        {
            _model->state = Operations::defragmentMemory(_model->state);
            refreshView();
        }
        catch (const std::exception& ex)
        {
            _view->showErrorMessage("Неизвестная ошибка: "s + ex.what());
        }
    }

    void compressMemory(
        uint32_t blockIndex,
        const MemoryManagement::Types::MemoryState& state
    )
    {
        using namespace MemoryManagement;
        using namespace std::literals;

        _model->state = state;
        try
        {
            _model->state = Operations::compressMemory(_model->state, blockIndex);
            refreshView();
        }
        catch (const Exceptions::OperationException& ex)
        {
            if (ex.what() == "SINGLE_BLOCK"s)
            {
                _view->showErrorMessage("Следующий блок свободен или отсутствует");
            }
            else
            {
                _view->showErrorMessage("Неизвестная ошибка: "s + ex.what());
            }
        }
        catch (const std::exception& ex)
        {
            _view->showErrorMessage("Неизвестная ошибка: "s + ex.what());
        }
    }

    void nextRequest(const MemoryManagement::Types::MemoryState& state)
    {
        _model->state = state;
        auto task = _model->task.next(_model->state);
        if (task)
        {
            _model->task = task.value();
            _model->state = _model->task.state();
            refreshView();
        }
        else
        {
            _view->showErrorMessage("Заявка обработана неверно");
        }
    }

    void resetState()
    {
        _model->state = _model->task.state();
        refreshView();
    }
};

class ProcessTaskPresenter : public AbstractTaskPresenter
{
private:
    Views::ProcessTaskView* _view;

    Models::ProcessesModel* _model;

public:
    ProcessTaskPresenter(Views::ProcessTaskView* view, Models::ProcessesModel* model) :
        AbstractTaskPresenter (),
        _view(view), _model(model)
    {}
};

using TaskPresenter = std::shared_ptr<AbstractTaskPresenter>;

class MainWindowPresenter
{
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
                newTaskPresenters.emplace_back(new MemoryTaskPresenter(*view, model));
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
