#include "newsview.h"
#include "delegatewithoutfocus.h"

NewsView::NewsView(QWidget * parent)
  : QTreeView(parent)
{
  setObjectName("newsView_");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setMinimumWidth(120);
  setSortingEnabled(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  DelegateWithoutFocus *itemDelegate = new DelegateWithoutFocus(this);
  setItemDelegate(itemDelegate);
  setContextMenuPolicy(Qt::CustomContextMenu);
}

/*virtual*/ void NewsView::mousePressEvent(QMouseEvent *event)
{
  if (!indexAt(event->pos()).isValid()) return;
  indexClicked_ = indexAt(event->pos());

  QModelIndex index = indexAt(event->pos());
  QSqlTableModel *model_ = (QSqlTableModel*)model();
  if (event->buttons() & Qt::LeftButton) {
    if (index.column() == model_->fieldIndex("starred")) {
      if (index.data(Qt::EditRole).toInt() == 0) {
        emit signalSetItemStar(index, 1);
      } else {
        emit signalSetItemStar(index, 0);
      }
      event->ignore();
      return;
    } else if (index.column() == model_->fieldIndex("read")) {
      if (index.data(Qt::EditRole).toInt() == 0) {
        emit signalSetItemRead(index, 1);
      } else {
        emit signalSetItemRead(index, 0);
      }
      event->ignore();
      return;
    } else if (index.column() == model_->fieldIndex("label")) {
      if (QApplication::keyboardModifiers() == Qt::NoModifier) {
        emit signaNewslLabelClicked(index);
        event->ignore();
        return;
      }
    }
    if (QApplication::keyboardModifiers() == Qt::AltModifier) {
      emit signalMiddleClicked(index);
      event->ignore();
      return;
    }
  } else if ((event->buttons() & Qt::MiddleButton)) {
    emit signalMiddleClicked(index);
    return;
  } else {
    if (selectionModel()->selectedRows(0).count() > 1)
      return;
  }
  QTreeView::mousePressEvent(event);
}

/*virtual*/ void NewsView::mouseMoveEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
}

/*virtual*/ void NewsView::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (!indexAt(event->pos()).isValid()) return;

  if (indexClicked_ == indexAt(event->pos()))
    emit signalDoubleClicked(indexAt(event->pos()));
  else
    mousePressEvent(event);
}

/*virtual*/ void NewsView::keyPressEvent(QKeyEvent *event)
{
  QTreeView::keyPressEvent(event);
  if (event->key() == Qt::Key_Up)         emit pressKeyUp(currentIndex());
  else if (event->key() == Qt::Key_Down)  emit pressKeyDown(currentIndex());
  else if (event->key() == Qt::Key_Home)  emit pressKeyHome(currentIndex());
  else if (event->key() == Qt::Key_End)   emit pressKeyEnd(currentIndex());
  else if (event->key() == Qt::Key_PageUp)   emit pressKeyPageUp(currentIndex());
  else if (event->key() == Qt::Key_PageDown) emit pressKeyPageDown(currentIndex());
}
