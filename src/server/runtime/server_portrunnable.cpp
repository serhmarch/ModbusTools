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
#include "server_portrunnable.h"

#include <ModbusServerPort.h>

#include <server.h>

#include <project/server_port.h>

#include "server_rundevice.h"

mbServerPortRunnable::mbServerPortRunnable(const Modbus::Settings &settings, mbServerRunDevice *device, QObject *parent)
    : QObject(parent)
{
    m_device = device;
    m_port = Modbus::createServerPort(settings, device);
    // Note: m_port can NOT be nullptr
    if (m_port->type() == Modbus::ASC)
    {
        connect(m_port, &Modbus::ServerPort::signalTx, this, &mbServerPortRunnable::slotAsciiTx);
        connect(m_port, &Modbus::ServerPort::signalRx, this, &mbServerPortRunnable::slotAsciiRx);
    }
    else
    {
        connect(m_port, &Modbus::ServerPort::signalTx, this, &mbServerPortRunnable::slotBytesTx);
        connect(m_port, &Modbus::ServerPort::signalRx, this, &mbServerPortRunnable::slotBytesRx);
    }

    connect(m_port, &Modbus::ServerPort::signalError  , this, &mbServerPortRunnable::slotError  );
    connect(m_port, &Modbus::ServerPort::signalMessage, this, &mbServerPortRunnable::slotMessage);

    setName(settings.value(mbServerPort::Strings::instance().name).toString());
}

mbServerPortRunnable::~mbServerPortRunnable()
{
    delete m_port;
}

void mbServerPortRunnable::setName(const QString &name)
{
    m_port->setName(name);
    setObjectName(name);
}

void mbServerPortRunnable::run()
{
    m_port->process();
}

void mbServerPortRunnable::close()
{
    m_port->close();
}

void mbServerPortRunnable::slotBytesTx(const QString &source, const QByteArray &bytes)
{
    mbServer::LogTxRx(source, "Tx: " + Modbus::bytesToString(bytes));
}

void mbServerPortRunnable::slotBytesRx(const QString &source, const QByteArray &bytes)
{
    mbServer::LogTxRx(source, "Rx: " + Modbus::bytesToString(bytes));
}

void mbServerPortRunnable::slotAsciiTx(const QString &source, const QByteArray &bytes)
{
    mbServer::LogTxRx(source, "Tx: " + Modbus::asciiToString(bytes));
}

void mbServerPortRunnable::slotAsciiRx(const QString &source, const QByteArray &bytes)
{
    mbServer::LogTxRx(source, "Rx: " + Modbus::asciiToString(bytes));
}

void mbServerPortRunnable::slotError(const QString &source, int code, const QString &message)
{
    mbServer::LogError(source, QString("Error(0x%1): %2").arg(QString::number(code, 16), message));
}

void mbServerPortRunnable::slotMessage(const QString &source, const QString &message)
{
    mbServer::LogInfo(source, message);
}
