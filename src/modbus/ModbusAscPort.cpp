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
#include "ModbusAscPort.h"
#include "ModbusSerialPort_p.h"

ModbusAscPort::ModbusAscPort(bool blocking) :
    ModbusSerialPort(ModbusSerialPortPrivate::create(blocking))
{
    ModbusSerialPortPrivate *d = d_ModbusSerialPort(d_ptr);
    d->c_buffSz = MB_ASC_IO_BUFF_SZ;
    d->buff = new uint8_t[MB_ASC_IO_BUFF_SZ];
}

ModbusAscPort::~ModbusAscPort()
{
    delete d_ModbusSerialPort(d_ptr)->buff;
}

StatusCode ModbusAscPort::writeBuffer(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff)
{
    ModbusSerialPortPrivate *d = d_ModbusSerialPort(d_ptr);
    const uint16_t szIBuff = MB_ASC_IO_BUFF_SZ/2;
    uint8_t ibuff[szIBuff];
    // 3 is unit, func and LRC bytes
    if (szInBuff > szIBuff-3)
        return d->setError(Modbus::Status_BadWriteBufferOverflow, StringLiteral("Write-buffer overflow"));
    ibuff[0] = unit;
    ibuff[1] = func;
    memcpy(&ibuff[2], buff, szInBuff);
    ibuff[szInBuff + 2] = Modbus::lrc(ibuff, szInBuff+2);
    d->sz = Modbus::bytesToAscii(ibuff, &d->buff[1], szInBuff + 3);
    d->buff[0]       = ':' ;  // start ASCII-message character
    d->buff[d->sz+1] = '\r';  // CR
    d->buff[d->sz+2] = '\n';  // LF
    d->sz += 3;
    return Status_Good;
}

StatusCode ModbusAscPort::readBuffer(uint8_t& unit, uint8_t &func, uint8_t* buff, uint16_t maxSzBuff, uint16_t* szOutBuff)
{
    ModbusSerialPortPrivate *d = d_ModbusSerialPort(d_ptr);
    const uint16_t szIBuff = MB_ASC_IO_BUFF_SZ/2;
    uint8_t ibuff[szIBuff];

    if (d->sz < 9) // 9 = 1(':')+2(unit)+2(func)+2(lrc)+1('\r')+1('\n')
        return d->setError(Status_BadNotCorrectRequest, StringLiteral("ASCII. Not correct response. Responsed data length to small"));

    if (d->buff[0] != ':')
        return d->setError(Status_BadAscMissColon, StringLiteral("ASCII. Missed colon ':' symbol"));

    if ((d->buff[d->sz-2] != '\r') || (d->buff[d->sz-1] != '\n'))
        return d->setError(Status_BadAscMissCrLf, StringLiteral("ASCII. Missed CR-LF ending symbols"));

    if ((d->sz = Modbus::asciiToBytes(&d->buff[1], ibuff, d->sz-3)) == 0)
        return d->setError(Status_BadAscChar, StringLiteral("ASCII. Bad ASCII symbol"));

    if (Modbus::lrc(ibuff, d->sz-1) != ibuff[d->sz-1])
        return d->setError(Status_BadLrc, StringLiteral("ASCII. Error LRC"));

    unit = ibuff[0];
    func = ibuff[1];

    d->sz -= 3; // 3 = 1(unit)+1(func)+1(lrc)
    if (d->sz > maxSzBuff)
        return d->setError(Modbus::Status_BadReadBufferOverflow, StringLiteral("ASCII. Read-buffer overflow"));
    memcpy(buff, &ibuff[2], d->sz);
    *szOutBuff = d->sz;
    return Status_Good;
}
