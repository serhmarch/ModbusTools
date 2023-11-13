/*
    Modbus

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
#include "ModbusServerPort.h"

namespace Modbus {

ServerPort::Strings::Strings() :
    type(QStringLiteral("type")),
    name(QStringLiteral("name")),
    unit(QStringLiteral("unit"))
{
}

const ServerPort::Strings &ServerPort::Strings::instance()
{
    static const Strings s;
    return s;
}

ServerPort::Defaults::Defaults() :
    type(TCP),
    unit(VALID_MODBUS_ADDRESS_BEGIN)
{
}

const ServerPort::Defaults &ServerPort::Defaults::instance()
{
    static const Defaults d;
    return d;
}


ServerPort::ServerPort(Port *port, Interface *device, QObject *parent) :
    QObject (parent)
{
    m_state = STATE_UNKNOWN;
    m_cmdClose = false;
    m_port = port;
    if (port) // TODO: find better descision
    {
        port->setParent(this);
        port->setServerMode(true);
        connect(m_port, &Port::signalTx     , this, &ServerPort::slotTx     );
        connect(m_port, &Port::signalRx     , this, &ServerPort::slotRx     );
        connect(m_port, &Port::signalError  , this, &ServerPort::setError   );
        connect(m_port, &Port::signalMessage, this, &ServerPort::setMessage );
    }
    m_device = device;
}

Modbus::Type ServerPort::type() const
{
    return m_port->type();
}

StatusCode ServerPort::open()
{
    m_cmdClose = false;
    return Status_Good;
}

StatusCode ServerPort::close()
{
    m_cmdClose = true;
    return Status_Good;
}

bool ServerPort::isOpen() const
{
    return m_port->isOpen();
}

ServerPort::Status ServerPort::status() const
{
    switch(m_state)
    {
    case STATE_UNKNOWN:
        return Unknown;
    case STATE_WAIT_FOR_CLOSE:
        return WaitForFinalize;
    case STATE_CLOSED:
        return Finalized;
    case STATE_WAIT_FOR_OPEN:
        return WaitForInitialize;
    default:
        return Initialized;
    }
}

Settings ServerPort::settings()
{
    return m_port->settings();
}

bool ServerPort::setSettings(const Settings &settings)
{
    return m_port->setSettings(settings);
}

StatusCode ServerPort::process()
{
    const int szBuff = 500;

    StatusCode r = Status_Good;
    uint8_t buff[szBuff], func;
    uint16_t outBytes, outCount = 0;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_CLOSED:
            if (m_cmdClose)
                break;
            m_state = STATE_WAIT_FOR_OPEN;
            // no need break
        case STATE_WAIT_FOR_OPEN:
            if (m_cmdClose)
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            r = m_port->open();
            if (!StatusIsGood(r))  // processing or an error occured
                return r;
            m_state = STATE_OPENED;
            fRepeatAgain = true;
            break;
        case STATE_WAIT_FOR_CLOSE:
            r = m_port->close();
            if (StatusIsProcessing(r)) // if not OK it's mean that an error occured or in process
                return r;
            m_state = STATE_CLOSED;
            break;
        case STATE_OPENED:
            m_state = STATE_BEGIN_READ;
            // no need break
        case STATE_BEGIN_READ:
            if (m_cmdClose)
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            r = m_port->read();
            if (!StatusIsGood(r))  // processing or an error occured
                return r;
            m_state = STATE_READ;
            // no need break
        case STATE_READ:
            // verify slave id
            r = m_port->readBuffer(m_unit, m_func, buff, szBuff, &outBytes);
            if (StatusIsGood(r))
                r = processInputData(buff, outBytes);
            if (StatusIsBad(r)) // data error
            {
                if (StatusIsStandardError(r)) // return standard error to device
                {
                    m_state = STATE_WRITE;
                    fRepeatAgain = true;
                    break;
                }
                else
                {
                    m_state = STATE_BEGIN_READ;
                    return r;
                }
            }
            m_state = STATE_PROCESS_DEVICE;
            // no need break
        case STATE_PROCESS_DEVICE:
            r = processDevice();
            if (StatusIsProcessing(r))
                return r;
            if (r == Status_BadUnknownUnit)
            {
                m_state = STATE_BEGIN_READ;
                return r;
            }
            m_state = STATE_WRITE;
            // no need break
        case STATE_WRITE:
            func = m_func;
            if (StatusIsBad(r))
            {
                func |= MBF_EXCEPTION;
                if (StatusIsStandardError(r))
                    buff[0] = static_cast<uint8_t>(r & 0xFF);
                else
                    buff[0] = static_cast<uint8_t>(Status_BadSlaveDeviceFailure & 0xFF);
                outCount = 1;
            }
            else
                processOutputData(buff, outCount);
            m_port->writeBuffer(m_unit, func, buff, outCount);
            m_state = STATE_BEGIN_WRITE;
            // no need break
        case STATE_BEGIN_WRITE:
            r = m_port->write();
            if (StatusIsProcessing(r))
                return r;
            m_state = STATE_BEGIN_READ;
            return r;
        default:
            if (m_cmdClose && isOpen())
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
            }
            else if (isOpen())
            {
                m_state = STATE_OPENED;
                fRepeatAgain = true;
            }
            else
                m_state = STATE_CLOSED;
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

void ServerPort::slotTx(const QByteArray &bytes)
{
    Q_EMIT signalTx(name(), bytes);
}

void ServerPort::slotRx(const QByteArray &bytes)
{
    Q_EMIT signalRx(name(), bytes);
}

StatusCode ServerPort::setError(StatusCode code, const QString &message)
{
    Q_EMIT signalError(name(), code, message);
    return code;
}

void ServerPort::setMessage(const QString &message)
{
    Q_EMIT signalMessage(name(), message);
}

StatusCode ServerPort::processInputData(const uint8_t *buff, uint16_t sz)
{
    switch (m_func)
    {
    case MBF_READ_COILS:            // Read Coil Status
    case MBF_READ_DISCRETE_INPUTS:  // Read Input Status
        if (sz != 4) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        m_offset = buff[1] | (buff[0] << 8);
        m_count  = buff[3] | (buff[2] << 8);
        if (m_count > MB_MAX_DISCRETS) // prevent valueBuff overflow
            return Status_BadIllegalDataValue;
        break;
    case MBF_READ_HOLDING_REGISTERS:    // Read holding registers
    case MBF_READ_INPUT_REGISTERS:      // Read input registers
        if (sz != 4) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        m_offset = buff[1] | (buff[0]<<8);
        m_count = buff[3] | (buff[2]<<8);
        if (m_count > MB_MAX_REGISTERS) // prevent valueBuff overflow
            return Status_BadIllegalDataValue;
        break;
    case MBF_WRITE_SINGLE_COIL: // Write single coil
        if (sz != 4) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        if (!(buff[2] == 0x00 || buff[2] == 0xFF) || (buff[3] != 0))  // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        m_offset = buff[1] | (buff[0]<<8);
        m_valueBuff[0] = buff[2];
        break;
    case MBF_WRITE_SINGLE_REGISTER: // Write single register
        if (sz != 4) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        m_offset = buff[1] | (buff[0]<<8);
        m_valueBuff[0] = buff[3];
        m_valueBuff[1] = buff[2];
        break;
    case MBF_READ_EXCEPTION_STATUS: // Read exception status
        if (sz > 0) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        break;
    case MBF_WRITE_MULTIPLE_COILS: // Write multiple coils
        if (sz < 5) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        if (sz != buff[4]+5) // don't match readed bytes and number of data bytes to follow
            return Status_BadNotCorrectRequest;
        m_offset = buff[1] | (buff[0]<<8);
        m_count = buff[3] | (buff[2]<<8);
        if ((m_count+7)/8 != buff[4]) // don't match count bites and bytes
            return Status_BadNotCorrectRequest;
        if (m_count > MB_MAX_DISCRETS) // prevent valueBuff overflow
            return Status_BadIllegalDataValue;
        memcpy(m_valueBuff, &buff[5], (m_count+7)/8);
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS: // Write multiple registers
        if (sz < 5) // not correct request from client - don't respond
            return Status_BadNotCorrectRequest;
        if (sz != buff[4]+5) // don't match readed bytes and number of data bytes to follow
            return Status_BadNotCorrectRequest;
        m_offset = buff[1] | (buff[0]<<8);
        m_count = buff[3] | (buff[2]<<8);
        if (m_count*2 != buff[4]) // don't match count values and bytes
            return Status_BadNotCorrectRequest;
        if (m_count > MB_MAX_REGISTERS) // prevent valueBuff overflow
            return Status_BadIllegalDataValue;
        for (uint16_t i = 0; i < m_count; i++)
        {
            m_valueBuff[i*2]   = buff[6+i*2];
            m_valueBuff[i*2+1] = buff[5+i*2];
        }
        break;
    default:
        return Status_BadIllegalFunction;
    }
    return Status_Good;
}

StatusCode ServerPort::processDevice()
{
    switch (m_func)
    {
    case MBF_READ_COILS: // Read Coil Status
        return m_device->readCoils(m_unit, m_offset, m_count, m_valueBuff);
    case MBF_READ_DISCRETE_INPUTS: // Read Input Status
        return m_device->readDiscreteInputs(m_unit, m_offset, m_count, m_valueBuff);
    case MBF_READ_HOLDING_REGISTERS: // Read holding registers
        return m_device->readHoldingRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_valueBuff));
    case MBF_READ_INPUT_REGISTERS: // Read input registers
        return m_device->readInputRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_valueBuff));
    case MBF_WRITE_SINGLE_COIL: // Write single coil
        return m_device->writeSingleCoil(m_unit, m_offset, m_valueBuff[0]);
    case MBF_WRITE_SINGLE_REGISTER: // Write single register
        return m_device->writeSingleRegister(m_unit, m_offset, reinterpret_cast<uint16_t*>(m_valueBuff)[0]);
    case MBF_READ_EXCEPTION_STATUS: // Write single register
        return m_device->readExceptionStatus(m_unit, m_valueBuff);
    case MBF_WRITE_MULTIPLE_COILS: // Write multiple coils
        return m_device->writeMultipleCoils(m_unit, m_offset, m_count, m_valueBuff);
    case MBF_WRITE_MULTIPLE_REGISTERS: // Write multiple registers
        return m_device->writeMultipleRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_valueBuff));
    default:
        return Status_BadIllegalFunction;
    }
}

StatusCode ServerPort::processOutputData(uint8_t *buff, uint16_t &sz)
{
    switch (m_func)
    {
    case MBF_READ_COILS: // Read Coil Status
    case MBF_READ_DISCRETE_INPUTS: // Read Input Status
        buff[0] = static_cast<uint8_t>((m_count+7)/8);
        memcpy(&buff[1], m_valueBuff, buff[0]);
        sz = buff[0] + 1;
        break;
    case MBF_READ_HOLDING_REGISTERS: // Read holding registers
    case MBF_READ_INPUT_REGISTERS: // Read input registers
        buff[0] = static_cast<uint8_t>(m_count * 2);
        for (uint16_t i = 0; i < m_count; i++)
        {
            buff[2+i*2] = m_valueBuff[i*2];
            buff[1+i*2] = m_valueBuff[i*2+1];
        }
        sz = buff[0] + 1;
        break;
    case MBF_WRITE_SINGLE_COIL: // Write single coil
        buff[0] = static_cast<uint8_t>(m_offset >> 8);      // address of coil (Hi-byte)
        buff[1] = static_cast<uint8_t>(m_offset & 0xFF);    // address of coil (Lo-byte)
        buff[2] = m_valueBuff[0] ? 0xFF : 0x00;             // value (Hi-byte)
        buff[3] = 0;                                        // value (Lo-byte)
        sz = 4;
        break;
    case MBF_WRITE_SINGLE_REGISTER: // Write single register
        buff[0] = static_cast<uint8_t>(m_offset >> 8);      // address of register (Hi-byte)
        buff[1] = static_cast<uint8_t>(m_offset & 0xFF);    // address of register (Lo-byte)
        buff[2] = m_valueBuff[1];                           // value (Hi-byte)
        buff[3] = m_valueBuff[0];                           // value (Lo-byte)
        sz = 4;
        break;
    case MBF_READ_EXCEPTION_STATUS: // Read Exception Status
        buff[0] = m_valueBuff[0];
        sz = 1;
        break;
    case MBF_WRITE_MULTIPLE_COILS: // Write multiple coils
    case MBF_WRITE_MULTIPLE_REGISTERS: // Write multiple registers
        buff[0] = static_cast<uint8_t>(m_offset >> 8);      // offset of written values (Hi-byte)
        buff[1] = static_cast<uint8_t>(m_offset & 0xFF);    // offset of written values (Lo-byte)
        buff[2] = static_cast<uint8_t>(sz >> 8);            // count of written values (Hi-byte)
        buff[3] = static_cast<uint8_t>(sz & 0xFF);          // count of written values (Lo-byte)
        sz = 4;
        break;
    }
    return Status_Good;
}

} // namespace Modbus

