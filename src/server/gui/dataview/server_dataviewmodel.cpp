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

void mbServerDataViewModel::refreshValues()
{
    int Column_Value = dataView()->getColumnIndexByType(mbServerDataView::Value);
    if (Column_Value >= 0)
        Q_EMIT dataChanged(createIndex(0, Column_Value), createIndex(rowCount()-1, Column_Value));
}
