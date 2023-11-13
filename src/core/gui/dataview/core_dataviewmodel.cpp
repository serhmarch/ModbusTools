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
#include "core_dataviewmodel.h"

#include <core.h>
#include <project/core_project.h>
#include <project/core_dataview.h>

mbCoreDataViewModel::mbCoreDataViewModel(mbCoreDataView *dataView, QObject *parent) :
    QAbstractTableModel(parent)
{
    m_dataView = dataView;
    connect(m_dataView, &mbCoreDataView::itemAdded  , this, &mbCoreDataViewModel::itemAdded);
    connect(m_dataView, &mbCoreDataView::itemRemoved, this, &mbCoreDataViewModel::itemRemoving);
    connect(m_dataView, &mbCoreDataView::itemChanged, this, &mbCoreDataViewModel::itemChanged);
}

mbCoreDataViewModel::~mbCoreDataViewModel()
{
}

int mbCoreDataViewModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_dataView->itemCount();
}

Qt::ItemFlags mbCoreDataViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    f |= Qt::ItemIsEditable;
    return f;
}

QModelIndex mbCoreDataViewModel::itemIndex(mbCoreDataViewItem *item) const
{
    return createIndex(m_dataView->itemIndex(item), 0);
}

mbCoreDataViewItem *mbCoreDataViewModel::itemCore(const QModelIndex &index) const
{
    return m_dataView->itemCore(index.row());
}

void mbCoreDataViewModel::itemAdded(mbCoreDataViewItem * /*item*/)
{
    beginResetModel();
    endResetModel();
}

void mbCoreDataViewModel::itemRemoving(mbCoreDataViewItem * /*item*/)
{
    beginResetModel();
    endResetModel();
}

void mbCoreDataViewModel::itemChanged(mbCoreDataViewItem* item)
{
    int i = m_dataView->itemIndex(item);
    Q_EMIT dataChanged(createIndex(i, 0), createIndex(i, columnCount()));

}

