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
#include "server_rundevice.h"

#include <project/server_device.h>

mbServerRunDevice::mbServerRunDevice(mbServerPort *port)
{
    const Modbus::Defaults &d = Modbus::Defaults::instance();

    m_port = port;
    m_settings.isBroadcastEnabled = d.isBroadcastEnabled;
    memset(m_units, 0, sizeof(m_units));
    m_timestamp = 0;
}

mbServerRunDevice::~mbServerRunDevice()
{
}

#define CHECK_ENABLED_AND_DELAY                                     \
    if (!device->isEnabled())                                       \
        return Modbus::Status_BadGatewayPathUnavailable;            \
    uint delay = device->delay();                                   \
    if (delay > 0)                                                  \
    {                                                               \
        if (m_timestamp == 0)                                       \
        {                                                           \
            m_timestamp = mb::currentTimestamp();                   \
            return Modbus::Status_Processing;                       \
        }                                                           \
        if ((mb::currentTimestamp()-m_timestamp) < delay)           \
            return Modbus::Status_Processing;                       \
        m_timestamp = 0; /* Note: clear timestamp for next use */   \
    }

Modbus::StatusCode mbServerRunDevice::readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readCoils(offset, count, values);
    }
}

Modbus::StatusCode mbServerRunDevice::readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readDiscreteInputs(offset, count, values);
    }
}

Modbus::StatusCode mbServerRunDevice::readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readHoldingRegisters(offset, count, values);
    }
}

Modbus::StatusCode mbServerRunDevice::readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readInputRegisters(offset, count, values);
    }
}

Modbus::StatusCode mbServerRunDevice::writeSingleCoil(uint8_t unit, uint16_t offset, bool value)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->writeSingleCoil(offset, value);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->writeSingleCoil(offset, value);
    }
}

Modbus::StatusCode mbServerRunDevice::writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->writeSingleRegister(offset, value);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->writeSingleRegister(offset, value);
    }
}

Modbus::StatusCode mbServerRunDevice::readExceptionStatus(uint8_t unit, uint8_t *status)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readExceptionStatus(status);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnQueryData(uint8_t unit, const void *indata, uint8_t insize, void *outdata, uint8_t *outsize)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnQueryData(indata, insize, outdata, outsize);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsRestartCommunicationsOption(uint8_t unit, bool clearEventLog)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->diagnosticsRestartCommunicationsOption(clearEventLog);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsRestartCommunicationsOption(clearEventLog);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnDiagnosticRegister(uint8_t unit, uint16_t *value)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnDiagnosticRegister(value);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsChangeAsciiInputDelimiter(uint8_t unit, char delimiter)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->diagnosticsChangeAsciiInputDelimiter(delimiter);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsChangeAsciiInputDelimiter(delimiter);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsForceListenOnlyMode(uint8_t unit)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->diagnosticsForceListenOnlyMode();
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsForceListenOnlyMode();
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsClearCountersAndDiagnosticRegister(uint8_t unit)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->diagnosticsClearCountersAndDiagnosticRegister();
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsClearCountersAndDiagnosticRegister();
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnBusMessageCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnBusMessageCount(m_port, count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnBusCommunicationErrorCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnBusCommunicationErrorCount(m_port, count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnBusExceptionErrorCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnBusExceptionErrorCount(count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnServerMessageCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnServerMessageCount(count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnServerNoResponseCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnServerNoResponseCount(count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnServerNAKCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnServerNAKCount(count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnServerBusyCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnServerBusyCount(count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsReturnBusCharacterOverrunCount(uint8_t unit, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsReturnBusCharacterOverrunCount(count);
    }
}

Modbus::StatusCode mbServerRunDevice::diagnosticsClearOverrunCounterAndFlag(uint8_t unit)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->diagnosticsClearOverrunCounterAndFlag();
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->diagnosticsClearOverrunCounterAndFlag();
    }
}

Modbus::StatusCode mbServerRunDevice::getCommEventCounter(uint8_t unit, uint16_t *status, uint16_t *eventCount)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->getCommEventCounter(status, eventCount);
    }
}

Modbus::StatusCode mbServerRunDevice::getCommEventLog(uint8_t unit, uint16_t *status, uint16_t *eventCount, uint16_t *messageCount, void *eventBuff, uint8_t *eventBuffSize)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->getCommEventLog(status, eventCount, messageCount, eventBuff, eventBuffSize);
    }
}

