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
#ifndef CORE_DEVICE_H
#define CORE_DEVICE_H

#include <QObject>
#include <QReadWriteLock>

#include <mbcore.h>

class mbCoreProject;

class MBTOOLS_EXPORT mbCoreDevice : public QObject
{
    Q_OBJECT

public:
    struct MBTOOLS_EXPORT Strings
    {
        const QString name                     ;
        const QString maxReadCoils             ;
        const QString maxReadDiscreteInputs    ;
        const QString maxReadHoldingRegisters  ;
        const QString maxReadInputRegisters    ;
        const QString maxWriteMultipleCoils    ;
        const QString maxWriteMultipleRegisters;
        const QString swapBytes                ;
        const QString registerOrder            ;
        const QString byteArrayFormat          ;
        const QString byteArraySeparator       ;
        const QString stringLengthType         ;
        const QString stringEncoding           ;

        Strings();
        static const Strings &instance();
    };

    struct MBTOOLS_EXPORT Defaults
    {
        const QString              name                     ;
        const uint16_t             maxReadCoils             ;
        const uint16_t             maxReadDiscreteInputs    ;
        const uint16_t             maxReadHoldingRegisters  ;
        const uint16_t             maxReadInputRegisters    ;
        const uint16_t             maxWriteMultipleCoils    ;
        const uint16_t             maxWriteMultipleRegisters;
        const mb::SwapData         swapBytes                ;
        const mb::RegisterOrder    registerOrder            ;
        const mb::DigitalFormat    byteArrayFormat          ;
        const QString              byteArraySeparator       ;
        const mb::StringLengthType stringLengthType         ;
        const mb::StringEncoding   stringEncoding           ;

        Defaults();
        static const Defaults &instance();
    };

public: // statistics
    struct MBTOOLS_EXPORT CoreStatistics : public mb::BaseStatistics
    {
        quint32 countBadIllegalFunction                   ;
        quint32 countBadIllegalDataAddress                ;
        quint32 countBadIllegalDataValue                  ;
        quint32 countBadServerDeviceFailure               ;
        quint32 countBadAcknowledge                       ;
        quint32 countBadServerDeviceBusy                  ;
        quint32 countBadNegativeAcknowledge               ;
        quint32 countBadMemoryParityError                 ;
        quint32 countBadGatewayPathUnavailable            ;
        quint32 countBadGatewayTargetDeviceFailedToRespond;
        CoreStatistics();
    };

public:
    explicit mbCoreDevice(QObject *parent = nullptr);
    ~mbCoreDevice();

public:
    inline mbCoreProject* projectCore() const { return m_project; }
    void setProjectCore(mbCoreProject* project);
    inline bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enable);
    inline void toggleEnabled() { setEnabled(!m_enabled); }

public: // settings
    inline QString name() const { return objectName(); }
    void setName(const QString& name);
    inline uint16_t maxReadCoils() const { return m_settingsCore.maxReadCoils; }
    inline void setMaxReadCoils(uint16_t max) { m_settingsCore.maxReadCoils = max; }
    inline uint16_t maxReadDiscreteInputs() const { return m_settingsCore.maxReadDiscreteInputs; }
    inline void setMaxReadDiscreteInputs(uint16_t max) { m_settingsCore.maxReadDiscreteInputs = max; }
    inline uint16_t maxReadInputRegisters() const { return m_settingsCore.maxReadInputRegisters; }
    inline void setMaxReadInputRegisters(uint16_t max) { m_settingsCore.maxReadInputRegisters = max; }
    inline uint16_t maxReadHoldingRegisters() const { return m_settingsCore.maxReadHoldingRegisters; }
    inline void setMaxReadHoldingRegisters(uint16_t max) { m_settingsCore.maxReadHoldingRegisters = max; }
    inline uint16_t maxWriteMultipleCoils() const { return m_settingsCore.maxWriteMultipleCoils; }
    inline void setMaxWriteMultipleCoils(uint16_t max) { m_settingsCore.maxWriteMultipleCoils = max; }
    inline uint16_t maxWriteMultipleRegisters() const { return m_settingsCore.maxWriteMultipleRegisters; }
    inline void setMaxWriteMultipleRegisters(uint16_t max) { m_settingsCore.maxWriteMultipleRegisters = max; }
    inline mb::SwapData swapBytes() const { return m_settingsCore.swapBytes; }
    inline void setSwapBytes(mb::SwapData swapBytes) { m_settingsCore.swapBytes = swapBytes; }
    inline mb::RegisterOrder registerOrder() const { return m_settingsCore.registerOrder; }
    inline void setRegisterOrder(mb::RegisterOrder registerOrder) { m_settingsCore.registerOrder = registerOrder; }
    inline mb::DigitalFormat byteArrayFormat() const { return m_settingsCore.byteArrayFormat; }
    inline void setByteArrayFormat(mb::DigitalFormat byteArrayFormat) { m_settingsCore.byteArrayFormat = byteArrayFormat; }
    inline QString byteArraySeparator() const { return m_settingsCore.byteArraySeparator; }
    inline void setByteArraySeparator(const QString &byteArraySeparator) { m_settingsCore.byteArraySeparator = byteArraySeparator; }
    QString byteArraySeparatorStr() const;
    void setByteArraySeparatorStr(const QString &byteArraySeparator);
    inline mb::StringLengthType stringLengthType() const { return m_settingsCore.stringLengthType; }
    inline void setStringLengthType(mb::StringLengthType stringLengthType) { m_settingsCore.stringLengthType = stringLengthType; }
    inline mb::StringEncoding stringEncoding() const { return m_settingsCore.stringEncoding; }
    inline void setStringEncoding(const mb::StringEncoding &stringEncoding) { m_settingsCore.stringEncoding = stringEncoding; }
    QString stringEncodingStr() const;
    void setStringEncodingStr(const QString& stringEncodingStr);

    virtual MBSETTINGS settings() const;
    virtual bool setSettings(const MBSETTINGS& settings);

