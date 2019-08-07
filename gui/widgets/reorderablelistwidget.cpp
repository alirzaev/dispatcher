#include "reorderablelistwidget.h"

ReorderableListWidget::ReorderableListWidget(QWidget *parent)
    : QListWidget(parent) {}

void ReorderableListWidget::dropEvent(QDropEvent *event) {
  QListWidget::dropEvent(event);
  emit itemsOrderChanged();
}
