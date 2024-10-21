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
#include <ModbusTcpServer.h>

#include <server.h>

#include <project/server_port.h>

#include "server_rundevice.h"

mbServerPortRunnable::mbServerPortRunnable(mbServerPort *serverPort, const Modbus::Settings &settings, mbServerRunDevice *device, QObject *parent)
    : QObject(parent)
{
    m_serverPort = serverPort;
    m_stat = m_serverPort->statistic();
    m_device = device;
    m_modbusPort = Modbus::createServerPort(device, settings);
    // Note: m_modbusPort can NOT be nullptr
    switch (m_modbusPort->type())
    {
    case Modbus::RTU:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotBytesTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotBytesRx);
        break;
    case Modbus::ASC:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotAsciiTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotAsciiRx);
        break;
    default:
        m_modbusPort->connect(&ModbusServerPort::signalTx, this, &mbServerPortRunnable::slotBytesTx);
        m_modbusPort->connect(&ModbusServerPort::signalRx, this, &mbServerPortRunnable::slotBytesRx);
        m_modbusPort->connect(&ModbusTcpServer::signalNewConnection, this, &mbServerPortRunnable::slotNewConnection);
        m_modbusPort->connect(&ModbusTcpServer::signalCloseConnection, this, &mbServerPortRunnable::slotCloseConnection);
        break;
    }

    m_modbusPort->connect(&ModbusServerPort::signalError, this, &mbServerPortRunnable::slotError);

    QString name = settings.value(mbServerPort::Strings::instance().name).toString();
    m_modbusPort->setObjectName(name.toUtf8().constData());
    setName(name);
}

mbServerPortRunnable::~mbServerPortRunnable()
{
    delete m_modbusPort;
}

void mbServerPortRunnable::setName(const QString &name)
{
    //m_modbusPort->setObjectName(name.toLatin1().constData());
    setObjectName(name);
}

void mbServerPortRunnable::run()
{
    m_modbusPort->process();
}

void mbServerPortRunnable::close()
{
    m_modbusPort->close();
    while (!m_modbusPort->isStateClosed())
    {
        m_modbusPort->process();
        QThread::yieldCurrentThread();
    }
}

void mbServerPortRunnable::slotBytesTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogTxRx(source, QStringLiteral("Tx: ") + Modbus::bytesToString(buff, size).data());
    m_stat.countTx++;
    m_serverPort->setStatCountTx(m_stat.countTx);
}

void mbServerPortRunnable::slotBytesRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogTxRx(source, QStringLiteral("Rx: ") + Modbus::bytesToString(buff, size).data());
    m_stat.countRx++;
    m_serverPort->setStatCountRx(m_stat.countRx);
}

void mbServerPortRunnable::slotAsciiTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogTxRx(source, QStringLiteral("Tx: ") + Modbus::bytesToString(buff, size).data());
    m_stat.countTx++;
    m_serverPort->setStatCountTx(m_stat.countTx);
}

void mbServerPortRunnable::slotAsciiRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size)
{
    mbServer::LogTxRx(source, QStringLiteral("Rx: ") + Modbus::bytesToString(buff, size).data());
    m_stat.countRx++;
    m_serverPort->setStatCountRx(m_stat.countRx);
}

void mbServerPortRunnable::slotError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    if (status != Modbus::Status_BadSerialReadTimeout)
        mbServer::LogError(source, QString("Error(0x%1): %2").arg(QString::number(status, 16), text));
}

void mbServerPortRunnable::slotNewConnection(const Modbus::Char *source)
{
    mbServer::LogInfo(name(), QStringLiteral("New Connection: ") + source);
}

void mbServerPortRunnable::slotCloseConnection(const Modbus::Char *source)
{
    mbServer::LogInfo(name(), QStringLiteral("Close Connection: ") + source);
}

