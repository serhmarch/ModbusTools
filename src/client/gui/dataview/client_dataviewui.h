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
#ifndef CLIENT_DATAVIEWUI_H
#define CLIENT_DATAVIEWUI_H

#include <core/gui/dataview/core_dataviewui.h>

class mbClientDataView;
class mbClientDataViewItem;
class mbClientDataViewModel;
class mbClientDataViewDelegate;

class mbClientDataViewUi : public mbCoreDataViewUi
{
    Q_OBJECT

public:
    mbClientDataViewUi(mbClientDataView *dataView, QWidget *parent = nullptr);

public:
    inline mbClientDataView *dataView() const { return reinterpret_cast<mbClientDataView *>(dataViewCore()); }
    inline mbClientDataViewModel *model() const { return reinterpret_cast<mbClientDataViewModel*>(modelCore()); }
    QList<mbClientDataViewItem*> selectedItems() const;
};

#endif // CLIENT_DATAVIEWUI_H
