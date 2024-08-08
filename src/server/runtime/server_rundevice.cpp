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

mbServerRunDevice::mbServerRunDevice()
{
    memset(m_units, 0, sizeof(m_units));
    m_timestamp = 0;
}

mbServerRunDevice::~mbServerRunDevice()
{
}

#define CHECK_DELAY                                                 \
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
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->readCoils(offset, count, values);
}

Modbus::StatusCode mbServerRunDevice::readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->readDiscreteInputs(offset, count, values);
}

Modbus::StatusCode mbServerRunDevice::readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->readHoldingRegisters(offset, count, values);
}

Modbus::StatusCode mbServerRunDevice::readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->readInputRegisters(offset, count, values);
}

Modbus::StatusCode mbServerRunDevice::writeSingleCoil(uint8_t unit, uint16_t offset, bool value)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->writeSingleCoil(offset, value);
}

Modbus::StatusCode mbServerRunDevice::writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->writeSingleRegister(offset, value);
}

Modbus::StatusCode mbServerRunDevice::readExceptionStatus(uint8_t unit, uint8_t *status)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->readExceptionStatus(status);
}

Modbus::StatusCode mbServerRunDevice::writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
    return device->writeMultipleCoils(offset, count, values);
}

Modbus::StatusCode mbServerRunDevice::writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
        return device->writeMultipleRegisters(offset, count, values);
}

Modbus::StatusCode mbServerRunDevice::maskWriteRegister(uint8_t unit, uint16_t offset, uint16_t andMask, uint16_t orMask)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
        return device->maskWriteRegister(offset, andMask, orMask);
}

Modbus::StatusCode mbServerRunDevice::readWriteMultipleRegisters(uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues)
{
    mbServerDevice *device = this->device(unit);
    if (!device)
        return Modbus::Status_BadGatewayPathUnavailable;
    CHECK_DELAY
        return device->readWriteMultipleRegisters(readOffset, readCount, readValues, writeOffset, writeCount, writeValues);
}
