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

#include <QBrush>

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

QVariant mbClientDataViewModel::data(const QModelIndex &index, int role) const
{
    mbClientDataViewItem* d = dataView()->item(index.row());
    if (d)
    {
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            int c = dataView()->getColumnTypeByIndex(index.column());
            switch(c)
            {
            case mbCoreDataView::Device:
                if (mbClientDevice *dev = d->device())
                    return dev->name();
                break;
            case mbCoreDataView::Address    : return d->addressStr();
            case mbCoreDataView::Format     : return d->formatStr();
            case mbClientDataView::Period   : return d->period();
            case mbCoreDataView::Comment    : return d->comment();
            case mbClientDataView::Status   : return mb::toString(d->status());
            case mbClientDataView::Timestamp: return mb::toString(d->timestamp());
            case mbCoreDataView::Value      : return d->value();
            }
        }
            break;
        case Qt::BackgroundRole:
        {
            int c = dataView()->getColumnTypeByIndex(index.column());
            switch(c)
            {
            case mbClientDataView::Status:
            {
                int status = d->status();
                if (status == mb::Status_MbStopped)
                    return QBrush(QColor(0xF0, 0xF0, 0xF0));
                if (status == mb::Status_MbInitializing)
                    return QBrush(Qt::lightGray);
                if (Modbus::StatusIsGood(static_cast<Modbus::StatusCode>(status)))
                    return QBrush(QColor(0xCC, 0xFF, 0xCC));
                if (Modbus::StatusIsBad(static_cast<Modbus::StatusCode>(status)))
                    return QBrush(QColor(0xFF, 0xCC, 0xCC));
            }
                break;
            }
        }
            break;
        case Qt::ForegroundRole:
        {
            int c = dataView()->getColumnTypeByIndex(index.column());
            switch(c)
            {
            case mbClientDataView::Status:
                return QBrush(Qt::black);
            }
        }
            break;
        }
    }
    return QVariant();
}

Qt::ItemFlags mbClientDataViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    int c = index.column();
    switch (c)
    {
    case mbCoreDataView::Device:
    case mbCoreDataView::Address:
    case mbClientDataView::Period:
        if (!mbClient::global()->isRunning()) // can be editable when it's no polling mode
            f |= Qt::ItemIsEditable;
        break;
    case mbCoreDataView::Value:
        if (mbClient::global()->isRunning()) // value can be editable only when it's polling mode
            f |= Qt::ItemIsEditable;
        break;
    case mbCoreDataView::Format:
    case mbCoreDataView::Comment:
        f |= Qt::ItemIsEditable;
        break;
    }
    return f;
}

bool mbClientDataViewModel::setDataEdit(const QModelIndex &index, const QVariant &value, int role)
{
    mbClientDataViewItem* d = dataView()->itemAt(index.row());
    if (d && (role == Qt::EditRole))
    {
        switch(index.column())
        {
        case mbClientDataView::Period:
            d->setPeriod(value.toInt());
            return true;
        default:
            return false;
        }
    }
    return false;
}

