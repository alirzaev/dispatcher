#pragma once

#include <cstdint>
#include <utility>

#include <QDialog>
#include <QWidget>

#include <tl/optional.hpp>

namespace Ui {
class AllocateMemoryDialog;
}

class AllocateMemoryDialog : public QDialog {
  Q_OBJECT

public:
  ~AllocateMemoryDialog() override;

  static tl::optional<std::pair<int32_t, int32_t>>
  getMemoryBlockInfo(QWidget *parent, int32_t availablePages);

private:
  explicit AllocateMemoryDialog(QWidget *parent, int32_t availablePages);

  std::pair<int32_t, int32_t> info;

  Ui::AllocateMemoryDialog *ui;

  int32_t availablePages;

  void tryAccept();
};
