#pragma once

#include <QListWidget>
#include <QObject>
#include <QWidget>

class ReorderableListWidget : public QListWidget {
  Q_OBJECT
public:
  ReorderableListWidget(QWidget *parent = nullptr);

protected:
  void dropEvent(QDropEvent *event) override;

signals:
  void itemsOrderChanged();
};
