#pragma once

#include <cstdint>
#include <utility>

#include <QDialog>

namespace Ui {
class AllocateMemoryDialog;
}

class AllocateMemoryDialog : public QDialog {
  Q_OBJECT

public:
  std::pair<int32_t, int32_t> data;

  explicit AllocateMemoryDialog(QWidget *parent = nullptr,
                                int32_t maxSize = 256);

  ~AllocateMemoryDialog() override;

private:
  Ui::AllocateMemoryDialog *ui;

  int32_t maxSize;

  void tryAccept();
};
