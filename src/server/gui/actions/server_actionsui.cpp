/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "server_actionsui.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include <server.h>

#include <project/server_project.h>
#include <project/server_action.h>
#include <project/server_device.h>

#include "server_actionsmodel.h"
#include "server_actionsdelegate.h"

mbServerActionsUi::mbServerActionsUi(QWidget *parent) : QWidget(parent)
{

    m_view = new QTableView(this);
    QHeaderView *header;
    header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = m_view->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QAbstractItemView::customContextMenuRequested, this, &mbServerActionsUi::customContextMenu);

    m_model = new mbServerActionsModel(this);
    m_view->setModel(m_model);

    m_delegate = new mbServerActionsDelegate(this);
    connect(m_delegate, &mbServerActionsDelegate::doubleClick, this, &mbServerActionsUi::doubleClick);
    connect(m_delegate, &mbServerActionsDelegate::contextMenu, this, &mbServerActionsUi::contextMenu);
    m_view->setItemDelegate(m_delegate);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);
}

QModelIndex mbServerActionsUi::currentItemModelIndex() const
{
    QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
    if (ls.count())
        return ls.first();
    return QModelIndex();
}

mbServerAction *mbServerActionsUi::currentItem() const
{
    return model()->action(currentItemModelIndex());
}

QList<mbServerAction *> mbServerActionsUi::selectedItems() const
{
    QList<mbServerAction*> r;
    mbServerProject *project = m_model->project();
    if (project)
    {
        QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
        if (!ls.count())
            return r;
        QSet<int> s;
        Q_FOREACH (QModelIndex i, ls)
            s.insert(i.row()); // get unique selected rows
        QList<int> lsi = s.values();
        std::sort(lsi.begin(), lsi.end());
        Q_FOREACH (int i, lsi)
            r.append(project->action(i)); // get unique selected rows
    }
    return r;
}

void mbServerActionsUi::selectItem(mbServerAction *item)
{
    QModelIndex index = m_model->itemIndex(item);
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    QModelIndex firstColumnIndex = index.sibling(index.row(), 0);
    QModelIndex lastColumnIndex = index.sibling(index.row(), index.model()->columnCount() - 1);
    QItemSelection rowSelection(firstColumnIndex, lastColumnIndex);
    selectionModel->clearSelection();
    selectionModel->select(rowSelection, QItemSelectionModel::Select);
}

void mbServerActionsUi::selectAll()
{
    QItemSelection s;
    s.select(m_model->index(0, 0), m_model->index(m_model->rowCount()-1, m_model->columnCount()-1));
    m_view->selectionModel()->select(s, QItemSelectionModel::Select);
}

void mbServerActionsUi::customContextMenu(const QPoint &pos)
{
    contextMenu(m_view->indexAt(pos));
}

void mbServerActionsUi::doubleClick(const QModelIndex &index)
{
    if (mbServerAction *action = m_model->action(index))
        Q_EMIT actionDoubleClick(action);
}

void mbServerActionsUi::contextMenu(const QModelIndex &index)
{
    mbServerAction *action = m_model->action(index);
    Q_EMIT actionContextMenu(action);
}
