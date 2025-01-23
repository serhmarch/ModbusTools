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
#include "server_simactionsui.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include <server.h>

#include <project/server_project.h>
#include <project/server_simaction.h>
#include <project/server_device.h>

#include "server_simactionsmodel.h"
#include "server_simactionsdelegate.h"

mbServerSimActionsUi::mbServerSimActionsUi(QWidget *parent) : QWidget(parent)
{

    m_view = new QTableView(this);
    QHeaderView *header;
    header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = m_view->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setAlternatingRowColors(true);
    connect(m_view, &QAbstractItemView::customContextMenuRequested, this, &mbServerSimActionsUi::customContextMenu);

    m_model = new mbServerSimActionsModel(this);
    m_view->setModel(m_model);

    m_delegate = new mbServerSimActionsDelegate(this);
    connect(m_delegate, &mbServerSimActionsDelegate::doubleClick, this, &mbServerSimActionsUi::doubleClick);
    connect(m_delegate, &mbServerSimActionsDelegate::contextMenu, this, &mbServerSimActionsUi::contextMenu);
    m_view->setItemDelegate(m_delegate);

    QString headerStyleSheet = R"(
    QHeaderView::section {
        background-color: #f0f0f0;        /* Light gray background */
        color: #2c3e50;                   /* Dark gray text color */
        border: 1px solid #dcdcdc;        /* Subtle light gray border around sections */
        font-size: 11px;                  /* Font size */
        font-weight: normal;              /* Normal text weight for a clean look */
        text-align: left;                 /* Align text to the left */
    }

    QHeaderView::section:pressed {
        background-color: #d0d0d0;        /* Darker background when pressed */
        border: 1px solid #bcbcbc;        /* Darker border when pressed */
    }

    )";

    m_view->setStyleSheet(headerStyleSheet);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);
}

QModelIndex mbServerSimActionsUi::currentItemModelIndex() const
{
    QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
    if (ls.count())
        return ls.first();
    return QModelIndex();
}

mbServerSimAction *mbServerSimActionsUi::currentItem() const
{
    return model()->simAction(currentItemModelIndex());
}

QList<mbServerSimAction *> mbServerSimActionsUi::selectedItems() const
{
    QList<mbServerSimAction*> r;
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
            r.append(project->simAction(i)); // get unique selected rows
    }
    return r;
}

void mbServerSimActionsUi::selectItem(mbServerSimAction *item)
{
    QModelIndex index = m_model->itemIndex(item);
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    QModelIndex firstColumnIndex = index.sibling(index.row(), 0);
    QModelIndex lastColumnIndex = index.sibling(index.row(), index.model()->columnCount() - 1);
    QItemSelection rowSelection(firstColumnIndex, lastColumnIndex);
    selectionModel->clearSelection();
    selectionModel->select(rowSelection, QItemSelectionModel::Select);
}

void mbServerSimActionsUi::selectAll()
{
    QItemSelection s;
    s.select(m_model->index(0, 0), m_model->index(m_model->rowCount()-1, m_model->columnCount()-1));
    m_view->selectionModel()->select(s, QItemSelectionModel::Select);
}

void mbServerSimActionsUi::customContextMenu(const QPoint &pos)
{
    contextMenu(m_view->indexAt(pos));
}

void mbServerSimActionsUi::doubleClick(const QModelIndex &index)
{
    if (mbServerSimAction *simAction = m_model->simAction(index))
        Q_EMIT simActionDoubleClick(simAction);
}

void mbServerSimActionsUi::contextMenu(const QModelIndex &index)
{
    mbServerSimAction *simAction = m_model->simAction(index);
    Q_EMIT simActionContextMenu(simAction);
}
