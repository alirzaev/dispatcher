#pragma once

#include <QDialog>
#include <QWidget>

namespace Ui {
class AboutWindow;
}

class AboutWindow : public QDialog {
  Q_OBJECT

public:
  explicit AboutWindow(QWidget *parent = nullptr);
  ~AboutWindow() override;

private:
  Ui::AboutWindow *ui;
};
