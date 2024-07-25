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

#include <ModbusQt.h>
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
        m_port->connect(&ModbusClientPort::signalTx, this, &mbClientPortRunnable::slotAsciiTx);
        m_port->connect(&ModbusClientPort::signalRx, this, &mbClientPortRunnable::slotAsciiRx);
    }
    else
    {
        m_port->connect(&ModbusClientPort::signalTx, this, &mbClientPortRunnable::slotBytesTx);
        m_port->connect(&ModbusClientPort::signalRx, this, &mbClientPortRunnable::slotBytesRx);
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

void mbClientPortRunnable::slotBytesTx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setBytesTx(bytes);
        mbClient::LogTxRx(r->name(), QStringLiteral("Tx: ") + Modbus::bytesToString(buff, size).data());
    }
    else
        mbClient::LogTxRx(name(), QStringLiteral("Tx: ") + Modbus::bytesToString(buff, size).data());
}

void mbClientPortRunnable::slotBytesRx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setBytesRx(bytes);
        mbClient::LogTxRx(r->name(), QStringLiteral("Rx: ") + Modbus::bytesToString(buff, size).data());
    }
    else
        mbClient::LogTxRx(name(), QStringLiteral("Rx: ") + Modbus::bytesToString(buff, size).data());
}

void mbClientPortRunnable::slotAsciiTx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setAsciiTx(bytes);
        mbClient::LogTxRx(r->name(), QStringLiteral("Tx: ") + Modbus::asciiToString(buff, size).data());
    }
    else
        mbClient::LogTxRx(name(), QStringLiteral("Tx: ") + Modbus::asciiToString(buff, size).data());
}

void mbClientPortRunnable::slotAsciiRx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_port->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setAsciiRx(bytes);
        mbClient::LogTxRx(r->name(), QStringLiteral("Rx: ") + Modbus::asciiToString(buff, size).data());
    }
    else
        mbClient::LogTxRx(name(), QStringLiteral("Rx: ") + Modbus::asciiToString(buff, size).data());
}
