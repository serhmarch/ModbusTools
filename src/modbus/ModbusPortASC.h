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
#ifndef MODBUSPORTASC_H
#define MODBUSPORTASC_H

#include "ModbusPortSerial.h"

namespace Modbus {

class MODBUS_EXPORT PortASC : public PortSerial
{
public:
    PortASC(QSerialPort* serialPort, QObject* parent = nullptr);
    PortASC(QObject* parent = nullptr);
    ~PortASC();

public:
    Type type() const override { return ASC; }

protected:
    StatusCode writeBuffer(uint8_t slave, uint8_t func, uint8_t *buff, uint16_t szInBuff) override;
    StatusCode readBuffer(uint8_t &slave, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff) override;
};

} // namespace Modbus

#endif // MODBUSPORTASC_H
