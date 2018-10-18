#pragma once

#include <string>
#include <QDebug>

#include "views.h"

namespace Presenters {
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
                qDebug() << "open: " << QString::fromStdString(path);
			});

            view->onSaveTaskListener([](const std::string& path) {
                qDebug() << "save: " << QString::fromStdString(path);
			});
		}
	};
}
