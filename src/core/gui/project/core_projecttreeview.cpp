#include "core_projecttreeview.h"

#include <QApplication>
#include <QTreeView>
#include <QMouseEvent>
#include <QDrag>

mbCoreProjectTreeView::mbCoreProjectTreeView(QWidget *parent) : QTreeView(parent)
{

}

void mbCoreProjectTreeView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragStartPosition = event->pos();
    QTreeView::mousePressEvent(event);
}

void mbCoreProjectTreeView::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;
    QModelIndex index = this->indexAt(m_dragStartPosition);
    if (!index.isValid())
        return;
    if (!index.parent().isValid()) // Note: invalid parent index means that is port
        return;
    m_dragInProgress = true;
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = model()->mimeData({index});
    drag->setMimeData(mimeData);
    drag->setPixmap(QPixmap(":/core/icons/device_small.png"));
    drag->exec(Qt::MoveAction | Qt::CopyAction);
    m_dragInProgress = false;
}

void mbCoreProjectTreeView::dropEvent(QDropEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid())
        model()->dropMimeData(event->mimeData(), event->dropAction(), index.row(), index.column(), index.parent());
}

void mbCoreProjectTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void mbCoreProjectTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void mbCoreProjectTreeView::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