Modbus::StatusCode mbServerRunDevice::writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->writeMultipleCoils(offset, count, values);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->writeMultipleCoils(offset, count, values);
    }
}

Modbus::StatusCode mbServerRunDevice::writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->writeMultipleRegisters(offset, count, values);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->writeMultipleRegisters(offset, count, values);
    }
}

Modbus::StatusCode mbServerRunDevice::reportServerID(uint8_t unit, void *data, uint8_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->reportServerID(data, count);
    }
}

Modbus::StatusCode mbServerRunDevice::readFileRecord(uint8_t unit, const Modbus::FileRecord *records, uint8_t recordsCount, void *outData, uint8_t *outSize)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readFileRecord(records, recordsCount, outData, outSize);
    }
}

Modbus::StatusCode mbServerRunDevice::writeFileRecord(uint8_t unit, const Modbus::FileRecord *records, uint8_t recordsCount, const void *inData, uint8_t *inSize)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->writeFileRecord(records, recordsCount, inData, inSize);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->writeFileRecord(records, recordsCount, inData, inSize);
    }
}

Modbus::StatusCode mbServerRunDevice::maskWriteRegister(uint8_t unit, uint16_t offset, uint16_t andMask, uint16_t orMask)
{
    if (isBroadcast(unit))
    {
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->maskWriteRegister(offset, andMask, orMask);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->maskWriteRegister(offset, andMask, orMask);
    }
}

Modbus::StatusCode mbServerRunDevice::readWriteMultipleRegisters(uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues)
{
    if (isBroadcast(unit))
    {
        // Note: No need to fill read buffer and return it to client in broadcast mode.
        //       So use `writeMultipleRegisters`-part only.
        Q_FOREACH (mbServerDevice *device, m_devices)
        {
            device->readWriteMultipleRegisters(readOffset, readCount, readValues, writeOffset, writeCount, writeValues);
            device->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        }
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readWriteMultipleRegisters(readOffset, readCount, readValues, writeOffset, writeCount, writeValues);
    }
}

Modbus::StatusCode mbServerRunDevice::readFIFOQueue(uint8_t unit, uint16_t fifoadr, uint16_t *values, uint16_t *count)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readFIFOQueue(fifoadr, values, count);
    }
}

Modbus::StatusCode mbServerRunDevice::readDeviceIdentification(uint8_t unit, uint8_t readDeviceId, uint8_t objectId, void *data, uint8_t *dataSize, uint8_t *numberOfObjects, uint8_t *conformityLevel, bool *moreFollows, uint8_t *nextObjectId)
{
    if (isBroadcast(unit))
    {
        this->pushEvent(MB_RECEIVE_EVENT_BROADCAST_RECEIVED);
        return Modbus::Status_Good;
    }
    else
    {
        mbServerDevice *device = this->device(unit);
        if (!device)
            return Modbus::Status_BadGatewayPathUnavailable;
        CHECK_ENABLED_AND_DELAY
        return device->readDeviceIdentification(readDeviceId, objectId, data, dataSize, numberOfObjects, conformityLevel, moreFollows, nextObjectId);
    }
}

void mbServerRunDevice::pushEvent(uint8_t event)
{
    Q_FOREACH (mbServerDevice *device, m_devices)
    {
        device->pushEvent(event);
    }
}

void mbServerRunDevice::setDevice(uint8_t unit, mbServerDevice *device)
{
    m_units[unit] = device;
    m_unitNumbers.insert(unit);
    m_devices.insert(device);
}
