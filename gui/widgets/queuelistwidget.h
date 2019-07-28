#pragma once

#include <QDropEvent>
#include <QListWidget>
#include <QObject>
#include <QWidget>

class QueueListWidget : public QListWidget {
  Q_OBJECT
public:
  QueueListWidget(QWidget *parent = nullptr);

protected:
  void dropEvent(QDropEvent *event) override;
signals:
  void itemsOrderChanged();
};
