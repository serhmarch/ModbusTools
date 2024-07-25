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
#include "server_actionsmodel.h"

#include <server.h>

#include <project/server_project.h>
#include <project/server_action.h>

mbServerActionsModel::mbServerActionsModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    m_project = nullptr;
    mbServer *core = mbServer::global();
    setProject(core->project());
    connect(core, &mbServer::projectChanged, this, &mbServerActionsModel::setProject);
}

QVariant mbServerActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            switch(section)
            {
            case Column_Device:           return QStringLiteral("Device");
            case Column_Address:          return QStringLiteral("Address");
            case Column_DataType:         return QStringLiteral("DataType");
            case Column_Period:           return QStringLiteral("Period");
            case Column_Comment:          return QStringLiteral("Comment");
            case Column_ActionType:       return QStringLiteral("Type");
            case Column_ExtendedSettings: return QStringLiteral("ExtendedSettings");
            }
            break;
        case Qt::Vertical:
            return section+1;
        }
    }
    return QVariant();
}

int mbServerActionsModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (m_project)
        return m_project->actionCount();
    return 0;
}

int mbServerActionsModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnCount;
}

QVariant mbServerActionsModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    if ( m_project &&
        (r >= 0) && (r < rowCount()) &&
        ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
    {
        int c = index.column();
        const mbServerAction *action = m_project->actionAt(r);
        switch(c)
        {
        case Column_Device:
        {
            mbServerDevice *dev = action->device();
            if (dev)
                return dev->name();
            return QString();
        }
        case Column_Address:
            return action->addressStr();
        case Column_DataType:
            return action->dataTypeStr();
        case Column_Period:
            return action->period();
        case Column_Comment:
            return action->comment();
        case Column_ActionType:
            return action->actionTypeStr();
        case Column_ExtendedSettings:
            return action->extendedSettingsStr();
        }
    }
    return QVariant();
}

bool mbServerActionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    if ((r >= 0) && (r < rowCount()) &&
        (role == Qt::EditRole))
    {
        int c = index.column();
        mbServerAction *action = m_project->actionAt(r);
        switch(c)
        {
        case Column_Device:
        {
            mbServerDevice *d = mbServer::global()->project()->device(value.toString());
            if (!d)
                return false;
            action->setDevice(d);
        }
            return true;
        case Column_Address:
            action->setAddressStr(value.toString());
            return true;
        case Column_DataType:
            action->setDataTypeStr(value.toString());
            return true;
        case Column_Period:
            action->setPeriod(value.toInt());
            return true;
        case Column_Comment:
            action->setComment(value.toString());
            return true;
        case Column_ActionType:
            action->setActionTypeStr(value.toString());
            return true;
        case Column_ExtendedSettings:
            action->setExtendedSettingsStr(value.toString());
            return true;
        }
    }
    return false;
}

Qt::ItemFlags mbServerActionsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    f |= Qt::ItemIsEditable;
    return f;
}

QModelIndex mbServerActionsModel::actionIndex(mbServerAction *action) const
{
    if (m_project)
        return createIndex(m_project->actionIndex(action), 0);
    return QModelIndex();
}

mbServerAction *mbServerActionsModel::action(const QModelIndex &index) const
{
    if (m_project)
        return m_project->action(index.row());
    return nullptr;
}

void mbServerActionsModel::setProject(mbCoreProject *project)
{
    beginResetModel();
    if (m_project)
        m_project->disconnect(this);
    m_project = static_cast<mbServerProject*>(project);
    if (m_project)
    {
        connect(m_project, &mbServerProject::actionAdded  , this, &mbServerActionsModel::actionAdd   );
        connect(m_project, &mbServerProject::actionRemoved, this, &mbServerActionsModel::actionRemove);
        connect(m_project, &mbServerProject::actionChanged, this, &mbServerActionsModel::actionChange);
    }
    endResetModel();
}

void mbServerActionsModel::actionAdd(mbServerAction * /*action*/)
{
    beginResetModel();
    endResetModel();
}

void mbServerActionsModel::actionRemove(mbServerAction * /*action*/)
{
    beginResetModel();
    endResetModel();
}

void mbServerActionsModel::actionChange(mbServerAction* action)
{
    int i = m_project->actionIndex(action);
    Q_EMIT dataChanged(createIndex(i, Column_Device), createIndex(i, ColumnCount-1));
}

