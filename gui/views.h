#pragma once

#include <functional>

namespace Views {
	class MemoryTaskView {

	};

	class ProcessTaskView {

	};

	class MainWindowView {
	public:
		using OnOpenListener = std::function<void(const std::string&)>;

		using OnSaveListener = std::function<void(const std::string&)>;

		MainWindowView() = default;

		virtual void onOpenTaskListener(OnOpenListener listener) = 0;

		virtual void onSaveTaskListener(OnSaveListener listener) = 0;
	};
}
