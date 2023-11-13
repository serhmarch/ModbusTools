#include "client_itemsui.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include <client.h>

#include <project/client_project.h>

#include "client_itemsmodel.h"
#include "client_itemsdelegate.h"

mbClientItemsUi::mbClientItemsUi(QWidget *parent)
    : QWidget{parent}
{
    m_view = new QTableView(this);
    QHeaderView *header;
    header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = m_view->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QAbstractItemView::customContextMenuRequested, this, &mbClientItemsUi::customContextMenu);

    m_model = new mbClientItemsModel(this);
    m_view->setModel(m_model);

    m_delegate = new mbClientItemsDelegate(this);
    connect(m_delegate, SIGNAL(doubleClick(QModelIndex)), this, SLOT(doubleClick(QModelIndex)));
    connect(m_delegate, SIGNAL(contextMenu(QModelIndex)), this, SLOT(contextMenu(QModelIndex)));
    m_view->setItemDelegate(m_delegate);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);
}

QList<mbClientItem*> mbClientItemsUi::currentItems() const
{
    QList<mbClientItem*> res;
    mbClientProject *prj = mbClient::global()->project();
    if (prj)
    {
        QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
        if (!ls.count())
            return res;
        QSet<int> r;
        Q_FOREACH (QModelIndex i, ls)
            r.insert(i.row()); // get unique selected rows
        QList<int> rows = r.values();
        std::sort(rows.begin(), rows.end()); // sorting rows
        Q_FOREACH (const int& i, rows)
            res.append(prj->itemAt(i));
    }
    return res;
}

void mbClientItemsUi::selectAll()
{
    QItemSelection s;
    s.select(m_model->index(0, 0), m_model->index(m_model->rowCount()-1, m_model->columnCount()-1));
    m_view->selectionModel()->select(s, QItemSelectionModel::Select);
}

void mbClientItemsUi::customContextMenu(const QPoint &pos)
{
    contextMenu(m_view->indexAt(pos));
}

void mbClientItemsUi::doubleClick(const QModelIndex &index)
{
    if (mbClientItem *item = m_model->item(index))
        Q_EMIT itemDoubleClick(item);
}

void mbClientItemsUi::contextMenu(const QModelIndex &index)
{
    mbClientItem *item = m_model->item(index);
    Q_EMIT itemContextMenu(item);
}
