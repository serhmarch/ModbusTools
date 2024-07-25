/*
    Modbus

    Created: 2024
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2024  Serhii Marchuk

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
#include "ModbusRtuPort.h"
#include "ModbusSerialPort_p.h"

ModbusRtuPort::ModbusRtuPort(bool blocking) :
    ModbusSerialPort(ModbusSerialPortPrivate::create(blocking))
{
    ModbusSerialPortPrivate *d = d_ModbusSerialPort(d_ptr);
    d->c_buffSz = MB_RTU_IO_BUFF_SZ;
    d->buff = new uint8_t[MB_RTU_IO_BUFF_SZ];
}

ModbusRtuPort::~ModbusRtuPort()
{
    delete d_ModbusSerialPort(d_ptr)->buff;
}

StatusCode ModbusRtuPort::writeBuffer(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff)
{
    ModbusSerialPortPrivate *d = d_ModbusSerialPort(d_ptr);
    uint16_t crc;
    // 2 is unit and function bytes + 2 bytes CRC16
    if (szInBuff > MB_RTU_IO_BUFF_SZ-(sizeof(crc)+2))
        return d->setError(Status_BadWriteBufferOverflow, StringLiteral("RTU. Write-buffer overflow"));
    d->buff[0] = unit;
    d->buff[1] = func;
    memcpy(&d->buff[2], buff, szInBuff);
    d->sz = szInBuff + 2;
    crc = Modbus::crc16(d->buff, d->sz);
    d->buff[d->sz  ] = reinterpret_cast<uint8_t*>(&crc)[0];
    d->buff[d->sz+1] = reinterpret_cast<uint8_t*>(&crc)[1];
    d->sz += 2;
    return Status_Good;
}

StatusCode ModbusRtuPort::readBuffer(uint8_t& unit, uint8_t& func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff)
{
    ModbusSerialPortPrivate *d = d_ModbusSerialPort(d_ptr);
    uint16_t crc;
    if (d->sz < 5)
        return d->setError(Status_BadNotCorrectRequest, StringLiteral("RTU. Not correct response. Responsed data length to small"));

    crc = d->buff[d->sz-2] | (d->buff[d->sz-1] << 8);
    if (Modbus::crc16(d->buff, d->sz-2) != crc)
        return d->setError(Status_BadCrc, StringLiteral("RTU. Wrong CRC"));

    unit = d->buff[0];
    func = d->buff[1];

    d->sz -= 4;
    if (d->sz > maxSzBuff)
        return d->setError(Status_BadReadBufferOverflow, StringLiteral("RTU. Read-buffer overflow"));
    memcpy(buff, &d->buff[2], d->sz);
    *szOutBuff = d->sz;
    return Status_Good;
}
