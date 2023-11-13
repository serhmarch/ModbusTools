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
#include "core_runtime.h"

#include <QCoreApplication>

#include <core.h>

#include "core_runtaskthread.h"

mbCoreRuntime::mbCoreRuntime(QObject *parent)
    : QObject{parent}
{
    m_project = nullptr;
}

bool mbCoreRuntime::isRunning()
{
    return m_project;
}

void mbCoreRuntime::start()
{
    if (m_project)
        return;
    m_project = mbCore::globalCore()->projectCore();
    if (!m_project)
        return;
    createComponents();
    startComponents();
}

void mbCoreRuntime::stop()
{
    if (!m_project)
        return;
    beginStopComponents();
    do
    {
        if (tryStopComponents())
            break;
        QCoreApplication::processEvents();
        QThread::usleep(1);
    }
    while (1);
    clearComponents();
    m_project = nullptr;
}

void mbCoreRuntime::createComponents()
{
    
}

void mbCoreRuntime::startComponents()
{
    Q_FOREACH (mbCoreRunTaskThread *taskThread, m_taskThreads)
        taskThread->start();
}

void mbCoreRuntime::beginStopComponents()
{
    Q_FOREACH (mbCoreRunTaskThread *taskThread, m_taskThreads)
        taskThread->stop();
}

bool mbCoreRuntime::tryStopComponents()
{
    Q_FOREACH (mbCoreRunTaskThread *taskThread, m_taskThreads)
    {
        if (taskThread->isRunning())
            return false;
    }
    return true;
}

void mbCoreRuntime::clearComponents()
{
    qDeleteAll(m_taskThreads);
    m_taskThreads.clear();
}
