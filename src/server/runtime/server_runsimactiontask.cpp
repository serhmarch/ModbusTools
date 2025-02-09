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
#include "server_runsimactiontask.h"

#include <QDateTime>

#include "server_runsimaction.h"

#include <project/server_simaction.h>

mbServerRunSimActionTask::mbServerRunSimActionTask(QObject *parent) : mbCoreTask(parent)
{
}

mbServerRunSimActionTask::~mbServerRunSimActionTask()
{
    qDeleteAll(m_actions);
}

void mbServerRunSimActionTask::setActions(const QList<mbServerSimAction *> &actions)
{
    Q_FOREACH(mbServerSimAction *i, actions)
    {
        if (!i->device())
            continue;
        mbServerRunSimAction *item = nullptr;
        MBSETTINGS s = i->settings();
        s[mbServerSimAction::Strings::instance().registerOrder] = i->getRegisterOrder();
        switch (i->actionType())
        {
        case mbServerSimAction::Increment:
            item = createRunActionIncrement(i->dataType(), s);
            break;
        case mbServerSimAction::Sine:
            item = createRunActionSine(i->dataType(), s);
            break;
        case mbServerSimAction::Random:
            item = createRunActionRandom(i->dataType(), s);
            break;
        case mbServerSimAction::Copy:
            item = createRunActionCopy(s);
            break;
        }
        if (item)
            m_actions.append(item);
    }
}

int mbServerRunSimActionTask::init()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH(mbServerRunSimAction *i, m_actions)
        i->init(time);
    return 0;
}

int mbServerRunSimActionTask::loop()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH(mbServerRunSimAction *i, m_actions)
        i->exec(time);
    return 0;
}

int mbServerRunSimActionTask::final()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH(mbServerRunSimAction *i, m_actions)
        i->final(time);
    return 0;
}
