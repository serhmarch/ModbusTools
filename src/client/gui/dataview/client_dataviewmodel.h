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
#ifndef CLIENT_DATAVIEWMODEL_H
#define CLIENT_DATAVIEWMODEL_H

#include <core/gui/dataview/core_dataviewmodel.h>

class mbClientDataView;
class mbClientDataViewItem;

class mbClientDataViewModel : public mbCoreDataViewModel
{
    Q_OBJECT

public:
    enum Column
    {
        Column_Device,
        Column_Address,
        Column_Format,
        Column_Period,
        Column_Comment,
        Column_Status,
        Column_Timestamp,
        Column_Value,
        ColumnCount
    };

public:
    mbClientDataViewModel(mbClientDataView *dataView, QObject* parent = nullptr);
    ~mbClientDataViewModel();

public:
    inline mbClientDataView *dataView() const { return reinterpret_cast<mbClientDataView*>(dataViewCore()); }
    inline mbClientDataViewItem *item(const QModelIndex &index) const { return reinterpret_cast<mbClientDataViewItem*>(itemCore(index)); }

public:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // CLIENT_DATAVIEWMODEL_H
