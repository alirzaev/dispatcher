#pragma once

#include <cstdint>

#include <QDialog>

#include <tl/optional.hpp>

namespace Ui {
class PidInputDialog;
}

class PidInputDialog : public QDialog {
  Q_OBJECT

public:
  ~PidInputDialog() override;

  static tl::optional<int32_t> getPid(QWidget *parent);

private:
  explicit PidInputDialog(QWidget *parent = nullptr);

  int32_t info;

  Ui::PidInputDialog *ui;

  void tryAccept();
};
