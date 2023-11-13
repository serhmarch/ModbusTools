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
#ifndef MODBUSPORTRTU_H
#define MODBUSPORTRTU_H

#include "ModbusPortSerial.h"

namespace Modbus {

class MODBUS_EXPORT PortRTU : public PortSerial
{
public:
    PortRTU(QSerialPort *serialPort, QObject *parent = nullptr);
    PortRTU(QObject* parent = nullptr);
    ~PortRTU();

public:
    Type type() const override { return RTU; }

protected:
    StatusCode writeBuffer(uint8_t slave, uint8_t func, uint8_t *buff, uint16_t szInBuff) override;
    StatusCode readBuffer(uint8_t &slave, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff) override;
};

} // namespace Modbus

#endif // MODBUSPORTRTU_H
