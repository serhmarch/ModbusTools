/*!
 * \file   ModbusAscPort.h
 * \brief  Contains definition of ASCII serial port class.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSASCPORT_H
#define MODBUSASCPORT_H

#include "ModbusSerialPort.h"

/*! \brief Implements ASCII version of the Modbus communication protocol.

    \details `ModbusAscPort` derived from `ModbusSerialPort` and implements `writeBuffer` and `readBuffer`
    for ASCII version of Modbus communication protocol.

 */
class MODBUS_EXPORT ModbusAscPort : public ModbusSerialPort
{
public:
    ///  \details Constructor of the class. if `blocking = true` then defines blocking mode, non blocking otherwise.
    ModbusAscPort(bool blocking = false);

    ///  \details Destructor of the class.
    ~ModbusAscPort();

public:
    /// \details Returns the Modbus protocol type. For `ModbusAscPort` returns `Modbus::ASC`.
    Modbus::ProtocolType type() const override { return Modbus::ASC; }

protected:
    Modbus::StatusCode writeBuffer(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff) override;
    Modbus::StatusCode readBuffer(uint8_t &unit, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff) override;

protected:
    using ModbusSerialPort::ModbusSerialPort;
};

#endif // MODBUSASCPORT_H
