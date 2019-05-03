#include <QDebug>

#include "queuelistwidget.h"

QueueListWidget::QueueListWidget(QWidget *parent) : QListWidget(parent) {
  setDragEnabled(true);
  setDragDropOverwriteMode(true);
  setDragDropMode(DragDropMode::InternalMove);
  setDefaultDropAction(Qt::DropAction::MoveAction);
}

void QueueListWidget::dropEvent(QDropEvent *event) {
  QListWidget::dropEvent(event);
  emit itemsOrderChanged();
}
