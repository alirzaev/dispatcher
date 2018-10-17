#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/filedlg.h>

#include <functional>
#include <string>

#include "views.h"

namespace Ui::Widgets {
	class MainWidget : public wxFrame, public Views::MainWindowView {
	public:

		MainWidget() :
			wxFrame(nullptr, wxID_ANY, "Лабораторная установка"),
			Views::MainWindowView()
		{
			SetMinSize({ 640, 480 });
			setupMenuBar();
			setupTabs();
		}

		void onOpenTaskListener(OnOpenListener listener) override
		{
			_onOpenListener = listener;
		}

		void onSaveTaskListener(OnSaveListener listener) override
		{
			_onSaveListener = listener;
		}

	private:
		OnOpenListener _onOpenListener;

		OnSaveListener _onSaveListener;

		enum MenuItems
		{
			ID_Open,
			ID_Save,
			ID_Quit
		};

		void setupTabs()
		{
			auto* notebook = new wxNotebook(this, wxID_ANY);

			auto* page = new wxControl(notebook, wxID_ANY);
			notebook->AddPage(page, "Задание №1");
		}

		void setupMenuBar()
		{
			auto* menuTask = new wxMenu();
			menuTask->Append(MenuItems::ID_Open, "&Открыть\tCtrl-O", "");
			menuTask->Append(MenuItems::ID_Save, "Сох&ранить\tCtrl-S", "");
			menuTask->AppendSeparator();
			menuTask->Append(MenuItems::ID_Quit, "Вы&ход\tCtrl-Q", "");

			auto* menuBar = new wxMenuBar();
			menuBar->Append(menuTask, "З&адание");
			SetMenuBar(menuBar);

			Bind(wxEVT_MENU, &MainWidget::onOpen, this, MenuItems::ID_Open);
			Bind(wxEVT_MENU, &MainWidget::onSave, this, MenuItems::ID_Save);
			Bind(wxEVT_MENU, &MainWidget::onQuit, this, MenuItems::ID_Quit);
		}

		void onOpen(wxCommandEvent& event)
		{
			wxFileDialog openFileDialog{
				this, "Открыть файл задания",
				"", "", "*.json",
				wxFD_OPEN
			};

			if (openFileDialog.ShowModal() == wxID_CANCEL) {
				return;
			}
			else if (_onOpenListener) {
				_onOpenListener(openFileDialog.GetPath().ToStdString());
			}
		}

		void onSave(wxCommandEvent& event)
		{
			wxFileDialog saveFileDialog{
				this, "Сохранить задание",
				"", "", "*.json",
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT
			};

			if (saveFileDialog.ShowModal() == wxID_CANCEL) {
				return;
			}
			else if (_onSaveListener) {
				_onSaveListener(saveFileDialog.GetPath().ToStdString());
			}
		}

		void onQuit(wxCommandEvent& event)
		{
			Close(true);
		}
	};
}