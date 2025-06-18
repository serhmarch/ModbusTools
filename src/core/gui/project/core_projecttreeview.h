#ifndef CORE_PROJECTTREEVIEW_H
#define CORE_PROJECTTREEVIEW_H

#include <QTreeView>

class mbCoreProjectTreeView : public QTreeView
{
    Q_OBJECT

public:
    mbCoreProjectTreeView(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private:
    QPoint m_dragStartPosition;
    bool m_dragInProgress = false;
};

#endif // CORE_PROJECTTREEVIEW_H
