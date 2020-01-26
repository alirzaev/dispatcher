#pragma once

#include <cstdint>
#include <utility>

#include <QDialog>

#include <tl/optional.hpp>

namespace Ui {
class FreeMemoryDialog;
}

class FreeMemoryDialog : public QDialog {
  Q_OBJECT

public:
  ~FreeMemoryDialog();

  static tl::optional<std::pair<int32_t, int32_t>>
  getFreeMemoryInfo(QWidget *parent);

private:
  explicit FreeMemoryDialog(QWidget *parent = nullptr);

  std::pair<int32_t, int32_t> info;

  Ui::FreeMemoryDialog *ui;

  void tryAccept();
};
