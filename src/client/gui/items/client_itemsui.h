#ifndef CLIENT_ITEMSUI_H
#define CLIENT_ITEMSUI_H

#include <QWidget>

class QTableView;

class mbClientItem;
class mbClientItemsModel;
class mbClientItemsDelegate;

class mbClientItemsUi : public QWidget
{
    Q_OBJECT
public:
    explicit mbClientItemsUi(QWidget *parent = nullptr);

public:
    QList<mbClientItem*> currentItems() const;

public Q_SLOTS:
    void selectAll();

Q_SIGNALS:
    void itemDoubleClick(mbClientItem *item);
    void itemContextMenu(mbClientItem *item);

protected Q_SLOTS:
    void customContextMenu(const QPoint &pos);
    void doubleClick(const QModelIndex &index);
    void contextMenu(const QModelIndex &index);

private:
    QTableView *m_view;
    mbClientItemsModel *m_model;
    mbClientItemsDelegate *m_delegate;
};

#endif // CLIENT_ITEMSUI_H
