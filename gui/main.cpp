#include <wx/wx.h>

#include <memory>

#include "widgets.h"
#include "presenters.h"
#include "views.h"

using namespace Ui;
using std::shared_ptr;
using std::make_shared;

class MyApp : public wxApp {
private:
	Widgets::MainWidget* mainWidget;
	shared_ptr<Presenters::MainWindowPresenter> presenter;
public:
	virtual bool OnInit()
	{
		mainWidget = new Widgets::MainWidget();
		presenter = make_shared<Presenters::MainWindowPresenter>(mainWidget);
		mainWidget->Show(true);
		return true;
	}
};

wxIMPLEMENT_APP(MyApp);
