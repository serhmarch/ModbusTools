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
#include "core_projectmodel.h"

#include <project/core_project.h>
#include <project/core_port.h>

#include <gui/core_ui.h>

mbCoreProjectModel::mbCoreProjectModel(QObject* parent) :
    QAbstractItemModel (parent)
{
    m_project = nullptr;
    m_settings.useNameWithSettings = mbCoreUi::Defaults::instance().settings_useNameWithSettings;
    mbCore *core = mbCore::globalCore();
    connect(core, &mbCore::projectChanged, this, &mbCoreProjectModel::setProject);
}

QVariant mbCoreProjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            switch(section)
            {
            case 0: return QStringLiteral("Name");
            }
            break;
        default:
            break;
        }
    }
    return QVariant();
}

int mbCoreProjectModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

bool mbCoreProjectModel::hasChildren(const QModelIndex &parent) const
{
    return rowCount(parent) > 0;
}

Qt::ItemFlags mbCoreProjectModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    return f;
}

void mbCoreProjectModel::setUseNameWithSettings(bool use)
{
    if (m_settings.useNameWithSettings != use)
    {
        beginResetModel();
        m_settings.useNameWithSettings = use;
        endResetModel();
    }
}

QModelIndex mbCoreProjectModel::portIndex(mbCorePort *port) const
{
    if (m_project)
    {
        int i = m_project->portIndex(port);
        return createIndex(i, 0, port);
    }
    return QModelIndex();
}

mbCorePort *mbCoreProjectModel::portCore(const QModelIndex &index) const
{
    if (index.parent().isValid() || !m_project)
        return nullptr;
    return reinterpret_cast<mbCorePort*>(index.internalPointer());
}

void mbCoreProjectModel::setProject(mbCoreProject *project)
{
    if (m_project)
    {
        Q_FOREACH (mbCorePort* p, m_project->portsCore())
            portRemove(p);
        m_project->disconnect(this);
    }
    m_project = static_cast<mbCoreProject*>(project);
    if (m_project)
    {
        connect(m_project, &mbCoreProject::portAdded   , this, &mbCoreProjectModel::portAdd   );
        connect(m_project, &mbCoreProject::portRemoving, this, &mbCoreProjectModel::portRemove);
        Q_FOREACH (mbCorePort* p, m_project->portsCore())
            portAdd(p);
    }
}

void mbCoreProjectModel::portAdd(mbCorePort *port)
{
    int i = m_project->portIndex(port);
    beginInsertRows(QModelIndex(), i, i);
    connect(port, &mbCorePort::changed, this, &mbCoreProjectModel::portChanged);
    endInsertRows();
}

void mbCoreProjectModel::portRemove(mbCorePort *port)
{
    int i = m_project->portIndex(port);
    beginRemoveRows(QModelIndex(), i, i);
    port->disconnect(this);
    endRemoveRows();
}

void mbCoreProjectModel::portChanged()
{
    mbCorePort* p = static_cast<mbCorePort*>(sender());
    QModelIndex index = portIndex(p);
    Q_EMIT dataChanged(index, index);
}
