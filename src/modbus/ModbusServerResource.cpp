/*
    Modbus

    Created: 2024
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
#include "ModbusServerResource.h"
#include "ModbusServerResource_p.h"

ModbusServerResource::ModbusServerResource(ModbusPort *port, ModbusInterface *device) :
    ModbusServerPort(new ModbusServerResourcePrivate(port, device))
{
    port->setServerMode(true);
}

ModbusPort *ModbusServerResource::port() const
{
    return d_ModbusServerResource(d_ptr)->port;
}

ProtocolType ModbusServerResource::type() const
{
    return d_ModbusServerResource(d_ptr)->port->type();
}

StatusCode ModbusServerResource::open()
{
    d_ModbusServerResource(d_ptr)->cmdClose = false;
    return Status_Good;
}

StatusCode ModbusServerResource::close()
{
    d_ModbusServerResource(d_ptr)->cmdClose = true;
    return Status_Good;
}

bool ModbusServerResource::isOpen() const
{
    return d_ModbusServerResource(d_ptr)->port->isOpen();
}

StatusCode ModbusServerResource::process()
{
    ModbusServerResourcePrivate *d = d_ModbusServerResource(d_ptr);
    const int szBuff = 500;

    StatusCode r = Status_Good;
    uint8_t buff[szBuff], func;
    uint16_t outBytes, outCount = 0;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_CLOSED:
            if (d->cmdClose)
                break;
            d->state = STATE_WAIT_FOR_OPEN;
            // no need break
        case STATE_WAIT_FOR_OPEN:
            if (d->cmdClose)
            {
                d->state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            r = d->port->open();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r))  // an error occured
            {
                d->setPortError(r);
                signalError(d->getName(), r, d->lastPortErrorText());
                d->state = STATE_CLOSED;
                return r;
            }
            d->state = STATE_OPENED;
            fRepeatAgain = true;
            break;
        case STATE_WAIT_FOR_CLOSE:
            r = d->port->close();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r))
            {
                d->setPortError(r);
                signalError(d->getName(), r, d->lastPortErrorText());
            }
            d->state = STATE_CLOSED;
            break;
        case STATE_OPENED:
            d->state = STATE_BEGIN_READ;
            // no need break
        case STATE_BEGIN_READ:
            if (d->cmdClose)
            {
                d->state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            r = d->port->read();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r)) // an error occured
            {
                d->setPortError(r);
                signalError(d->getName(), r, d->lastPortErrorText());
                return r;
            }
            signalRx(d->getName(), d->port->readBufferData(), d->port->readBufferSize());
            if (!d->port->isOpen())
            {
                d->state = STATE_CLOSED;
                return Status_Uncertain;
            }
            d->state = STATE_READ;
            // no need break
        case STATE_READ:
            // verify unit id
            r = d->port->readBuffer(d->unit, d->func, buff, szBuff, &outBytes);
            if (StatusIsBad(r))
                d->setPortError(r);
            if (StatusIsGood(r))
                r = processInputData(buff, outBytes);
            if (StatusIsBad(r)) // data error
            {
                signalError(d->getName(), r, d->getLastErrorText());
                if (StatusIsStandardError(r)) // return standard error to device
                {
                    d->state = STATE_WRITE;
                    fRepeatAgain = true;
                    break;
                }
                else
                {
                    d->state = STATE_BEGIN_READ;
                    return r;
                }
            }
            d->state = STATE_PROCESS_DEVICE;
            // no need break
        case STATE_PROCESS_DEVICE:
            r = processDevice();
            if (StatusIsProcessing(r))
                return r;
            if (r == Status_BadGatewayPathUnavailable)
            {
                d->state = STATE_BEGIN_READ;
                return r;
            }
            d->state = STATE_WRITE;
            // no need break
        case STATE_WRITE:
            func = d->func;
            if (StatusIsBad(r))
            {
                signalError(d->getName(), r, d->lastErrorText());
                func |= MBF_EXCEPTION;
                if (StatusIsStandardError(r))
                    buff[0] = static_cast<uint8_t>(r & 0xFF);
                else
                    buff[0] = static_cast<uint8_t>(Status_BadServerDeviceFailure & 0xFF);
                outCount = 1;
            }
            else
                processOutputData(buff, outCount);
            d->port->writeBuffer(d->unit, func, buff, outCount);
            d->state = STATE_BEGIN_WRITE;
            // no need break
        case STATE_BEGIN_WRITE:
            r = d->port->write();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r))
            {
                d->setPortError(r);
                signalError(d->getName(), r, d->lastPortErrorText());
            }
            else
                signalTx(d->getName(), d->port->writeBufferData(), d->port->writeBufferSize());
            d->state = STATE_BEGIN_READ;
            return r;
        default:
            if (d->cmdClose && isOpen())
            {
                d->state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
            }
            else if (isOpen())
            {
                d->state = STATE_OPENED;
                fRepeatAgain = true;
            }
            else
                d->state = STATE_CLOSED;
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode ModbusServerResource::processInputData(const uint8_t *buff, uint16_t sz)
{
    ModbusServerResourcePrivate *d = d_ModbusServerResource(d_ptr);
    switch (d->func)
    {
    case MBF_READ_COILS:            // Read Coil Status
    case MBF_READ_DISCRETE_INPUTS:  // Read Input Status
        if (sz != 4) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        d->offset = buff[1] | (buff[0] << 8);
        d->count  = buff[3] | (buff[2] << 8);
        if (d->count > MB_MAX_DISCRETS) // prevent valueBuff overflow
            return d->setError(Status_BadIllegalDataValue, StringLiteral("Not correct data value"));
        break;
    case MBF_READ_HOLDING_REGISTERS:    // Read holding registers
    case MBF_READ_INPUT_REGISTERS:      // Read input registers
        if (sz != 4) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        d->offset = buff[1] | (buff[0]<<8);
        d->count = buff[3] | (buff[2]<<8);
        if (d->count > MB_MAX_REGISTERS) // prevent valueBuff overflow
            return d->setError(Status_BadIllegalDataValue, StringLiteral("Not correct data value"));
        break;
    case MBF_WRITE_SINGLE_COIL: // Write single coil
        if (sz != 4) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        if (!(buff[2] == 0x00 || buff[2] == 0xFF) || (buff[3] != 0))  // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data value"));
        d->offset = buff[1] | (buff[0]<<8);
        d->valueBuff[0] = buff[2];
        break;
    case MBF_WRITE_SINGLE_REGISTER: // Write single register
        if (sz != 4) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        d->offset = buff[1] | (buff[0]<<8);
        d->valueBuff[0] = buff[3];
        d->valueBuff[1] = buff[2];
        break;
    case MBF_READ_EXCEPTION_STATUS: // Read exception status
        if (sz > 0) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        break;
    case MBF_WRITE_MULTIPLE_COILS: // Write multiple coils
        if (sz < 5) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        if (sz != buff[4]+5) // don't match readed bytes and number of data bytes to follow
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        d->offset = buff[1] | (buff[0]<<8);
        d->count = buff[3] | (buff[2]<<8);
        if ((d->count+7)/8 != buff[4]) // don't match count bites and bytes
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        if (d->count > MB_MAX_DISCRETS) // prevent valueBuff overflow
            return d->setError(Status_BadIllegalDataValue, StringLiteral("Not correct data value"));
        memcpy(d->valueBuff, &buff[5], (d->count+7)/8);
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS: // Write multiple registers
        if (sz < 5) // not correct request from client - don't respond
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        if (sz != buff[4]+5) // don't match readed bytes and number of data bytes to follow
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        d->offset = buff[1] | (buff[0]<<8);
        d->count = buff[3] | (buff[2]<<8);
        if (d->count*2 != buff[4]) // don't match count values and bytes
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct data size"));
        if (d->count > MB_MAX_REGISTERS) // prevent valueBuff overflow
            return d->setError(Status_BadIllegalDataValue, StringLiteral("Not correct data value"));
        for (uint16_t i = 0; i < d->count; i++)
        {
            d->valueBuff[i*2]   = buff[6+i*2];
            d->valueBuff[i*2+1] = buff[5+i*2];
        }
        break;
    default:
        return d->setError(Status_BadIllegalFunction, StringLiteral("Unsupported function"));
    }
    return Status_Good;
}

StatusCode ModbusServerResource::processDevice()
{
    ModbusServerResourcePrivate *d = d_ModbusServerResource(d_ptr);
    switch (d->func)
    {
    case MBF_READ_COILS: // Read Coil Status
        return d->device->readCoils(d->unit, d->offset, d->count, d->valueBuff);
    case MBF_READ_DISCRETE_INPUTS: // Read Input Status
        return d->device->readDiscreteInputs(d->unit, d->offset, d->count, d->valueBuff);
    case MBF_READ_HOLDING_REGISTERS: // Read holding registers
        return d->device->readHoldingRegisters(d->unit, d->offset, d->count, reinterpret_cast<uint16_t*>(d->valueBuff));
    case MBF_READ_INPUT_REGISTERS: // Read input registers
        return d->device->readInputRegisters(d->unit, d->offset, d->count, reinterpret_cast<uint16_t*>(d->valueBuff));
    case MBF_WRITE_SINGLE_COIL: // Write single coil
        return d->device->writeSingleCoil(d->unit, d->offset, d->valueBuff[0]);
    case MBF_WRITE_SINGLE_REGISTER: // Write single register
        return d->device->writeSingleRegister(d->unit, d->offset, reinterpret_cast<uint16_t*>(d->valueBuff)[0]);
    case MBF_READ_EXCEPTION_STATUS: // Write single register
        return d->device->readExceptionStatus(d->unit, d->valueBuff);
    case MBF_WRITE_MULTIPLE_COILS: // Write multiple coils
        return d->device->writeMultipleCoils(d->unit, d->offset, d->count, d->valueBuff);
    case MBF_WRITE_MULTIPLE_REGISTERS: // Write multiple registers
        return d->device->writeMultipleRegisters(d->unit, d->offset, d->count, reinterpret_cast<uint16_t*>(d->valueBuff));
    default:
        return d->setError(Status_BadIllegalFunction, StringLiteral("Unsupported function"));
    }
}

StatusCode ModbusServerResource::processOutputData(uint8_t *buff, uint16_t &sz)
{
    ModbusServerResourcePrivate *d = d_ModbusServerResource(d_ptr);
    switch (d->func)
    {
    case MBF_READ_COILS: // Read Coil Status
    case MBF_READ_DISCRETE_INPUTS: // Read Input Status
        buff[0] = static_cast<uint8_t>((d->count+7)/8);
        memcpy(&buff[1], d->valueBuff, buff[0]);
        sz = buff[0] + 1;
        break;
    case MBF_READ_HOLDING_REGISTERS: // Read holding registers
    case MBF_READ_INPUT_REGISTERS: // Read input registers
        buff[0] = static_cast<uint8_t>(d->count * 2);
        for (uint16_t i = 0; i < d->count; i++)
        {
            buff[2+i*2] = d->valueBuff[i*2];
            buff[1+i*2] = d->valueBuff[i*2+1];
        }
        sz = buff[0] + 1;
        break;
    case MBF_WRITE_SINGLE_COIL: // Write single coil
        buff[0] = static_cast<uint8_t>(d->offset >> 8);      // address of coil (Hi-byte)
        buff[1] = static_cast<uint8_t>(d->offset & 0xFF);    // address of coil (Lo-byte)
        buff[2] = d->valueBuff[0] ? 0xFF : 0x00;             // value (Hi-byte)
        buff[3] = 0;                                         // value (Lo-byte)
        sz = 4;
        break;
    case MBF_WRITE_SINGLE_REGISTER: // Write single register
        buff[0] = static_cast<uint8_t>(d->offset >> 8);      // address of register (Hi-byte)
        buff[1] = static_cast<uint8_t>(d->offset & 0xFF);    // address of register (Lo-byte)
        buff[2] = d->valueBuff[1];                           // value (Hi-byte)
        buff[3] = d->valueBuff[0];                           // value (Lo-byte)
        sz = 4;
        break;
    case MBF_READ_EXCEPTION_STATUS: // Read Exception Status
        buff[0] = d->valueBuff[0];
        sz = 1;
        break;
    case MBF_WRITE_MULTIPLE_COILS: // Write multiple coils
    case MBF_WRITE_MULTIPLE_REGISTERS: // Write multiple registers
        buff[0] = static_cast<uint8_t>(d->offset >> 8);      // offset of written values (Hi-byte)
        buff[1] = static_cast<uint8_t>(d->offset & 0xFF);    // offset of written values (Lo-byte)
        buff[2] = static_cast<uint8_t>(sz >> 8);            // count of written values (Hi-byte)
        buff[3] = static_cast<uint8_t>(sz & 0xFF);          // count of written values (Lo-byte)
        sz = 4;
        break;
    }
    return Status_Good;
}
