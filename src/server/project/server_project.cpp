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
#include "server_project.h"

#include "server_action.h"

mbServerProject::mbServerProject(QObject *parent) :
    mbCoreProject(parent)
{
}

mbServerProject::~mbServerProject()
{
    qDeleteAll(m_actions);
}

int mbServerProject::actionInsert(mbServerAction *action, int index)
{
    if (!hasAction(action))
    {
        if ((index >= 0) && (index < m_actions.count()))
            m_actions.insert(index, action);
        else
        {
            index = m_actions.count();
            m_actions.append(action);
        }
        Q_EMIT actionAdded(action);
        connect(action, &mbServerAction::changed, this, &mbServerProject::slotActionChanged);
        return index;
    }
    return -1;
}

void mbServerProject::actionsInsert(const QList<mbServerAction *> &actions, int index)
{
    if (index < 0 || index >= actionCount())
    {
        Q_FOREACH (mbServerAction *action, actions)
            actionAdd(action);
    }
    else
    {
        Q_FOREACH (mbServerAction *action, actions)
            actionInsert(action, index++);
    }
}

void mbServerProject::actionsRemove(const QList<mbServerAction *> &actions)
{
    Q_FOREACH (mbServerAction *action, actions)
        actionRemove(action);
}

int mbServerProject::actionRemove(int index)
{
    if ((index >= 0) && (index < actionCount()))
    {
        mbServerAction *action = actionAt(index);
        Q_EMIT actionRemoving(action);
        action->disconnect(this);
        m_actions.removeAt(index);
        Q_EMIT actionRemoved(action);
        return index;
    }
    return -1;
}

void mbServerProject::slotActionChanged()
{
    mbServerAction *action = static_cast<mbServerAction*>(sender());
    Q_EMIT actionChanged(action);
}
