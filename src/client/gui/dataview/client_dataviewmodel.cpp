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
#include "client_dataviewmodel.h"

#include <core.h>
#include <project/client_project.h>
#include <project/client_device.h>
#include <project/client_dataview.h>

mbClientDataViewModel::mbClientDataViewModel(mbClientDataView *dataView, QObject *parent) :
    mbCoreDataViewModel(dataView, parent)
{
}

mbClientDataViewModel::~mbClientDataViewModel()
{
}

QVariant mbClientDataViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            switch(section)
            {
            case Column_Device   : return QStringLiteral("Device");
            case Column_Address  : return QStringLiteral("Address");
            case Column_Format   : return QStringLiteral("Format");
            case Column_Period   : return QStringLiteral("Period");
            case Column_Comment  : return QStringLiteral("Comment");
            case Column_Status   : return QStringLiteral("Status");
            case Column_Timestamp: return QStringLiteral("Timestamp");
            case Column_Value    : return QStringLiteral("Value");
            }
            break;
        case Qt::Vertical:
            return section+1;
        }
    }
    return QVariant();
}

int mbClientDataViewModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnCount;
}

QVariant mbClientDataViewModel::data(const QModelIndex &index, int role) const
{
    mbClientDataViewItem* d = dataView()->item(index.row());
    if (d && ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
    {
        switch(index.column())
        {
        case Column_Device:
            if (mbClientDevice *dev = d->device())
                return dev->name();
            break;
        case Column_Address  : return d->addressStr();
        case Column_Format   : return d->formatStr();
        case Column_Period   : return d->period();
        case Column_Comment  : return d->comment();
        case Column_Status   : return mb::toString(d->status());
        case Column_Timestamp: return mb::toString(d->timestamp());
        case Column_Value    : return d->value();
        }
    }
    return QVariant();
}

bool mbClientDataViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    mbClientDataViewItem* d = dataView()->itemAt(index.row());
    if (d && (role == Qt::EditRole))
    {
        switch(index.column())
        {
        case Column_Device:
            if (mbClientDevice *dev = dataView()->project()->device(value.toString()))
            {
                d->setDevice(dev);
                return true;
            }
            return false;
        case Column_Address:
            d->setAddressStr(value.toString());
            return true;
        case Column_Format:
            d->setFormatStr(value.toString());
            return true;
        case Column_Period:
            d->setPeriod(value.toInt());
            return true;
        case Column_Comment:
            d->setComment(value.toString());
            return true;
        case Column_Value:
            d->setValue(value.toString());
            return true;
        default:
            return false;
        }
    }
    return false;
}

Qt::ItemFlags mbClientDataViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    int c = index.column();
    switch (c)
    {
    case Column_Device:
    case Column_Address:
    case Column_Period:
        if (!mbClient::global()->isRunning()) // can be editable when it's no polling mode
            f |= Qt::ItemIsEditable;
        break;
    case Column_Value:
        if (mbClient::global()->isRunning()) // value can be editable only when it's polling mode
            f |= Qt::ItemIsEditable;
        break;
    case Column_Format:
    case Column_Comment:
        f |= Qt::ItemIsEditable;
        break;
    }
    return f;
}

