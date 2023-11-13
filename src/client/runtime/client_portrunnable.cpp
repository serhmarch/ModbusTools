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
#include "client_portrunnable.h"

#include <QEventLoop>

#include <ModbusClientPort.h>

#include <client.h>

#include <project/client_port.h>

#include "client_rundevice.h"
#include "client_devicerunnable.h"
#include "client_runmessage.h"

mbClientPortRunnable::mbClientPortRunnable(const Modbus::Settings &settings, const QList<mbClientRunDevice*> &devices, QObject *parent)
    : QObject(parent)
{
    m_devices = devices;
    m_port = Modbus::createClientPort(settings);
    // Note: m_port can NOT be nullptr
    if (m_port->type() == Modbus::ASC)
    {
        connect(m_port->port(), &Modbus::Port::signalTx, this, &mbClientPortRunnable::slotAsciiTx);
        connect(m_port->port(), &Modbus::Port::signalRx, this, &mbClientPortRunnable::slotAsciiRx);
    }
    else
    {
        connect(m_port->port(), &Modbus::Port::signalTx, this, &mbClientPortRunnable::slotBytesTx);
        connect(m_port->port(), &Modbus::Port::signalRx, this, &mbClientPortRunnable::slotBytesRx);
    }

    Q_FOREACH (mbClientRunDevice *device, m_devices)
    {
        mbClientDeviceRunnable *d = new mbClientDeviceRunnable(device, m_port);
        m_runnables.append(d);
        m_hashRunnables.insert(d->modbusClient(), d);
    }
    setName(settings.value(mbClientPort::Strings::instance().name).toString());
}

mbClientPortRunnable::~mbClientPortRunnable()
{
    m_hashRunnables.clear();
    qDeleteAll(m_runnables);
    m_runnables.clear();
    delete m_port;
}

void mbClientPortRunnable::run()
{
    Q_FOREACH (mbClientDeviceRunnable *d, m_runnables)
        d->run();
}

void mbClientPortRunnable::close()
{
    m_port->close();
}

void mbClientPortRunnable::slotBytesTx(const QByteArray &bytes)
{
    const Modbus::Client *c = reinterpret_cast<const Modbus::Client*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    if (r)
    {
        r->currentMessage()->setBytesTx(bytes);
        mbClient::LogTxRx(r->name(), "Tx: " + Modbus::bytesToString(bytes));
    }
    else
        mbClient::LogTxRx(name(), "Tx: " + Modbus::bytesToString(bytes));
}

void mbClientPortRunnable::slotBytesRx(const QByteArray &bytes)
{
    const Modbus::Client *c = reinterpret_cast<const Modbus::Client*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    if (r)
    {
        r->currentMessage()->setBytesRx(bytes);
        mbClient::LogTxRx(r->name(), "Rx: " + Modbus::bytesToString(bytes));
    }
    else
        mbClient::LogTxRx(name(), "Rx: " + Modbus::bytesToString(bytes));
}

void mbClientPortRunnable::slotAsciiTx(const QByteArray &bytes)
{
    const Modbus::Client *c = reinterpret_cast<const Modbus::Client*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    if (r)
    {
        r->currentMessage()->setAsciiTx(bytes);
        mbClient::LogTxRx(r->name(), "Tx: " + Modbus::asciiToString(bytes));
    }
    else
        mbClient::LogTxRx(name(), "Tx: " + Modbus::asciiToString(bytes));
}

void mbClientPortRunnable::slotAsciiRx(const QByteArray &bytes)
{
    const Modbus::Client *c = reinterpret_cast<const Modbus::Client*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    if (r)
    {
        r->currentMessage()->setAsciiRx(bytes);
        mbClient::LogTxRx(r->name(), "Rx: " + Modbus::asciiToString(bytes));
    }
    else
        mbClient::LogTxRx(name(), "Rx: " + Modbus::asciiToString(bytes));
}
