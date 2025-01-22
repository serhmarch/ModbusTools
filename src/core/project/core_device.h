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

#include <mbcore.h>

class mbCoreProject;

class MB_EXPORT mbCoreDevice : public QObject
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString name                     ;
        const QString maxReadCoils             ;
        const QString maxReadDiscreteInputs    ;
        const QString maxReadHoldingRegisters  ;
        const QString maxReadInputRegisters    ;
        const QString maxWriteMultipleCoils    ;
        const QString maxWriteMultipleRegisters;
        const QString byteOrder                ;
        const QString registerOrder            ;
        const QString byteArrayFormat          ;
        const QString byteArraySeparator       ;
        const QString stringLengthType         ;
        const QString stringEncoding           ;

        Strings();
        static const Strings &instance();
    };

    struct MB_EXPORT Defaults
    {
        const QString              name                     ;
        const uint16_t             maxReadCoils             ;
        const uint16_t             maxReadDiscreteInputs    ;
        const uint16_t             maxReadHoldingRegisters  ;
        const uint16_t             maxReadInputRegisters    ;
        const uint16_t             maxWriteMultipleCoils    ;
        const uint16_t             maxWriteMultipleRegisters;
        const mb::DataOrder        byteOrder                ;
        const mb::RegisterOrder    registerOrder            ;
        const mb::DigitalFormat    byteArrayFormat          ;
        const QString              byteArraySeparator       ;
        const mb::StringLengthType stringLengthType         ;
        const mb::StringEncoding   stringEncoding           ;

        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbCoreDevice(QObject *parent = nullptr);

public:
    inline mbCoreProject* projectCore() const { return m_project; }
    void setProjectCore(mbCoreProject* project);

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
    inline mb::DataOrder byteOrder() const { return m_settingsCore.byteOrder; }
    inline void setByteOrder(mb::DataOrder byteOrder) { m_settingsCore.byteOrder = byteOrder; }
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

Q_SIGNALS:
    void nameChanged(const QString& newName);
    void changed();

protected:
    mbCoreProject* m_project;

protected: // settings
    struct
    {
        uint16_t             maxReadCoils             ;
        uint16_t             maxReadDiscreteInputs    ;
        uint16_t             maxReadInputRegisters    ;
        uint16_t             maxReadHoldingRegisters  ;
        uint16_t             maxWriteMultipleCoils    ;
        uint16_t             maxWriteMultipleRegisters;
        mb::DataOrder        byteOrder                ;
        mb::RegisterOrder    registerOrder            ;
        mb::DigitalFormat    byteArrayFormat          ;
        QString              byteArraySeparator       ;
        mb::StringLengthType stringLengthType         ;
        mb::StringEncoding   stringEncoding           ;
    } m_settingsCore;
};

#endif // CORE_DEVICE_H