public: // statistics
    inline CoreStatistics statisticsCore() const { QReadLocker locker(&m_statLock); return *m_stat; }
    inline quint32 statCountTx() const { QReadLocker locker(&m_statLock); return m_stat->countTx; }
    inline quint32 statCountRx() const { QReadLocker locker(&m_statLock); return m_stat->countRx; }
    inline quint32 statCountGood() const { QReadLocker locker(&m_statLock); return m_stat->countGood; }
    inline quint32 statCountBad() const { QReadLocker locker(&m_statLock); return m_stat->countBad; }
    inline quint32 statCountBadStandard() const { QReadLocker locker(&m_statLock); return m_stat->countBadStandard; }

    inline quint32 statTimeResponseLast () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseLast ; }
    inline quint32 statTimeResponseMin  () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseMin  ; }
    inline quint32 statTimeResponseMax  () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseMax  ; }
    inline quint32 statTimeResponseAvg  () const { QReadLocker locker(&m_statLock); return m_stat->timeResponseAvg  ; }
    inline quint64 statTimeResponseTotal() const { QReadLocker locker(&m_statLock); return m_stat->timeResponseTotal; }
    inline quint32 statTimeResponseCount() const { QReadLocker locker(&m_statLock); return m_stat->timeResponseCount; }

    inline void updateResponseTime(quint64 responseTime) { QWriteLocker locker(&m_statLock); m_stat->updateResponseTime(responseTime); }

    inline quint32 statCountBadIllegalFunction                   () const { QReadLocker locker(&m_statLock); return m_stat->countBadIllegalFunction                   ; }
    inline quint32 statCountBadIllegalDataAddress                () const { QReadLocker locker(&m_statLock); return m_stat->countBadIllegalDataAddress                ; }
    inline quint32 statCountBadIllegalDataValue                  () const { QReadLocker locker(&m_statLock); return m_stat->countBadIllegalDataValue                  ; }
    inline quint32 statCountBadServerDeviceFailure               () const { QReadLocker locker(&m_statLock); return m_stat->countBadServerDeviceFailure               ; }
    inline quint32 statCountBadAcknowledge                       () const { QReadLocker locker(&m_statLock); return m_stat->countBadAcknowledge                       ; }
    inline quint32 statCountBadServerDeviceBusy                  () const { QReadLocker locker(&m_statLock); return m_stat->countBadServerDeviceBusy                  ; }
    inline quint32 statCountBadNegativeAcknowledge               () const { QReadLocker locker(&m_statLock); return m_stat->countBadNegativeAcknowledge               ; }
    inline quint32 statCountBadMemoryParityError                 () const { QReadLocker locker(&m_statLock); return m_stat->countBadMemoryParityError                 ; }
    inline quint32 statCountBadGatewayPathUnavailable            () const { QReadLocker locker(&m_statLock); return m_stat->countBadGatewayPathUnavailable            ; }
    inline quint32 statCountBadGatewayTargetDeviceFailedToRespond() const { QReadLocker locker(&m_statLock); return m_stat->countBadGatewayTargetDeviceFailedToRespond; }

    void incStatCountTx();
    void incStatCountRx();

    virtual void resetStatistics();
    virtual void setStatStatus(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString& err = QString());

protected:
    virtual void resetStatisticsInner();
    virtual void setStatStatusInner(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString& err = QString());

Q_SIGNALS:
    void nameChanged(const QString& newName);
    void changed();
    void enabledChanged(bool enabled);
    void statCountTxChanged(quint32 count);
    void statCountRxChanged(quint32 count);


protected:
    mbCoreProject* m_project;
    bool m_enabled;

protected: // settings
    struct
    {
        uint16_t             maxReadCoils             ;
        uint16_t             maxReadDiscreteInputs    ;
        uint16_t             maxReadInputRegisters    ;
        uint16_t             maxReadHoldingRegisters  ;
        uint16_t             maxWriteMultipleCoils    ;
        uint16_t             maxWriteMultipleRegisters;
        mb::SwapData         swapBytes                ;
        mb::RegisterOrder    registerOrder            ;
        mb::DigitalFormat    byteArrayFormat          ;
        QString              byteArraySeparator       ;
        mb::StringLengthType stringLengthType         ;
        mb::StringEncoding   stringEncoding           ;
    } m_settingsCore;

protected: // statistics
    mutable QReadWriteLock m_statLock;
    CoreStatistics *m_stat;
};

#endif // CORE_DEVICE_H
