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
#include "server_runactiontask.h"

#include <QDateTime>

#include "server_runaction.h"

#include <project/server_action.h>

mbServerRunActionTask::mbServerRunActionTask(QObject *parent) : mbCoreTask(parent)
{
}

mbServerRunActionTask::~mbServerRunActionTask()
{
    qDeleteAll(m_actions);
}

void mbServerRunActionTask::setActions(const QList<mbServerAction *> &actions)
{
    Q_FOREACH(mbServerAction *i, actions)
    {
        if (!i->device())
            continue;
        mbServerRunAction *item = nullptr;
        switch (i->actionType())
        {
        case mbServerAction::Increment:
            item = createRunActionIncrement(i->dataType(), i->settings());
            break;
        case mbServerAction::Sine:
            item = createRunActionSine(i->dataType(), i->settings());
            break;
        case mbServerAction::Random:
            item = createRunActionRandom(i->dataType(), i->settings());
            break;
        case mbServerAction::Copy:
            item = createRunActionCopy(i->settings());
            break;
        }
        if (item)
            m_actions.append(item);
    }
}

int mbServerRunActionTask::init()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH(mbServerRunAction *i, m_actions)
        i->init(time);
    return 0;
}

int mbServerRunActionTask::loop()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH(mbServerRunAction *i, m_actions)
        i->exec(time);
    return 0;
}

int mbServerRunActionTask::final()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH(mbServerRunAction *i, m_actions)
        i->final(time);
    return 0;
}
