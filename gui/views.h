#pragma once

#include <functional>
#include <vector>
#include <variant>
#include <string>

#include "../schedulers/utils/tasks.h"
#include "../schedulers/algo/memory/types.h"
#include "../schedulers/algo/memory/requests.h"

namespace Views {
	class MemoryTaskView {
    public:
        virtual void setMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) = 0;

        virtual void setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) = 0;

        virtual void setRequest(MemoryManagement::Requests::RequestPtr request) = 0;
	};

	class ProcessTaskView {

	};

    using TaskView = std::variant<MemoryTaskView*, ProcessTaskView*>;

	class MainWindowView {
	public:
		using OnOpenListener = std::function<void(const std::string&)>;

		using OnSaveListener = std::function<void(const std::string&)>;

		MainWindowView() = default;

		virtual void onOpenTaskListener(OnOpenListener listener) = 0;

		virtual void onSaveTaskListener(OnSaveListener listener) = 0;

        virtual std::vector<TaskView>
        createTaskViews(const std::vector<Utils::Tasks::Task>& tasks) = 0;
	};
}
