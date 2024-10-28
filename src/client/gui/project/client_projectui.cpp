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
#include "client_projectui.h"

#include <QTreeView>

#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>

#include "client_projectmodel.h"
#include "client_projectdelegate.h"

mbClientProjectUi::mbClientProjectUi(QWidget *parent) :
    mbCoreProjectUi(new mbClientProjectModel, new mbClientProjectDelegate, parent)
{
}

void mbClientProjectUi::doubleClick(const QModelIndex &index)
{
    if (mbClientPort *d = static_cast<mbClientProjectModel*>(m_model)->port(index))
    {
        Q_EMIT portDoubleClick(d);
        return;
    }
    if (mbClientDevice *d = static_cast<mbClientProjectModel*>(m_model)->device(index))
    {
        Q_EMIT deviceDoubleClick(d);
        return;
    }
}

void mbClientProjectUi::contextMenu(const QModelIndex &index)
{
    if (mbClientDevice *d = static_cast<mbClientProjectModel*>(m_model)->device(index))
    {
        Q_EMIT deviceContextMenu(d);
        return;
    }
    mbClientPort *d = static_cast<mbClientProjectModel*>(m_model)->port(index);
    Q_EMIT portContextMenu(d);
}
