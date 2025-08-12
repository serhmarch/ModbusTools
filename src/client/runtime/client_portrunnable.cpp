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

#include "client_runport.h"
#include "client_rundevice.h"
#include "client_devicerunnable.h"
#include "client_runmessage.h"

mbClientPortRunnable::mbClientPortRunnable(mbClientRunPort *port, const Modbus::Settings &settings, QObject *parent)
    : QObject(parent)
{
    m_state = STATE_PAUSE;
    m_port = port;
    m_stat = m_port->statistic();
    m_devices = m_port->devices();
    m_modbusClientPort = Modbus::createClientPort(settings);
    m_modbusClientPort->setBroadcastEnabled(port->isBroadcastEnabled());
    // Note: m_modbusClientPort can NOT be nullptr
    switch (m_modbusClientPort->type())
    {
    case Modbus::ASC:
    case Modbus::ASCvTCP:
    case Modbus::ASCvUDP:
        m_modbusClientPort->connect(&ModbusClientPort::signalTx, this, &mbClientPortRunnable::slotAsciiTx);
        m_modbusClientPort->connect(&ModbusClientPort::signalRx, this, &mbClientPortRunnable::slotAsciiRx);
        break;
    default:
        m_modbusClientPort->connect(&ModbusClientPort::signalTx, this, &mbClientPortRunnable::slotBytesTx);
        m_modbusClientPort->connect(&ModbusClientPort::signalRx, this, &mbClientPortRunnable::slotBytesRx);
        break;
    }

    Q_FOREACH (mbClientRunDevice *device, m_devices)
    {
        mbClientDeviceRunnable *d = new mbClientDeviceRunnable(device, m_modbusClientPort);
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
    delete m_modbusClientPort;
}

void mbClientPortRunnable::run()
{
    switch (m_state)
    {
    default:
    case STATE_PAUSE:
        if (m_port->hasExternalMessage())
        {
            m_port->popExternalMessage(&m_currentMessage);
            m_currentMessage->prepareToSend();
        }
        else
            break;
        m_state = STATE_EXEC_EXTERNAL;
        // no need break
    case STATE_EXEC_EXTERNAL:
    {
        Modbus::StatusCode r = execExternalMessage();
        if (Modbus::StatusIsProcessing(r))
            break;
        m_currentMessage = nullptr;
        m_state = STATE_PAUSE;
    }
        break;
    }
    Q_FOREACH (mbClientDeviceRunnable *d, m_runnables)
        d->run();
}

void mbClientPortRunnable::close()
{
    m_modbusClientPort->close();
}

Modbus::StatusCode mbClientPortRunnable::execExternalMessage()
{
    Modbus::StatusCode res;
    int func = m_currentMessage->function();
    switch (func)
    {
    case MBF_READ_COILS:
        res = m_modbusClientPort->readCoils(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_DISCRETE_INPUTS:
        res = m_modbusClientPort->readDiscreteInputs(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_INPUT_REGISTERS:
        res = m_modbusClientPort->readInputRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_HOLDING_REGISTERS:
        res = m_modbusClientPort->readHoldingRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_EXCEPTION_STATUS:
        res = m_modbusClientPort->readExceptionStatus(m_currentMessage->unit(), reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_SINGLE_COIL:
        res = m_modbusClientPort->writeSingleCoil(m_currentMessage->unit(), m_currentMessage->offset(), *reinterpret_cast<const bool*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        res = m_modbusClientPort->writeSingleRegister(m_currentMessage->unit(), m_currentMessage->offset(), *reinterpret_cast<const uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_MULTIPLE_COILS:
        res = m_modbusClientPort->writeMultipleCoils(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusClientPort->writeMultipleRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg());
        break;
    case MBF_REPORT_SERVER_ID:
        res = m_modbusClientPort->reportServerID(m_currentMessage->unit(), &m_byteCount, reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer()));
        if (Modbus::StatusIsGood(res))
            static_cast<mbClientRunMessageReportServerID*>(m_currentMessage.data())->setCount(m_byteCount);
        break;
    case MBF_MASK_WRITE_REGISTER:
        res = m_modbusClientPort->maskWriteRegister(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->innerBufferReg()[0], m_currentMessage->innerBufferReg()[1]);
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusClientPort->readWriteMultipleRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg(),
                                                         m_currentMessage->writeOffset(), m_currentMessage->writeCount(), m_currentMessage->innerBufferReg());
        break;
    default:
        return Modbus::Status_Bad;
    }
    if (Modbus::StatusIsProcessing(res))
        return res;
    if (Modbus::StatusIsBad(res))
    {
        QString text = m_modbusClientPort->lastErrorText();
        mbClient::LogError(m_port->name(), text);
    }
    m_currentMessage->setComplete(res, mb::currentTimestamp());
    return res;
}

void mbClientPortRunnable::slotBytesTx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusClientPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setBytesTx(bytes);
        mbClient::LogTx(r->name(), Modbus::bytesToString(buff, size).data());
    }
    else
    {
        if (m_modbusClientPort->currentClient() == m_modbusClientPort)
            m_currentMessage->setBytesTx(bytes);
        mbClient::LogTx(name(), Modbus::bytesToString(buff, size).data());
    }
    m_stat.countTx++;
    m_port->setStatCountTx(m_stat.countTx);
}

void mbClientPortRunnable::slotBytesRx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusClientPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setBytesRx(bytes);
        mbClient::LogRx(r->name(), Modbus::bytesToString(buff, size).data());
    }
    else
    {
        if (m_modbusClientPort->currentClient() == m_modbusClientPort)
            m_currentMessage->setBytesRx(bytes);
        mbClient::LogRx(name(), Modbus::bytesToString(buff, size).data());
    }
    m_stat.countRx++;
    m_port->setStatCountRx(m_stat.countRx);
}

void mbClientPortRunnable::slotAsciiTx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusClientPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setAsciiTx(bytes);
        mbClient::LogTx(r->name(), Modbus::asciiToString(buff, size).data());
    }
    else
    {
        if (m_modbusClientPort->currentClient() == m_modbusClientPort)
            m_currentMessage->setAsciiTx(bytes);
        mbClient::LogTx(name(), Modbus::asciiToString(buff, size).data());
    }
    m_stat.countTx++;
    m_port->setStatCountTx(m_stat.countTx);
}

void mbClientPortRunnable::slotAsciiRx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusClientPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setAsciiRx(bytes);
        mbClient::LogRx(r->name(), Modbus::asciiToString(buff, size).data());
    }
    else
    {
        if (m_modbusClientPort->currentClient() == m_modbusClientPort)
            m_currentMessage->setAsciiRx(bytes);
        mbClient::LogRx(name(), Modbus::asciiToString(buff, size).data());
    }
    m_stat.countRx++;
    m_port->setStatCountRx(m_stat.countRx);
}
