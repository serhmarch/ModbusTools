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
#include "server_dataviewmodel.h"

#include <server.h>
#include <project/server_project.h>
#include <project/server_device.h>
#include <project/server_dataview.h>

mbServerDataViewModel::mbServerDataViewModel(mbServerDataView *dataView, QObject *parent) :
    mbCoreDataViewModel(dataView, parent)
{
}

mbServerDataViewModel::~mbServerDataViewModel()
{
}

QVariant mbServerDataViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            switch(section)
            {
            case Column_Device : return QStringLiteral("Device");
            case Column_Address: return QStringLiteral("Address");
            case Column_Format : return QStringLiteral("Format");
            case Column_Comment: return QStringLiteral("Comment");
            case Column_Value  : return QStringLiteral("Value");
            }
            break;
        case Qt::Vertical:
            return section+1;
        }
    }
    return QVariant();
}

int mbServerDataViewModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnCount;
}

QVariant mbServerDataViewModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    if ((r >= 0) && (r < rowCount()) &&
        ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
    {
        int c = index.column();
        const mbServerDataViewItem *item = dataView()->itemAt(r);
        switch(c)
        {
        case Column_Device:
        {
            mbServerDevice *dev = item->device();
            if (dev)
                return dev->name();
            return QString();
        }
        case Column_Address:
            return item->addressStr();
        case Column_Format:
            return item->formatStr();
        case Column_Comment:
            return item->comment();
        case Column_Value:
            return item->value();
        }
    }
    return QVariant();
}

bool mbServerDataViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    if ((r >= 0) && (r < rowCount()) &&
        (role == Qt::EditRole))
    {
        int c = index.column();
        mbServerDataViewItem *item = dataView()->itemAt(r);
        switch(c)
        {
        case Column_Device:
        {
            mbServerDevice *d = mbServer::global()->project()->device(value.toString());
            if (!d)
                return false;
            item->setDevice(d);
        }
            return true;
        case Column_Address:
            item->setAddressStr(value.toString());
            return true;
        case Column_Format:
            item->setFormatStr(value.toString());
            return true;
        case Column_Comment:
            item->setComment(value.toString());
            return true;
        case Column_Value:
            item->setValue(value);
        }
    }
    return false;
}

Qt::ItemFlags mbServerDataViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = mbCoreDataViewModel::flags(index);
    f |= Qt::ItemIsEditable;
    return f;
}

void mbServerDataViewModel::refreshValues()
{
    Q_EMIT dataChanged(createIndex(0, Column_Value), createIndex(rowCount()-1, Column_Value));
}
