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
#include <QElapsedTimer>

#include <ModbusQt.h>
#include <ModbusClientPort.h>
#include <ModbusClient.h>

#include <client.h>

#include <project/client_port.h>
#include <project/client_device.h>

#include "client_runport.h"
#include "client_rundevice.h"
#include "client_devicerunnable.h"
#include "client_runmessage.h"

mbClientPortRunnable::mbClientPortRunnable(mbClientRunPort *port, const Modbus::Settings &settings, QObject *parent)
    : QObject(parent)
{
    m_state = STATE_PAUSE;
    m_runPort = port;
    m_port = m_runPort->port();
    m_devices = m_runPort->devices();
    m_modbusPort = Modbus::createClientPort(settings);
    m_modbusPort->setBroadcastEnabled(port->isBroadcastEnabled());
    // Note: m_modbusPort can NOT be nullptr
    switch (m_modbusPort->type())
    {
    case Modbus::ASC:
    case Modbus::ASCvTCP:
    case Modbus::ASCvUDP:
        m_modbusPort->connect(&ModbusClientPort::signalTx, this, &mbClientPortRunnable::slotAsciiTx);
        m_modbusPort->connect(&ModbusClientPort::signalRx, this, &mbClientPortRunnable::slotAsciiRx);
        break;
    default:
        m_modbusPort->connect(&ModbusClientPort::signalTx, this, &mbClientPortRunnable::slotBytesTx);
        m_modbusPort->connect(&ModbusClientPort::signalRx, this, &mbClientPortRunnable::slotBytesRx);
        break;
    }
    m_modbusPort->connect(&ModbusClientPort::signalError    , this, &mbClientPortRunnable::slotError    );
    m_modbusPort->connect(&ModbusClientPort::signalStarted  , this, &mbClientPortRunnable::slotStarted  );
    m_modbusPort->connect(&ModbusClientPort::signalCompleted, this, &mbClientPortRunnable::slotCompleted);

    Q_FOREACH (mbClientRunDevice *device, m_devices)
    {
        mbClientDeviceRunnable *d = new mbClientDeviceRunnable(device, m_modbusPort);
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
    delete m_modbusPort;
}

void mbClientPortRunnable::run()
{
    QElapsedTimer timer;
    timer.start();

    switch (m_state)
    {
    default:
    case STATE_PAUSE:
        if (m_runPort->hasExternalMessage())
        {
            m_runPort->popExternalMessage(&m_currentMessage);
            m_currentMessage->prepareToSend();
        }
        else
            break;
        m_state = STATE_EXEC_EXTERNAL;
        MB_FALLTHROUGH
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

    //------------------------------------------
    // Statistics
    qint64 microsElapsed = timer.nsecsElapsed() / 1000;
    m_port->setStatCycleTime(microsElapsed);
}

void mbClientPortRunnable::close()
{
    m_modbusPort->close();
}

Modbus::StatusCode mbClientPortRunnable::execExternalMessage()
{
    Modbus::StatusCode res;
    int func = m_currentMessage->function();
    switch (func)
    {
    case 0: // mbClientRunMessageRaw
    {
        mbClientRunMessageRaw *m = static_cast<mbClientRunMessageRaw *>(m_currentMessage.data());
        res = m_modbusPort->rawRequest(m->inputBuffer(),
                                       m->inputCount(),
                                       m->outputBuffer(),
                                       m->outputMaxCount(),
                                       m->outputCountPtr());
    }
        break;
    case MBF_READ_COILS:
        res = m_modbusPort->readCoils(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_DISCRETE_INPUTS:
        res = m_modbusPort->readDiscreteInputs(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_INPUT_REGISTERS:
        res = m_modbusPort->readInputRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg());
        break;
    case MBF_READ_HOLDING_REGISTERS:
        res = m_modbusPort->readHoldingRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg());
        break;
    case MBF_WRITE_SINGLE_COIL:
        res = m_modbusPort->writeSingleCoil(m_currentMessage->unit(), m_currentMessage->offset(), *reinterpret_cast<const bool*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        res = m_modbusPort->writeSingleRegister(m_currentMessage->unit(), m_currentMessage->offset(), *reinterpret_cast<const uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_EXCEPTION_STATUS:
        res = m_modbusPort->readExceptionStatus(m_currentMessage->unit(), reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_DIAGNOSTICS:
        switch (static_cast<mbClientRunMessageDiagnostics*>(m_currentMessage.data())->subFunction())
        {
        case MBF_DIAGNOSTICS_RETURN_QUERY_DATA:
            res = m_modbusPort->diagnosticsReturnQueryData(m_currentMessage->unit(), 
                                                             m_currentMessage->innerBuffer(),
                                                             static_cast<uint8_t>(m_currentMessage->count()),
                                                             reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer()),
                                                             &m_byteCount);
            break;
        case MBF_DIAGNOSTICS_RESTART_COMMUNICATIONS_OPTION:
            res = m_modbusPort->diagnosticsRestartCommunicationsOption(m_currentMessage->unit(), reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer())[0]);
            m_byteCount = 0;
            break;
        case MBF_DIAGNOSTICS_RETURN_DIAGNOSTIC_REGISTER:
            res = m_modbusPort->diagnosticsReturnDiagnosticRegister(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_CHANGE_ASCII_INPUT_DELIMITER:
            res = m_modbusPort->diagnosticsChangeAsciiInputDelimiter(m_currentMessage->unit(), reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer())[0]);
            m_byteCount = 0;
            break;
        case MBF_DIAGNOSTICS_FORCE_LISTEN_ONLY_MODE:
            res = m_modbusPort->diagnosticsForceListenOnlyMode(m_currentMessage->unit());
            m_byteCount = 0;
            break;
        case MBF_DIAGNOSTICS_CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER:
            res = m_modbusPort->diagnosticsClearCountersAndDiagnosticRegister(m_currentMessage->unit());
            m_byteCount = 0;
            break;
        case MBF_DIAGNOSTICS_RETURN_BUS_MESSAGE_COUNT:
            res = m_modbusPort->diagnosticsReturnBusMessageCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_BUS_COMMUNICATION_ERROR_COUNT:
            res = m_modbusPort->diagnosticsReturnBusCommunicationErrorCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_BUS_EXCEPTION_ERROR_COUNT:
            res = m_modbusPort->diagnosticsReturnBusExceptionErrorCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_SERVER_MESSAGE_COUNT:
            res = m_modbusPort->diagnosticsReturnServerMessageCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_SERVER_NO_RESPONSE_COUNT:
            res = m_modbusPort->diagnosticsReturnServerNoResponseCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_SERVER_NAK_COUNT:
            res = m_modbusPort->diagnosticsReturnServerNAKCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_SERVER_BUSY_COUNT:
            res = m_modbusPort->diagnosticsReturnServerBusyCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_RETURN_BUS_CHARACTER_OVERRUN_COUNT:
            res = m_modbusPort->diagnosticsReturnBusCharacterOverrunCount(m_currentMessage->unit(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
            m_byteCount = 2;
            break;
        case MBF_DIAGNOSTICS_CLEAR_OVERRUN_COUNTER_AND_FLAG:
            res = m_modbusPort->diagnosticsClearOverrunCounterAndFlag(m_currentMessage->unit());
            m_byteCount = 0;
            break;
        default:
            res = Modbus::Status_BadIllegalFunction;
            break;
        }
        if (Modbus::StatusIsGood(res))
            static_cast<mbClientRunMessageDiagnostics*>(m_currentMessage.data())->setCount(m_byteCount);
        break;
    case MBF_GET_COMM_EVENT_COUNTER:
    {
        uint16_t status, eventCount;
        res = m_modbusPort->getCommEventCounter(m_currentMessage->unit(),
                                                &status,
                                                &eventCount);
        if (Modbus::StatusIsGood(res))
        {
            auto m = static_cast<mbClientRunMessageGetCommEventCounter*>(m_currentMessage.data());
            m->setStatus(status);
            m->setEventCount(eventCount);
        }
    }
        break;
    case MBF_GET_COMM_EVENT_LOG:
    {
        uint16_t status, eventCount, messageCount;
        uint8_t byteCount;
        res = m_modbusPort->getCommEventLog(m_currentMessage->unit(), 
                                            &status,
                                            &eventCount,
                                            &messageCount,
                                            m_currentMessage->innerBuffer(),
                                            &byteCount);
        if (Modbus::StatusIsGood(res))
        {
            auto m = static_cast<mbClientRunMessageGetCommEventLog*>(m_currentMessage.data());
            m->setStatus(status);
            m->setEventCount(eventCount);
            m->setMessageCount(messageCount);
            m->setCount(byteCount);
        }
    }
        break;
    case MBF_WRITE_MULTIPLE_COILS:
        res = m_modbusPort->writeMultipleCoils(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusPort->writeMultipleRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg());
        break;
    case MBF_REPORT_SERVER_ID:
        res = m_modbusPort->reportServerID(m_currentMessage->unit(), m_currentMessage->innerBuffer(), &m_byteCount);
        if (Modbus::StatusIsGood(res))
            static_cast<mbClientRunMessageReportServerID*>(m_currentMessage.data())->setCount(m_byteCount);
        break;
    case MBF_READ_FILE_RECORD:
    {
        auto m = static_cast<mbClientRunMessageReadFileRecord*>(m_currentMessage.data());
        res = m_modbusPort->readFileRecord(m_currentMessage->unit(), m->fileRecords(), m->count(), m->fileData(), &m_byteCount);
        if (Modbus::StatusIsGood(res))
            m->setDataSize(m_byteCount);
    }
        break;
    case MBF_WRITE_FILE_RECORD:
    {
        auto m = static_cast<mbClientRunMessageWriteFileRecord*>(m_currentMessage.data());
        res = m_modbusPort->writeFileRecord(m_currentMessage->unit(), m->fileRecords(), m->count(), m->fileData(), nullptr);
    }
        break;
    case MBF_MASK_WRITE_REGISTER:
        res = m_modbusPort->maskWriteRegister(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->innerBufferReg()[0], m_currentMessage->innerBufferReg()[1]);
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusPort->readWriteMultipleRegisters(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg(),
                                                         m_currentMessage->writeOffset(), m_currentMessage->writeCount(), m_currentMessage->innerBufferReg());
        break;
    case MBF_READ_FIFO_QUEUE:
    {
        uint16_t count;
        res = m_modbusPort->readFIFOQueue(m_currentMessage->unit(), m_currentMessage->offset(), m_currentMessage->innerBufferReg(), &count);
        if (Modbus::StatusIsGood(res))
            static_cast<mbClientRunMessageReadFIFOQueue*>(m_currentMessage.data())->setCount(count);
    }
        break;
    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
    {
        // Note: supported only READ_DEVICE_ID sub-function
        auto m = static_cast<mbClientRunMessageReadDeviceId*>(m_currentMessage.data());
        uint8_t numberOfObjects, conformityLevel, nextObjectId, dataSize;
        bool moreFollows;
        res = m_modbusPort->readDeviceIdentification(m_currentMessage->unit(), 
                                                     m->deviceId(),
                                                     m->objectId(),
                                                     m->innerBuffer(),
                                                     &dataSize,
                                                     &numberOfObjects,
                                                     &conformityLevel,
                                                     &moreFollows,
                                                     &nextObjectId);
        if (Modbus::StatusIsGood(res))
        {
            m->setDataSize(dataSize);
            m->setNumberOfObjects(numberOfObjects);
            m->setConformityLevel(conformityLevel);
            m->setMoreFollows(moreFollows);
            m->setNextObjectId(nextObjectId);
        }
    }
        break;
    default:
        return Modbus::Status_Bad;
    }
    if (Modbus::StatusIsProcessing(res))
        return res;
    if (Modbus::StatusIsBad(res))
    {
        QString text = m_modbusPort->lastErrorText();
        mbClient::LogError(m_runPort->name(), text);
    }
    m_currentMessage->setComplete(res, mb::currentTimestamp());
    return res;
}

void mbClientPortRunnable::slotBytesTx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setBytesTx(bytes);
        reinterpret_cast<mbClientDevice*>(c->context())->incStatCountTx();
        mbClient::LogTx(r->name(), Modbus::bytesToString(buff, size).data());
    }
    else
    {
        if (m_modbusPort->currentClient() == m_modbusPort)
        {
            m_currentMessage->setBytesTx(bytes);
        }
        mbClient::LogTx(name(), Modbus::bytesToString(buff, size).data());
    }
    m_port->incStatCountTx();
}

void mbClientPortRunnable::slotBytesRx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setBytesRx(bytes);
        reinterpret_cast<mbClientDevice*>(c->context())->incStatCountRx();
        mbClient::LogRx(r->name(), Modbus::bytesToString(buff, size).data());
    }
    else
    {
        if (m_modbusPort->currentClient() == m_modbusPort)
            m_currentMessage->setBytesRx(bytes);
        mbClient::LogRx(name(), Modbus::bytesToString(buff, size).data());
    }
    m_port->incStatCountRx();
}

void mbClientPortRunnable::slotAsciiTx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setAsciiTx(bytes);
        reinterpret_cast<mbClientDevice*>(c->context())->incStatCountTx();
        mbClient::LogTx(r->name(), Modbus::asciiToString(buff, size).data());
    }
    else
    {
        if (m_modbusPort->currentClient() == m_modbusPort)
            m_currentMessage->setAsciiTx(bytes);
        mbClient::LogTx(name(), Modbus::asciiToString(buff, size).data());
    }
    m_port->incStatCountTx();
}

