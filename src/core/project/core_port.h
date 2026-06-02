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
#include <QReadWriteLock>

#include <mbcore.h>

class mbCoreProject;

class MBTOOLS_EXPORT mbCorePort : public QObject
{
    Q_OBJECT

public:
    struct MBTOOLS_EXPORT Strings
    {
        const QString name;
        const QString type;

        Strings();
        static const Strings &instance();
    };

    struct MBTOOLS_EXPORT Defaults
    {
        const QString       name;
        const Modbus::ProtocolType  type;

        Defaults();
        static const Defaults &instance();
    };

public: // statistics
    struct MBTOOLS_EXPORT CoreStatistics : public mb::BaseStatistics
    {
        quint32 countBadTimeout  ;
        quint32 countBadCRC      ;
        quint32 cycleCount       ;
        quint64 cycleSumDuration ;
        quint32 cycleLastDuration;
        quint32 cycleMinDuration ;
        quint32 cycleMaxDuration ;
        quint32 cycleAvgDuration ;

        CoreStatistics();
    };

public:
    explicit mbCorePort(QObject *parent = nullptr);
    ~mbCorePort();

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
    inline QString ipaddr() const { return m_settings.host; }
    inline void setIpaddr(const QString& host) { m_settings.host = host; }
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

public: // statistics
    inline CoreStatistics statisticsCore() const { QReadLocker locker(&m_statLock); return *m_stat; }
    virtual void resetStatistics();

    inline quint32 statTimeResponseLast () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseLast ; }
    inline quint32 statTimeResponseMin  () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseMin  ; }
    inline quint32 statTimeResponseMax  () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseMax  ; }
    inline quint32 statTimeResponseAvg  () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseAvg  ; }
    inline quint64 statTimeResponseTotal() const { QReadLocker locker(&m_statLock); return m_stat->timeResponseTotal; }
    inline quint32 statTimeResponseCount() const { QReadLocker locker(&m_statLock); return m_stat->timeResponseCount; }

    inline void updateResponseTime(quint64 responseTime) { QWriteLocker locker(&m_statLock); m_stat->updateResponseTime(responseTime); }

    inline quint32 statCountTx() const { QReadLocker locker(&m_statLock); return m_stat->countTx; }
    inline quint32 statCountRx() const { QReadLocker locker(&m_statLock); return m_stat->countRx; }
    inline quint32 statCountGood() const { QReadLocker locker(&m_statLock); return m_stat->countGood; }
    inline quint32 statCountBad() const { QReadLocker locker(&m_statLock); return m_stat->countBad; }
    inline quint32 statCountBadTimeout() const { QReadLocker locker(&m_statLock); return m_stat->countBadTimeout; }
    inline quint32 statCountBadCRC() const { QReadLocker locker(&m_statLock); return m_stat->countBadCRC; }

    void incStatCountTx();
    void incStatCountRx();

    virtual void setStatCycleTime(quint64 time);

    virtual void setStatStatus(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString& err = QString());

protected:
    virtual void resetStatisticsInner();
    virtual void setStatCycleTimeInner(quint64 time);
    virtual void setStatStatusInner(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString& err = QString());

Q_SIGNALS:
    void nameChanged(const QString& newName);
    void changed();
    void statCountTxChanged(quint32 count);
    void statCountRxChanged(quint32 count);

protected:
    mbCoreProject* m_project;

protected:
    struct
    {
        Modbus::ProtocolType        type              ;
        QString                     host              ;
        QString                     ipaddr            ;
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

protected: // statistics
    mutable QReadWriteLock m_statLock;
    CoreStatistics *m_stat;
};

#endif // CORE_PORT_H
