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
    connect(m_dataView, &mbCoreDataView::addressNotationChanged, this, &mbCoreDataViewModel::reset);
    connect(m_dataView, &mbCoreDataView::columnsChanged, this, &mbCoreDataViewModel::reset);

    connect(mbCore::globalCore(), &mbCore::addressNotationChanged, this, &mbCoreDataViewModel::reset);
}

mbCoreDataViewModel::~mbCoreDataViewModel()
{
}

QVariant mbCoreDataViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            return m_dataView->getColumnNameByIndex(section);
        case Qt::Vertical:
            return section+1;
        }
    }
    return QVariant();
}

int mbCoreDataViewModel::columnCount(const QModelIndex &parent) const
{
    return m_dataView->getColumnCount();
}

int mbCoreDataViewModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_dataView->itemCount();
}

QVariant mbCoreDataViewModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        int c = m_dataView->getColumnTypeByIndex(index.column());
        const mbCoreDataViewItem *item = m_dataView->itemCoreAt(r);
        switch(c)
        {
        case mbCoreDataView::Device:
        {
            mbCoreDevice *dev = item->deviceCore();
            if (dev)
                return dev->name();
            return QString();
        }
        case mbCoreDataView::Address:
            return item->addressStr();
        case mbCoreDataView::Format:
            return item->formatStr();
        case mbCoreDataView::Comment:
            return item->comment();
        case mbCoreDataView::Value:
            return item->value();
        default:
            return dataDisplayEdit(index, role);
        }
    }
    return QVariant();
}

bool mbCoreDataViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    if ((r >= 0) && (r < rowCount()) &&
        (role == Qt::EditRole))
    {
        int c = m_dataView->getColumnTypeByIndex(index.column());
        mbCoreDataViewItem *item = m_dataView->itemCoreAt(r);
        switch(c)
        {
        case mbCoreDataView::Device:
        {
            mbCoreDevice *d = mbCore::globalCore()->projectCore()->deviceCore(value.toString());
            if (!d)
                return false;
            item->setDeviceCore(d);
        }
            return true;
        case mbCoreDataView::Address:
            item->setAddressStr(value.toString());
            return true;
        case mbCoreDataView::Format:
            item->setFormatStr(value.toString());
            return true;
        case mbCoreDataView::Comment:
            item->setComment(value.toString());
            return true;
        case mbCoreDataView::Value:
            item->setValue(value);
            return true;
        default:
            return setDataEdit(index, value, role);
        }
    }
    return false;
}

Qt::ItemFlags mbCoreDataViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    f |= Qt::ItemIsEditable;
    return f;
}

QVariant mbCoreDataViewModel::dataDisplayEdit(const QModelIndex &/*index*/, int /*role*/) const
{
    return QVariant();
}

bool mbCoreDataViewModel::setDataEdit(const QModelIndex &/*index*/, const QVariant &/*value*/, int /*role*/)
{
    return false;
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

void mbCoreDataViewModel::reset()
{
    beginResetModel();
    endResetModel();
}

