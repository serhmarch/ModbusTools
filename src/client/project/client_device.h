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
#ifndef CLIENT_DEVICE_H
#define CLIENT_DEVICE_H

#include <QSerialPort>
#include <QMutex>

#include <Modbus.h>
#include <client.h>

#include <project/core_device.h>

class mbClientProject;
class mbClientPort;

class mbClientDevice : public mbCoreDevice
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDevice::Strings
    {
        const QString unit    ;
        const QString portName;

        Strings();
        static const Strings &instance();
    };

    struct Defaults : public mbCoreDevice::Defaults
    {
        const uint8_t unit    ;
        const QString portName;

        Defaults();
        static const Defaults &instance();
    };

public:
    mbClientDevice(QObject* parent = nullptr);
    ~mbClientDevice();

public:
    inline mb::Client::DeviceHandle_t handle() const { return const_cast<mb::Client::DeviceHandle_t>(this); }
    inline mbClientPort* port() const { return m_port; }
    void setPort(mbClientPort* port);
    inline mbClientProject* project() const { return reinterpret_cast<mbClientProject*>(mbCoreDevice::projectCore()); }
    inline void setProject(mbClientProject* project) { mbCoreDevice::setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public: // settings
    inline QString name() const { return objectName(); }
    void setName(const QString& name);
    inline uint8_t unit() const { return m_settings.unit; }
    inline void setUnit(uint8_t unit) { m_settings.unit = unit; }
    QString portName() const;
    void setPortName(const QString &portName);

    MBSETTINGS settings() const override;
    bool setSettings(const MBSETTINGS &settings) override;

private:
    mbClientPort* m_port;

private: // settings
    struct
    {
        uint8_t unit    ;
        QString portName;
    } m_settings;
};

#endif // CLIENT_DEVICE_H
