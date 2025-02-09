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

#include "server_simaction.h"

mbServerProject::mbServerProject(QObject *parent) :
    mbCoreProject(parent)
{
}

mbServerProject::~mbServerProject()
{
    qDeleteAll(m_simActions);
}

int mbServerProject::simActionInsert(mbServerSimAction *simAction, int index)
{
    if (!hasSimAction(simAction))
    {
        if ((index >= 0) && (index < m_simActions.count()))
            m_simActions.insert(index, simAction);
        else
        {
            index = m_simActions.count();
            m_simActions.append(simAction);
        }
        Q_EMIT simActionAdded(simAction);
        connect(simAction, &mbServerSimAction::changed, this, &mbServerProject::slotSimActionChanged);
        return index;
    }
    return -1;
}

void mbServerProject::simActionsInsert(const QList<mbServerSimAction *> &simActions, int index)
{
    if (index < 0 || index >= simActionCount())
    {
        Q_FOREACH (mbServerSimAction *simAction, simActions)
            simActionAdd(simAction);
    }
    else
    {
        Q_FOREACH (mbServerSimAction *simAction, simActions)
            simActionInsert(simAction, index++);
    }
}

void mbServerProject::simActionsRemove(const QList<mbServerSimAction *> &simActions)
{
    Q_FOREACH (mbServerSimAction *simAction, simActions)
        simActionRemove(simAction);
}

int mbServerProject::simActionRemove(int index)
{
    if ((index >= 0) && (index < simActionCount()))
    {
        mbServerSimAction *simAction = simActionAt(index);
        Q_EMIT simActionRemoving(simAction);
        simAction->disconnect(this);
        m_simActions.removeAt(index);
        Q_EMIT simActionRemoved(simAction);
        return index;
    }
    return -1;
}

void mbServerProject::slotSimActionChanged()
{
    mbServerSimAction *simAction = static_cast<mbServerSimAction*>(sender());
    Q_EMIT simActionChanged(simAction);
}
