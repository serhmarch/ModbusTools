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
#include "server_simactionsmodel.h"

#include <server.h>

#include <project/server_project.h>
#include <project/server_simaction.h>

mbServerSimActionsModel::mbServerSimActionsModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    m_project = nullptr;
    mbServer *core = mbServer::global();
    setProject(core->project());
    connect(core, &mbServer::projectChanged, this, &mbServerSimActionsModel::setProject);
}

QVariant mbServerSimActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int mbServerSimActionsModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (m_project)
        return m_project->simActionCount();
    return 0;
}

int mbServerSimActionsModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnCount;
}

QVariant mbServerSimActionsModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    if ( m_project &&
        (r >= 0) && (r < rowCount()) &&
        ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
    {
        int c = index.column();
        const mbServerSimAction *simAction = m_project->simActionAt(r);
        switch(c)
        {
        case Column_Device:
        {
            mbServerDevice *dev = simAction->device();
            if (dev)
                return dev->name();
            return QString();
        }
        case Column_Address:
            return simAction->addressStr();
        case Column_DataType:
            return simAction->dataTypeStr();
        case Column_Period:
            return simAction->period();
        case Column_Comment:
            return simAction->comment();
        case Column_ActionType:
            return simAction->actionTypeStr();
        case Column_ExtendedSettings:
            return simAction->extendedSettingsStr();
        }
    }
    return QVariant();
}

bool mbServerSimActionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    if ((r >= 0) && (r < rowCount()) &&
        (role == Qt::EditRole))
    {
        int c = index.column();
        mbServerSimAction *simAction = m_project->simActionAt(r);
        switch(c)
        {
        case Column_Device:
        {
            mbServerDevice *d = mbServer::global()->project()->device(value.toString());
            if (!d)
                return false;
            simAction->setDevice(d);
        }
            return true;
        case Column_Address:
            simAction->setAddressStr(value.toString());
            return true;
        case Column_DataType:
            simAction->setDataTypeStr(value.toString());
            return true;
        case Column_Period:
            simAction->setPeriod(value.toInt());
            return true;
        case Column_Comment:
            simAction->setComment(value.toString());
            return true;
        case Column_ActionType:
            simAction->setActionTypeStr(value.toString());
            return true;
        case Column_ExtendedSettings:
            simAction->setExtendedSettingsStr(value.toString());
            return true;
        }
    }
    return false;
}

Qt::ItemFlags mbServerSimActionsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (!mbServer::global()->isRunning())
        f |= Qt::ItemIsEditable;
    return f;
}

QModelIndex mbServerSimActionsModel::simActionIndex(mbServerSimAction *action) const
{
    if (m_project)
        return createIndex(m_project->simActionIndex(action), 0);
    return QModelIndex();
}

mbServerSimAction *mbServerSimActionsModel::simAction(const QModelIndex &index) const
{
    if (m_project)
        return m_project->simAction(index.row());
    return nullptr;
}

void mbServerSimActionsModel::setProject(mbCoreProject *project)
{
    beginResetModel();
    if (m_project)
        m_project->disconnect(this);
    m_project = static_cast<mbServerProject*>(project);
    if (m_project)
    {
        connect(m_project, &mbServerProject::simActionAdded  , this, &mbServerSimActionsModel::simActionAdd   );
        connect(m_project, &mbServerProject::simActionRemoved, this, &mbServerSimActionsModel::simActionRemove);
        connect(m_project, &mbServerProject::simActionChanged, this, &mbServerSimActionsModel::simActionChange);
    }
    endResetModel();
}

void mbServerSimActionsModel::simActionAdd(mbServerSimAction * /*action*/)
{
    beginResetModel();
    endResetModel();
}

void mbServerSimActionsModel::simActionRemove(mbServerSimAction * /*action*/)
{
    beginResetModel();
    endResetModel();
}

void mbServerSimActionsModel::simActionChange(mbServerSimAction* action)
{
    int i = m_project->simActionIndex(action);
    Q_EMIT dataChanged(createIndex(i, Column_Device), createIndex(i, ColumnCount-1));
}

