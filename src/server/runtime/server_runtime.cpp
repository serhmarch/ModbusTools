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
#include "server_runtime.h"

#include <QCoreApplication>

#include <server.h>

#include <project/server_project.h>
#include <project/server_port.h>
#include <project/server_deviceref.h>

#include <runtime/core_runtaskthread.h>

#include <gui/server_ui.h>
#include <gui/script/server_scriptmanager.h>
#include <gui/script/server_devicescripteditor.h>

#include "server_runthread.h"
#include "server_rundevice.h"
#include "server_runactiontask.h"

#include "server_runscriptthread.h"

mbServerRuntime::mbServerRuntime(QObject *parent)
    : mbCoreRuntime{parent}
{
}

void mbServerRuntime::createComponents()
{
    mbCoreRuntime::createComponents();

    mbServerRunActionTask *actionTask = new mbServerRunActionTask;
    actionTask->setActions(project()->actions());
    mbCoreRunTaskThread *actionThread = new mbCoreRunTaskThread(actionTask);
    m_taskThreads.append(actionThread);

    Q_FOREACH (mbServerPort *port, project()->ports())
        createRunThread(port);

    Q_FOREACH (mbServerDevice *dev, project()->devices())
        createScriptThread(dev);
}

void mbServerRuntime::startComponents()
{
    mbCoreRuntime::startComponents();
    Q_FOREACH (mbServerRunThread *t, m_threads)
        t->start();

    Q_FOREACH (mbServerRunScriptThread *t, m_scriptThreads)
        t->start();
}

void mbServerRuntime::beginStopComponents()
{
    mbCoreRuntime::beginStopComponents();
    Q_FOREACH (mbServerRunThread *t, m_threads)
        t->stop();

    Q_FOREACH (mbServerRunScriptThread *t, m_scriptThreads)
        t->stop();
}

bool mbServerRuntime::tryStopComponents()
{
    if (!mbCoreRuntime::tryStopComponents())
        return false;

    Q_FOREACH (mbServerRunThread *t, m_threads)
    {
        if (t->isRunning())
            return false;
    }

    Q_FOREACH (mbServerRunScriptThread *t, m_scriptThreads)
    {
        if (t->isRunning())
            return false;
    }
    return true;
}

void mbServerRuntime::clearComponents()
{
    mbCoreRuntime::clearComponents();

    qDeleteAll(m_threads);
    m_threads.clear();

    qDeleteAll(m_scriptThreads);
    m_scriptThreads.clear();
}

mbServerRunThread *mbServerRuntime::createRunThread(mbServerPort *port)
{
    mbServerRunDevice *device = new mbServerRunDevice();
    for (int unit = 0; unit <= 255; unit++)
    {
        mbServerDeviceRef *ref = port->deviceByUnit(unit);
        if (ref)
            device->setDevice(static_cast<quint8>(unit), ref->device());
    }
    mbServerRunThread *t = new mbServerRunThread(port, device);
    m_threads.insert(port, t);
    return t;
}

mbServerRunScriptThread *mbServerRuntime::createScriptThread(mbServerDevice *device)
{
    bool useTemporary = m_project->isModified();
    MBSETTINGS scripts = device->scriptSources();
    mbServerUi *ui = mbServer::global()->ui();
    if (ui)
    {
        const mbServerDevice::Strings &s = mbServerDevice::Strings::instance();
        mbServerScriptManager *sm = ui->scriptManager();
        if (mbServerDeviceScriptEditor *se = sm->deviceScriptEditor(device, mbServerDevice::Script_Init))
        {
            QString text = se->toPlainText();
            if (text.count())
                scripts[s.scriptInit] = text;
        }
        if (mbServerDeviceScriptEditor *se = sm->deviceScriptEditor(device, mbServerDevice::Script_Loop))
        {
            QString text = se->toPlainText();
            if (text.count())
                scripts[s.scriptLoop] = text;
        }
        if (mbServerDeviceScriptEditor *se = sm->deviceScriptEditor(device, mbServerDevice::Script_Final))
        {
            QString text = se->toPlainText();
            if (text.count())
                scripts[s.scriptFinal] = text;
        }
    }
    if (scripts.count())
    {
        mbServerRunScriptThread *t = new mbServerRunScriptThread(device, scripts, useTemporary);
        m_scriptThreads.insert(device, t);
        return t;
    }
    return nullptr;
}

