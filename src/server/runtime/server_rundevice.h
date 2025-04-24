/*
    Modbus Tools

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
#ifndef SERVER_RUNDEVICE_H
#define SERVER_RUNDEVICE_H

#include <QSet>
#include <mbcore.h>

class mbServerDevice;

class mbServerRunDevice : public ModbusInterface
{
public:
    explicit mbServerRunDevice();
    virtual ~mbServerRunDevice();

public: // Modbus::Interface
    Modbus::StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values) override;
    Modbus::StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values) override;
    Modbus::StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) override;
    Modbus::StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) override;
    Modbus::StatusCode writeSingleCoil(uint8_t unit, uint16_t offset, bool value) override;
    Modbus::StatusCode writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value) override;
    Modbus::StatusCode readExceptionStatus(uint8_t unit, uint8_t *status) override;
    Modbus::StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values) override;
    Modbus::StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values) override;
    Modbus::StatusCode reportServerID(uint8_t unit, uint8_t *count, uint8_t *data) override;
    Modbus::StatusCode maskWriteRegister(uint8_t unit, uint16_t offset, uint16_t andMask, uint16_t orMask) override;
    Modbus::StatusCode readWriteMultipleRegisters(uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues) override;

public: // settings
    inline bool isBroadcastEnabled() const { return m_settings.isBroadcastEnabled; }
    inline void setBroadcastEnabled(bool enable) { m_settings.isBroadcastEnabled = enable; }

public:
    inline bool isBroadcast(uint8_t unit) const { return (unit == 0) && isBroadcastEnabled(); }
    inline QSet<mbServerDevice*> devices() const { return m_devices; }
    inline QSet<uint8_t> unitNumbers() const { return m_unitNumbers; }
    inline mbServerDevice *device(uint8_t unit) const { return m_units[unit]; }
    void setDevice(uint8_t unit, mbServerDevice *device);

private:
    struct
    {
        bool isBroadcastEnabled;
    } m_settings;

private: // devices
    static const int UnitsSize = 256;
    mbServerDevice *m_units[UnitsSize];
    QSet<mbServerDevice*> m_devices;
    QSet<uint8_t> m_unitNumbers;
    mb::Timestamp_t m_timestamp;
};

#endif // SERVER_RUNDEVICE_H
