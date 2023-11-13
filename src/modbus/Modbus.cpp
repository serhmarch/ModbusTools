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
#include "Modbus.h"

#include "ModbusClientPort.h"
#include "ModbusServerPort.h"
#include "ModbusPortRTU.h"
#include "ModbusPortASC.h"
#include "ModbusPortTCP.h"
#include "ModbusServerTCP.h"

namespace Modbus {

uint16_t crc16(const uint8_t *bytes, uint32_t count)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < count; i++)
    {
        crc ^= bytes[i];
        for (uint32_t j = 0; j < 8; j++)
        {
            uint16_t temp = crc & 0x0001;
            crc >>= 1;
            if (temp) crc ^= 0xA001;
        }
    }
    return crc;
}

uint8_t lrc(const uint8_t *bytes, uint32_t count)
{
    uint8_t lrc = 0x00;
    for (; count; count--)
        lrc += *bytes++;
    return static_cast<uint8_t>(-static_cast<int8_t>(lrc));
}

uint16_t bytesToAscii(uint8_t* bytesBuff, uint8_t* asciiBuff, uint16_t count)
{
    uint16_t i, j, qAscii = 0;
    uint8_t tmp;

    for (i = 0; i < count; i++)
    {
        for (j = 0; j < 2; j++)
        {
            tmp = (bytesBuff[i] >> ((1 - j) * 4)) & 0x0F;
            asciiBuff[i * 2 + j] = ((tmp < 10) ? (0x30 | tmp) : (55 + tmp));
            qAscii++;
        }
    }
    return qAscii;
}

uint16_t asciiToBytes(uint8_t* asciiBuff, uint8_t* bytesBuff, uint16_t count)
{
    uint16_t i, j, qBytes = 0;
    uint8_t tmp;
    for (i = 0; i < count; i++)
    {
        j = i & 1;
        if (!j)
        {
            bytesBuff[i / 2] = 0;
            qBytes++;
        }
        tmp = asciiBuff[i];
        if ((tmp >= '0') && (tmp <= '9'))
            tmp &= 0x0F;
        else if ((tmp >= 'A') && (tmp <= 'F'))
            tmp -= 55;
        else
            return 0;
        bytesBuff[i / 2] |= tmp << (4 * (1 - j));
    }
    return qBytes;
}

QString bytesToString(const QByteArray& bytes)
{
    QString str;
    for (int i = 0; i < bytes.count(); ++i)
    {
        uint8_t num = *reinterpret_cast<const uint8_t*>(&bytes.constData()[i]);
        QString  c = QString::number(num, 16).toUpper();
        if (c.length() == 1)
            c = "0" + c;
        str += c + " ";
    }
    return str;
}

QString asciiToString(const QByteArray &bytes)
{
    QString str;
    int j = 0;
    for (int i = 0; i < bytes.count(); ++i)
    {
        QChar c = QChar(bytes.at(i));
        if (c == QChar::CarriageReturn)
            str += ((str.right(1) == " ") ? "CR " : " CR ");
        else if (c == QChar::LineFeed)
            str += ((str.right(1) == " ") ? "LF " : " LF ");
        else
        {
            str += c;
            j++;
            if (j & 1)
                str += " ";
        }
    }
    return str;
}

ClientPort *createClientPort(const Settings &settings, QObject *parent)
{
    Port *p;
    Type type = enumValue<Type>(settings.value(Port::Strings::instance().type).toString());
    switch (type)
    {
    case Modbus::RTU:
        p = new PortRTU();
        break;
    case Modbus::ASC:
        p = new PortASC();
        break;
    case Modbus::TCP:
        p = new PortTCP();
        break;
    default:
        return nullptr;
    }
    ClientPort *port = new ClientPort(p, parent);
    port->setSettings(settings);
    return port;
}

ServerPort *createServerPort(const Settings &settings, Interface *device, QObject *parent)
{
    Port *p;
    ServerPort *port;
    Type type = enumValue<Type>(settings.value(Port::Strings::instance().type).toString());
    switch (type)
    {
    case Modbus::RTU:
        p = new PortRTU();
        port = new ServerPort(p, device, parent);
        break;
    case Modbus::ASC:
        p = new PortASC();
        port = new ServerPort(p, device, parent);
        break;
    case Modbus::TCP:
        port = new ServerTCP(device, parent);
        break;
    default:
        return nullptr;
    }
    port->setSettings(settings);
    return port;
}

} //namespace Modbus