void mbClientPortRunnable::slotAsciiRx(const Modbus::Char */*source*/, const uint8_t* buff, uint16_t size)
{
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    QByteArray bytes(reinterpret_cast<const char*>(buff), size);
    if (r)
    {
        r->currentMessage()->setAsciiRx(bytes);
        reinterpret_cast<mbClientDevice*>(c->context())->incStatCountRx();
        mbClient::LogRx(r->name(), Modbus::asciiToString(buff, size).data());
    }
    else
    {
        if (m_modbusPort->currentClient() == m_modbusPort)
            m_currentMessage->setAsciiRx(bytes);
        mbClient::LogRx(name(), Modbus::asciiToString(buff, size).data());
    }
    m_port->incStatCountRx();
}

void mbClientPortRunnable::slotError(const Modbus::Char* /*source*/, Modbus::StatusCode status, const Modbus::Char *text)
{
    Modbus::Timestamp tm = Modbus::currentTimestamp();
    QString s = QString(text);
    m_port->setStatStatus(status, tm, s);
    const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusPort->currentClient());
    mbClientDeviceRunnable *r = deviceRunnable(c);
    if (r)
    {
        auto* d = reinterpret_cast<mbClientDevice*>(c->context());
        d->setStatStatus(status, tm, s);
    }
}

void mbClientPortRunnable::slotStarted(const Modbus::Char *source)
{
    m_responseTimer.start();
}

void mbClientPortRunnable::slotCompleted(const Modbus::Char *, Modbus::StatusCode status)
{
    if (Modbus::StatusIsGood(status))
    {
        Modbus::Timestamp tm = Modbus::currentTimestamp();
        m_port->setStatStatus(status, tm);
        const ModbusClient *c = reinterpret_cast<const ModbusClient*>(m_modbusPort->currentClient());
        mbClientDeviceRunnable *r = deviceRunnable(c);
        auto responseTime_usec = m_responseTimer.nsecsElapsed() / 1000;
        if (r)
        {
            auto* d = reinterpret_cast<mbClientDevice*>(c->context());
            d->setStatStatus(status, tm);
            d->updateResponseTime(responseTime_usec);
        }
        m_port->updateResponseTime(responseTime_usec);
    }
}

