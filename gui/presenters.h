#pragma once

#include <string>
#include <wx/log.h>

#include "views.h"

namespace Ui::Presenters {
	class MemoryTaskPresenter {

	};

	class ProcessTaskPresenter {

	};

	class MainWindowPresenter {
	private:
		Views::MainWindowView* _view;

	public:
		MainWindowPresenter(Views::MainWindowView* view) :
			_view(view)
		{
			view->onOpenTaskListener([](const std::string& path) {
				wxLogDebug(path.c_str());
			});

			view->onSaveTaskListener([](const std::string& path) {
				wxLogDebug(path.c_str());
			});
		}
	};
}