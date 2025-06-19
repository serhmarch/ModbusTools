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
#include "server_projectui.h"

#include <QTreeView>

#include <server.h>
#include <project/server_project.h>
#include <project/server_port.h>
#include <project/server_deviceref.h>

#include <gui/server_ui.h>
#include <gui/server_windowmanager.h>
#include <gui/project/core_projecttreeview.h>

#include "server_projectmodel.h"
#include "server_projectdelegate.h"

mbServerProjectUi::mbServerProjectUi(QWidget *parent) :
    mbCoreProjectUi(new mbServerProjectModel, new mbServerProjectDelegate, parent)
{
}

mbServerDeviceRef *mbServerProjectUi::currentDeviceRef() const
{
    QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
    if (!ls.isEmpty())
        return static_cast<mbServerProjectModel*>(m_model)->deviceRef(ls.first());
    return nullptr;
}

void mbServerProjectUi::doubleClick(const QModelIndex &index)
{
    if (mbServerPort *d = static_cast<mbServerProjectModel*>(m_model)->port(index))
    {
        Q_EMIT portDoubleClick(d);
        return;
    }
    if (mbServerDeviceRef *d = static_cast<mbServerProjectModel*>(m_model)->deviceRef(index))
    {
        Q_EMIT deviceDoubleClick(d);
        return;
    }
}

void mbServerProjectUi::contextMenu(const QModelIndex &index)
{
    if (mbServerDeviceRef *d = static_cast<mbServerProjectModel*>(m_model)->deviceRef(index))
    {
        Q_EMIT deviceContextMenu(d);
        return;
    }
    mbServerPort *d = static_cast<mbServerProjectModel*>(m_model)->port(index);
    Q_EMIT portContextMenu(d);
}

void mbServerProjectUi::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList ls = selected.indexes();
    if (ls.count())
    {
        mbServerDeviceRef *ref = static_cast<mbServerProjectModel*>(m_model)->deviceRef(ls.first());
        if (ref)
            Q_EMIT deviceClick(ref->device());
    }
    mbCoreProjectUi::selectionChanged(selected, deselected);
}
