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
        const QString portName               ;
        const QString unit                   ;
        const QString funcWriteSingleCoil    ;
        const QString funcWriteSingleRegister;

        Strings();
        static const Strings &instance();
    };

    struct Defaults : public mbCoreDevice::Defaults
    {
        const QString portName               ;
        const uint8_t unit                   ;
        const uint8_t funcWriteSingleCoil    ;
        const uint8_t funcWriteSingleRegister;

        Defaults();
        static const Defaults &instance();
    };

public: // statistics
    struct Statistics : public CoreStatistics
    {
        quint32 countBadConnection;
        quint32 countBadTimeout   ;
        quint32 countBadCRC       ;

        Statistics();
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
    QString portName() const;
    void setPortName(const QString &portName);
    inline uint8_t unit() const { return m_settings.unit; }
    inline void setUnit(uint8_t unit) { m_settings.unit = unit; }
    inline uint8_t funcWriteSingleCoil() const { return m_settings.funcWriteSingleCoil; }
    inline void setFuncWriteSingleCoil(uint8_t funcWriteSingleCoil) { m_settings.funcWriteSingleCoil = funcWriteSingleCoil; }
    inline uint8_t funcWriteSingleRegister() const { return m_settings.funcWriteSingleRegister; }
    inline void setFuncWriteSingleRegister(uint8_t funcWriteSingleRegister) { m_settings.funcWriteSingleRegister = funcWriteSingleRegister; }

    MBSETTINGS settings() const override;
    bool setSettings(const MBSETTINGS &settings) override;

public: // statistics
    inline Statistics statistics() const { QReadLocker locker(&m_statLock); return *static_cast<Statistics*>(m_stat); }

private:
    void resetStatisticsInner() override;
    void setStatStatusInner(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString& err = QString()) override;

private:
    mbClientPort* m_port;

private: // settings
    struct
    {
        QString portName;
        uint8_t unit    ;
        uint8_t funcWriteSingleCoil;
        uint8_t funcWriteSingleRegister;
    } m_settings;
};

#endif // CLIENT_DEVICE_H
