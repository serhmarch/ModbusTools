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
#include "client_runthread.h"

#include <QEventLoop>

#include <ModbusClientPort.h>

#include <client.h>

#include <project/client_port.h>

#include "client_rundevice.h"
#include "client_portrunnable.h"

mbClientRunThread::mbClientRunThread(mbClientPort *clientPort, QObject *parent)
    : QThread(parent)
{
    m_ctrlRun = true;
    m_clientPort = clientPort;
    m_settings = clientPort->settings();
    moveToThread(this);
}

mbClientRunThread::~mbClientRunThread()
{
}

void mbClientRunThread::pushDevices(const QList<mbClientRunDevice *> &devices)
{
    m_devices.append(devices);
}

void mbClientRunThread::run()
{
    QEventLoop loop;
    mbClientPortRunnable port(m_clientPort, m_settings, m_devices, this);
    m_ctrlRun = true;
    mbClient::LogInfo(port.name(), QStringLiteral("Start polling"));
    while (m_ctrlRun)
    {
        loop.processEvents();
        port.run();
        Modbus::msleep(1);
    }
    port.close();
    mbClient::LogInfo(port.name(), QStringLiteral("Finish polling"));
}
