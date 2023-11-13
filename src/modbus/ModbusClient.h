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
#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include "ModbusClientPort.h"

namespace Modbus {

class MODBUS_EXPORT Client : public QObject, public Interface
{
    Q_OBJECT

public:
    struct MODBUS_EXPORT Strings
    {
        const QString unit;

        Strings();
        static const Strings &instance();
    };

    struct MODBUS_EXPORT Defaults
    {
        const uint8_t unit;

        Defaults();
        static const Defaults &instance();
    };

public:
    Client(uint8_t unit, ClientPort *port, QObject *parent = nullptr);
    ~Client();

public:
    Type type() const;
    inline QString name() const { return objectName(); }
    void setName(const QString &name);
    inline uint8_t unit() const { return m_unit; }
    inline void setUnit(uint8_t unit) { m_unit = unit; }
    bool isOpen() const;
    Settings settings() const;
    bool setSettings(const Settings& settings);
    inline ClientPort *port() const { return m_port; }

public:
    inline StatusCode readCoils(uint16_t offset, uint16_t count, void *values) { return readCoils(m_unit, offset, count, values); }
    inline StatusCode readDiscreteInputs(uint16_t offset, uint16_t count, void *values) { return readDiscreteInputs(m_unit, offset, count, values); }
    inline StatusCode readHoldingRegisters(uint16_t offset, uint16_t count, uint16_t *values) { return readHoldingRegisters(m_unit, offset, count, values); }
    inline StatusCode readInputRegisters(uint16_t offset, uint16_t count, uint16_t *values) { return readInputRegisters(m_unit, offset, count, values); }
    inline StatusCode writeSingleCoil(uint16_t offset, bool value) { return writeSingleCoil(m_unit, offset, value); }
    inline StatusCode writeSingleRegister(uint16_t offset, uint16_t value) { return writeSingleRegister(m_unit, offset, value); }
    inline StatusCode readExceptionStatus(uint8_t *value) { return readExceptionStatus(m_unit, value); }
    inline StatusCode writeMultipleCoils(uint16_t offset, uint16_t count, const void *values) { return writeMultipleCoils(m_unit, offset, count, values); }
    inline StatusCode writeMultipleRegisters(uint16_t offset, uint16_t count, const uint16_t *values) { return writeMultipleRegisters(m_unit, offset, count, values); }

    inline StatusCode readCoilStatusAsBoolArray(uint16_t offset, uint16_t count, bool *values) { return readCoilStatusAsBoolArray(m_unit, offset, count, values); }
    inline StatusCode readInputStatusAsBoolArray(uint16_t offset, uint16_t count, bool *values) { return readInputStatusAsBoolArray(m_unit, offset, count, values); }
    inline StatusCode forceMultipleCoilsAsBoolArray(uint16_t offset, uint16_t count, const bool *values) { return forceMultipleCoilsAsBoolArray(m_unit, offset, count, values); }

public: // Modbus Interface
    virtual StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values);
    virtual StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values);
    virtual StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);
    virtual StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);
    virtual StatusCode writeSingleCoil(uint8_t unit, uint16_t offset, bool value);
    virtual StatusCode writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value);
    virtual StatusCode readExceptionStatus(uint8_t unit, uint8_t *value);
    virtual StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values);
    virtual StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values);

public:
    StatusCode readCoilStatusAsBoolArray(uint8_t unit, uint16_t offset, uint16_t count, bool *values);
    StatusCode readInputStatusAsBoolArray(uint8_t unit, uint16_t offset, uint16_t count, bool *values);
    StatusCode forceMultipleCoilsAsBoolArray(uint8_t unit, uint16_t offset, uint16_t count, const bool *values);

public:
    inline QString lastErrorText() const { return m_lastErrorText; }

private:
    StatusCode request(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff, uint16_t maxSzBuff, uint16_t *szOutBuff);

private:
    uint8_t m_unit;
    ClientPort *m_port;
    ClientPort::RequestParams *m_rp;
    uint8_t m_buff[MB_VALUE_BUFF_SZ];
    QString m_lastErrorText;
};

} // namespace Modbus

#endif // MODBUSCLIENT_H
