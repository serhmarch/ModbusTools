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
#include "server_runthread.h"

#include <QEventLoop>

#include <ModbusServerPort.h>

#include <server.h>

#include <project/server_port.h>

#include "server_portrunnable.h"
#include "server_rundevice.h"

mbServerRunThread::mbServerRunThread(mbServerPort *serverPort, mbServerRunDevice *device, QObject *parent)
    : QThread(parent)
{
    m_serverPort = serverPort;
    m_ctrlRun = true;
    m_device = device;
    m_settings = serverPort->settings();
}

mbServerRunThread::~mbServerRunThread()
{
    delete m_device;
}

void mbServerRunThread::run()
{
    QEventLoop loop;
    mbServerPortRunnable port(m_serverPort, m_settings, m_device);
    m_ctrlRun = true;
    mbServer::LogInfo(port.name(), QStringLiteral("Start"));
    while (m_ctrlRun)
    {
        loop.processEvents();
        port.run();
        Modbus::msleep(1);
    }
    port.close();
    mbServer::LogInfo(port.name(), QStringLiteral("Stop"));
}
