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
#ifndef CORE_PORT_H
#define CORE_PORT_H

#include <QObject>
#include <QSerialPort>

#include <mbcore.h>

class mbCoreProject;

class MB_EXPORT mbCorePort : public QObject
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString name;
        const QString type;

        Strings();
        static const Strings &instance();
    };

    struct MB_EXPORT Defaults
    {
        const QString       name;
        const Modbus::Type  type;

        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbCorePort(QObject *parent = nullptr);

public:
    inline mbCoreProject* projectCore() const { return m_project; }
    void setProjectCore(mbCoreProject* project);

public: // common settings
    inline QString name() const { return objectName(); }
    void setName(const QString &name);
    inline Modbus::Type type() const { return m_settings.type; }
    inline void setType(Modbus::Type type) { m_settings.type = type; }

public: // tcp settings
    inline QString host() const { return m_settings.host; }
    inline void setHost(const QString& host) { m_settings.host = host; }
    inline uint16_t port() const { return m_settings.port; }
    inline void setPort(uint16_t port) { m_settings.port = port; }
    inline uint32_t timeout() const { return m_settings.timeout; }
    inline void setTimeout(uint32_t timeout) { m_settings.timeout = timeout; }

public: // serial settings
    inline QString serialPortName() const { return m_settings.serialPortName; }
    inline void setSerialPortName(const QString& name) { m_settings.serialPortName = name; }
    inline qint32 baudRate() const { return m_settings.baudRate; }
    inline void setBaudRate(qint32 baudRate) { m_settings.baudRate = baudRate; }
    inline QSerialPort::DataBits dataBits() const { return m_settings.dataBits; }
    inline void setDataBits(QSerialPort::DataBits dataBits) { m_settings.dataBits = dataBits; }
    inline QSerialPort::StopBits stopBits() const { return m_settings.stopBits; }
    inline void setStopBits(QSerialPort::StopBits stopBits) { m_settings.stopBits = stopBits; }
    inline QSerialPort::Parity parity() const { return m_settings.parity; }
    inline void setParity(QSerialPort::Parity parity) { m_settings.parity = parity; }
    inline QSerialPort::FlowControl flowControl() const { return m_settings.flowControl; }
    inline void setFlowControl(QSerialPort::FlowControl flowControl) { m_settings.flowControl = flowControl; }
    inline uint32_t timeoutFirstByte() const { return m_settings.timeoutFB; }
    inline void setTimeoutFirstByte(uint32_t timeout) { m_settings.timeoutFB = timeout; }
    inline uint32_t timeoutInterByte() const { return m_settings.timeoutIB; }
    inline void setTimeoutInterByte(uint32_t timeout) { m_settings.timeoutIB = timeout; }

public: // settings
    virtual MBSETTINGS settings() const;
    virtual bool setSettings(const MBSETTINGS &settings);

Q_SIGNALS:
    void nameChanged(const QString& newName);
    void changed();

protected:
    mbCoreProject* m_project;

protected:
    struct
    {
        Modbus::Type                type          ;
        QString                     host          ;
        uint16_t                    port          ;
        uint32_t                    timeout       ;
        QString                     serialPortName;
        qint32                      baudRate      ;
        QSerialPort::DataBits       dataBits      ;
        QSerialPort::StopBits       stopBits      ;
        QSerialPort::Parity         parity        ;
        QSerialPort::FlowControl    flowControl   ;
        uint32_t                    timeoutFB     ;
        uint32_t                    timeoutIB     ;
    } m_settings;
};

#endif // CORE_PORT_H
