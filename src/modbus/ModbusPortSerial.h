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
#ifndef MODBUSPORTSERIAL_H
#define MODBUSPORTSERIAL_H

#include <QSerialPort>

#include "ModbusPort.h"

namespace Modbus {

class MODBUS_EXPORT PortSerial : public Port
{
public:
    struct MODBUS_EXPORT Strings : public Port::Strings
    {
        const QString serialPortName;
        const QString baudRate;
        const QString dataBits;
        const QString parity;
        const QString stopBits;
        const QString flowControl;
        const QString timeoutFirstByte;
        const QString timeoutInterByte;

        Strings();
        static const Strings &instance();
    };

    struct MODBUS_EXPORT Defaults : public Port::Defaults
    {
        const QString serialPortName;
        const qint32 baudRate;
        const QSerialPort::DataBits dataBits;
        const QSerialPort::Parity parity;
        const QSerialPort::StopBits stopBits;
        const QSerialPort::FlowControl flowControl;
        const uint32_t timeoutFirstByte;
        const uint32_t timeoutInterByte;

        Defaults();
        static const Defaults &instance();
    };

public:
    PortSerial(QSerialPort* serialPort, QObject* parent = nullptr);
    PortSerial(QObject* parent = nullptr);

public:
    StatusCode open() override;
    StatusCode close() override;
    bool isOpen() const override;

public: //settings
    inline QSerialPort* serialPort() const { return m_serialPort; }
    inline QString serialPortName() const { return m_serialPort->portName(); }
    inline void setSerialPortName(const QString& serialPort) { m_serialPort->setPortName(serialPort); }
    inline qint32 baudRate() const { return m_serialPort->baudRate(); }
    inline bool setBaudRate(qint32 baudRate) { return m_serialPort->setBaudRate(baudRate); }
    inline QSerialPort::DataBits dataBits() const { return m_serialPort->dataBits(); }
    inline bool setDataBits(QSerialPort::DataBits dataBits) { return m_serialPort->setDataBits(dataBits); }
    inline QSerialPort::StopBits stopBits() const { return m_serialPort->stopBits(); }
    inline bool setStopBits(QSerialPort::StopBits stopBits) { return m_serialPort->setStopBits(stopBits); }
    inline QSerialPort::Parity parity() const { return m_serialPort->parity(); }
    inline bool setParity(QSerialPort::Parity parity) { return m_serialPort->setParity(parity); }
    inline QSerialPort::FlowControl flowControl() const { return m_serialPort->flowControl(); }
    inline bool setFlowControl(QSerialPort::FlowControl flowControl) { return m_serialPort->setFlowControl(flowControl); }
    inline int timeoutFirstByte() const { return m_timeoutFB; }
    inline void setTimeoutFirstByte(int timeout) { m_timeoutFB = timeout; }
    inline int timeoutInterByte() const { return m_timeoutIB; }
    inline void setTimeoutInterByte(int timeout) { m_timeoutIB = timeout; }
    // settings
    Settings settings() const override;
    bool setSettings(const Settings& settings) override;

protected:
    StatusCode write() override;
    StatusCode read() override;

protected:
    QSerialPort *m_serialPort;
    uint32_t m_timeoutFB;
    uint32_t m_timeoutIB;
    qint64 m_timestamp;
    uint8_t *m_buff;
    uint16_t c_buffSz;
    uint16_t m_sz;
};

} // namespace Modbus

#endif // MODBUSPORTSERIAL_H
