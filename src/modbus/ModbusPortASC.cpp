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
#include "ModbusPortASC.h"

namespace Modbus {

PortASC::PortASC(QSerialPort *serialPort, QObject *parent) : PortSerial(serialPort, parent)
{
    c_buffSz = MB_ASC_IO_BUFF_SZ;
    m_buff = new quint8[c_buffSz];
}

PortASC::PortASC(QObject *parent) : PortSerial(parent)
{
    c_buffSz = MB_ASC_IO_BUFF_SZ;
    m_buff = new quint8[c_buffSz];
}

PortASC::~PortASC()
{
    delete m_buff;
}

StatusCode PortASC::writeBuffer(quint8 slave, quint8 func, quint8 *buff, quint16 szInBuff)
{
    if (!m_modeServer)
    {
        if (m_block)
            return Status_Processing;
        m_unit = slave;
        m_func = func;
        m_block = true;
    }
    const quint16 szIBuff = MB_ASC_IO_BUFF_SZ/2;
    quint8 ibuff[szIBuff];
    // 3 is slave, func and LRC bytes
    if (szInBuff > szIBuff-3)
        return setError(Modbus::Status_BadWriteBufferOverflow, QStringLiteral("Write-buffer overflow"));
    ibuff[0] = slave;
    ibuff[1] = func;
    memcpy(&ibuff[2], buff, szInBuff);
    ibuff[szInBuff + 2] = Modbus::lrc(ibuff, szInBuff+2);
    m_sz = Modbus::bytesToAscii(ibuff, &m_buff[1], szInBuff + 3);
    m_buff[0]      = ':' ;  // start ASCII-message character
    m_buff[m_sz+1] = '\r';  // CR
    m_buff[m_sz+2] = '\n';  // LF
    m_sz += 3;
    return Status_Good;
}

StatusCode PortASC::readBuffer(quint8& slave, quint8 &func, quint8* buff, quint16 maxSzBuff, quint16* szOutBuff)
{
    const quint16 szIBuff = MB_ASC_IO_BUFF_SZ/2;
    quint8 ibuff[szIBuff];

    if (m_sz < 9) // 9 = 1(':')+2(unit)+2(func)+2(lrc)+1('\r')+1('\n')
        return setError(Status_BadNotCorrectRequest, QStringLiteral("Not correct response. Responsed data length to small"));

    if (m_buff[0] != ':')
        return setError(Status_BadAscMissColon, QStringLiteral("ASCII-mode. Missed colon ':' symbol"));

    if ((m_buff[m_sz-2] != '\r') || (m_buff[m_sz-1] != '\n'))
        return setError(Status_BadAscMissCrLf, QStringLiteral("ASCII-mode. Missed CR-LF ending symbols"));

    if ((m_sz = Modbus::asciiToBytes(&m_buff[1], ibuff, m_sz-3)) == 0)
        return setError(Status_BadAscChar, QStringLiteral("ASCII-mode. Bad ASCII symbol"));

    if (Modbus::lrc(ibuff, m_sz-1) != ibuff[m_sz-1])
        return setError(Status_BadLrc, QStringLiteral("ASCII-mode. Error LRC"));

    if (!m_modeServer)
    {
        if (m_unit != ibuff[0])
            return setError(Status_BadNotCorrectRequest, QStringLiteral("Not correct response. Requested unit (slave) address not equal to responded"));

        if ((ibuff[1] & MBF_EXCEPTION) == MBF_EXCEPTION)
            return setError(static_cast<StatusCode>(Status_Bad | buff[2]), QStringLiteral("Returned modbus exception"));

        if (m_func != ibuff[1])
            return setError(Status_BadNotCorrectRequest, QStringLiteral("Not correct response. Requested function not equal to responded"));
    }

    slave = ibuff[0];
    func = ibuff[1];

    m_sz -= 3; // 3 = 1(unit)+1(func)+1(lrc)
    if (m_sz > maxSzBuff)
        return setError(Modbus::Status_BadReadBufferOverflow, QStringLiteral("Read-buffer overflow"));
    memcpy(buff, &ibuff[2], m_sz);
    *szOutBuff = m_sz;
    return Status_Good;
}

} // namespace Modbus
