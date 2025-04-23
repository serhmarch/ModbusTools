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
        const Modbus::ProtocolType  type;

        Defaults();
        static const Defaults &instance();
    };

    struct Statistic
    {
        Statistic()
        {
            countTx = 0;
            countRx = 0;
        }
        quint32 countTx;
        quint32 countRx;
    };

public:
    explicit mbCorePort(QObject *parent = nullptr);

public:
    inline mbCoreProject* projectCore() const { return m_project; }
    void setProjectCore(mbCoreProject* project);

public: // common settings
    inline QString name() const { return objectName(); }
    void setName(const QString &name);
    virtual QString extendedName() const = 0;
    inline Modbus::ProtocolType type() const { return m_settings.type; }
    inline void setType(Modbus::ProtocolType type) { m_settings.type = type; }

public: // tcp settings
    inline QString host() const { return m_settings.host; }
    inline void setHost(const QString& host) { m_settings.host = host; }
    inline uint16_t port() const { return m_settings.port; }
    inline void setPort(uint16_t port) { m_settings.port = port; }
    inline uint32_t timeout() const { return m_settings.timeout; }
    inline void setTimeout(uint32_t timeout) { m_settings.timeout = timeout; }
    inline uint32_t maxConnections() const { return m_settings.maxconn; }
    inline void setMaxConnections(uint32_t maxconn) { m_settings.maxconn = maxconn; }

public: // serial settings
    inline QString serialPortName() const { return m_settings.serialPortName; }
    inline void setSerialPortName(const QString& name) { m_settings.serialPortName = name; }
    inline int32_t baudRate() const { return m_settings.baudRate; }
    inline void setBaudRate(int32_t baudRate) { m_settings.baudRate = baudRate; }
    inline int8_t dataBits() const { return m_settings.dataBits; }
    inline void setDataBits(int8_t dataBits) { m_settings.dataBits = dataBits; }
    inline Modbus::StopBits stopBits() const { return m_settings.stopBits; }
    inline void setStopBits(Modbus::StopBits stopBits) { m_settings.stopBits = stopBits; }
    inline Modbus::Parity parity() const { return m_settings.parity; }
    inline void setParity(Modbus::Parity parity) { m_settings.parity = parity; }
    inline Modbus::FlowControl flowControl() const { return m_settings.flowControl; }
    inline void setFlowControl(Modbus::FlowControl flowControl) { m_settings.flowControl = flowControl; }
    inline uint32_t timeoutFirstByte() const { return m_settings.timeoutFB; }
    inline void setTimeoutFirstByte(uint32_t timeout) { m_settings.timeoutFB = timeout; }
    inline uint32_t timeoutInterByte() const { return m_settings.timeoutIB; }
    inline void setTimeoutInterByte(uint32_t timeout) { m_settings.timeoutIB = timeout; }

public: // common settings
    inline bool isBroadcastEnabled() const { return m_settings.isBroadcastEnabled; }
    inline void setBroadcastEnabled(bool enable) { m_settings.isBroadcastEnabled = enable; }

public: // settings
    virtual MBSETTINGS settings() const;
    virtual bool setSettings(const MBSETTINGS &settings);

public: // statistic
    inline Statistic statistic() const { return m_stat; }

    inline quint32 statGoodCount() const { return m_stat.countTx; }
    void setStatCountTx(quint32 count);

    inline quint32 statBadCount() const { return m_stat.countRx; }
    void setStatCountRx(quint32 count);

Q_SIGNALS:
    void nameChanged(const QString& newName);
    void changed();
    void statCountTxChanged(quint32 count);
    void statCountRxChanged (quint32 count);

protected:
    mbCoreProject* m_project;

protected:
    struct
    {
        Modbus::ProtocolType        type              ;
        QString                     host              ;
        uint16_t                    port              ;
        uint32_t                    timeout           ;
        uint32_t                    maxconn           ;
        QString                     serialPortName    ;
        int32_t                     baudRate          ;
        int8_t                      dataBits          ;
        Modbus::StopBits            stopBits          ;
        Modbus::Parity              parity            ;
        Modbus::FlowControl         flowControl       ;
        uint32_t                    timeoutFB         ;
        uint32_t                    timeoutIB         ;
        bool                        isBroadcastEnabled;
    } m_settings;

    Statistic m_stat;
};

#endif // CORE_PORT_H
