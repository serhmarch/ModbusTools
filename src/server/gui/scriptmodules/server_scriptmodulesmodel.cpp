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
#include "server_scriptmodulesmodel.h"

#include <server.h>

#include <project/server_project.h>
#include <project/server_scriptmodule.h>

mbServerScriptModulesModel::mbServerScriptModulesModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    m_project = nullptr;
    mbServer *core = mbServer::global();
    setProject(core->project());
    connect(core, &mbServer::projectChanged, this, &mbServerScriptModulesModel::setProject);
}

QVariant mbServerScriptModulesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            switch(section)
            {
            case Column_Name   : return QStringLiteral("Name");
            case Column_Comment: return QStringLiteral("Comment");
            }
            break;
        case Qt::Vertical:
            return section+1;
        }
    }
    return QVariant();
}

int mbServerScriptModulesModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (m_project)
        return m_project->scriptModuleCount();
    return 0;
}

int mbServerScriptModulesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnCount;
}

QVariant mbServerScriptModulesModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    if ( m_project &&
        (r >= 0) && (r < rowCount()) &&
        ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
    {
        int c = index.column();
        const mbServerScriptModule *scriptModule = m_project->scriptModuleAt(r);
        switch(c)
        {
        case Column_Name:
            return scriptModule->name();
        case Column_Comment:
            return scriptModule->comment();
        }
    }
    return QVariant();
}

bool mbServerScriptModulesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    if ((r >= 0) && (r < rowCount()) &&
        (role == Qt::EditRole))
    {
        int c = index.column();
        mbServerScriptModule *scriptModule = m_project->scriptModuleAt(r);
        switch(c)
        {
        case Column_Name:
            scriptModule->setName(value.toString());
            return true;
        case Column_Comment:
            scriptModule->setComment(value.toString());
            return true;
        }
    }
    return false;
}

Qt::ItemFlags mbServerScriptModulesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (!mbServer::global()->isRunning())
        f |= Qt::ItemIsEditable;
    return f;
}

QModelIndex mbServerScriptModulesModel::scriptModuleIndex(mbServerScriptModule *action) const
{
    if (m_project)
        return createIndex(m_project->scriptModuleIndex(action), 0);
    return QModelIndex();
}

mbServerScriptModule *mbServerScriptModulesModel::scriptModule(const QModelIndex &index) const
{
    if (m_project)
        return m_project->scriptModule(index.row());
    return nullptr;
}

void mbServerScriptModulesModel::setProject(mbCoreProject *project)
{
    beginResetModel();
    if (m_project)
        m_project->disconnect(this);
    m_project = static_cast<mbServerProject*>(project);
    if (m_project)
    {
        connect(m_project, &mbServerProject::scriptModuleAdded  , this, &mbServerScriptModulesModel::scriptModuleAdd   );
        connect(m_project, &mbServerProject::scriptModuleRemoved, this, &mbServerScriptModulesModel::scriptModuleRemove);
        connect(m_project, &mbServerProject::scriptModuleChanged, this, &mbServerScriptModulesModel::scriptModuleChange);
    }
    endResetModel();
}

void mbServerScriptModulesModel::scriptModuleAdd(mbServerScriptModule * /*action*/)
{
    beginResetModel();
    endResetModel();
}

void mbServerScriptModulesModel::scriptModuleRemove(mbServerScriptModule * /*action*/)
{
    beginResetModel();
    endResetModel();
}

void mbServerScriptModulesModel::scriptModuleChange(mbServerScriptModule* action)
{
    int i = m_project->scriptModuleIndex(action);
    Q_EMIT dataChanged(createIndex(i, Column_Name), createIndex(i, ColumnCount-1));
}

void mbServerScriptModulesModel::reset()
{
    beginResetModel();
    endResetModel();
}

