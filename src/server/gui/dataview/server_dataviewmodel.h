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
#ifndef SERVER_DATAVIEWMODEL_H
#define SERVER_DATAVIEWMODEL_H

#include <core/gui/dataview/core_dataviewmodel.h>

class mbServerDataView;
class mbServerDataViewItem;

class mbServerDataViewModel : public mbCoreDataViewModel
{
    Q_OBJECT
public:
    enum Column
    {
        Column_Device,
        Column_Address,
        Column_Format,
        Column_Comment,
        Column_Value,
        ColumnCount
    };

public:
    mbServerDataViewModel(mbServerDataView *dataView, QObject* parent = nullptr);
    ~mbServerDataViewModel();

public:
    inline mbServerDataView *dataView() const { return reinterpret_cast<mbServerDataView*>(dataViewCore()); }
    inline mbServerDataViewItem *item(const QModelIndex &index) const { return reinterpret_cast<mbServerDataViewItem*>(itemCore(index)); }

public: // table model interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

public:
    void refreshValues();
};

#endif // SERVER_DATAVIEWMODEL_H
