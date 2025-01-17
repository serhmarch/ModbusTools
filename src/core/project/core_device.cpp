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
#include "core_device.h"

#include "core_project.h"

mbCoreDevice::Strings::Strings() :
    name                     (QStringLiteral("name")),
    maxReadCoils             (QStringLiteral("maxReadCoils")),
    maxReadDiscreteInputs    (QStringLiteral("maxReadDiscreteInputs")),
    maxReadHoldingRegisters  (QStringLiteral("maxReadHoldingRegisters")),
    maxReadInputRegisters    (QStringLiteral("maxReadInputRegisters")),
    maxWriteMultipleCoils    (QStringLiteral("maxWriteMultipleCoils")),
    maxWriteMultipleRegisters(QStringLiteral("maxWriteMultipleRegisters")),
    registerOrder            (QStringLiteral("registerOrder")),
    byteArrayFormat          (QStringLiteral("byteArrayFormat")),
    byteArraySeparator       (QStringLiteral("byteArraySeparator")),
    stringLengthType         (QStringLiteral("stringLengthType")),
    stringEncoding           (QStringLiteral("stringEncoding"))
{
}

const mbCoreDevice::Strings &mbCoreDevice::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDevice::Defaults::Defaults() :
    name(QStringLiteral("Device")),
    maxReadCoils(MB_MAX_DISCRETS),
    maxReadDiscreteInputs(MB_MAX_DISCRETS),
    maxReadHoldingRegisters(MB_MAX_REGISTERS),
    maxReadInputRegisters(MB_MAX_REGISTERS),
    maxWriteMultipleCoils(MB_MAX_DISCRETS),
    maxWriteMultipleRegisters(MB_MAX_REGISTERS),
    registerOrder(mb::R0R1R2R3),
    byteArrayFormat(mb::Hex),
    byteArraySeparator(QStringLiteral(" ")),
    stringLengthType(mb::ZerroEnded),
    stringEncoding(mb::Defaults::instance().stringEncoding)
{
}

const mbCoreDevice::Defaults &mbCoreDevice::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCoreDevice::mbCoreDevice(QObject *parent)
    : QObject{parent}
{
    Defaults d = Defaults();

    m_project = nullptr;

    m_settingsCore.maxReadCoils                 = d.maxReadCoils             ;
    m_settingsCore.maxReadDiscreteInputs        = d.maxReadDiscreteInputs    ;
    m_settingsCore.maxReadHoldingRegisters      = d.maxReadHoldingRegisters  ;
    m_settingsCore.maxReadInputRegisters        = d.maxReadInputRegisters    ;
    m_settingsCore.maxWriteMultipleCoils        = d.maxWriteMultipleCoils    ;
    m_settingsCore.maxWriteMultipleRegisters    = d.maxWriteMultipleRegisters;
    m_settingsCore.registerOrder                = d.registerOrder            ;
    m_settingsCore.byteArrayFormat              = d.byteArrayFormat          ;
    m_settingsCore.byteArraySeparator           = d.byteArraySeparator       ;
    m_settingsCore.stringLengthType             = d.stringLengthType         ;
    m_settingsCore.stringEncoding               = d.stringEncoding           ;
}

void mbCoreDevice::setProjectCore(mbCoreProject *project)
{
    m_project = project;
}

void mbCoreDevice::setName(const QString &name)
{
    QString tn = name;
    if (tn.isEmpty())
        tn = Defaults::instance().name;
    if (m_project && m_project->deviceCore(tn) != this)
    {
        if (m_project->hasDevice(tn))
            tn = m_project->freeDeviceName(tn);
        m_project->deviceRename(this, tn);
    }
    setObjectName(tn);
    Q_EMIT changed();
    Q_EMIT nameChanged(tn);
}

QString mbCoreDevice::byteArraySeparatorStr() const
{
    return mb::makeEscapeSequnces(m_settingsCore.byteArraySeparator);
}

void mbCoreDevice::setByteArraySeparatorStr(const QString &byteArraySeparator)
{
    m_settingsCore.byteArraySeparator = mb::resolveEscapeSequnces(byteArraySeparator);
}

QString mbCoreDevice::stringEncodingStr() const
{
    return mb::fromStringEncoding(m_settingsCore.stringEncoding);
}

void mbCoreDevice::setStringEncodingStr(const QString &stringEncodingStr)
{
    m_settingsCore.stringEncoding = mb::toStringEncoding(stringEncodingStr);
}

MBSETTINGS mbCoreDevice::settings() const
{
    Strings s = Strings();

    MBSETTINGS r;

    r.insert(s.name                     , name                      ());
    r.insert(s.maxReadCoils             , maxReadCoils              ());
    r.insert(s.maxReadDiscreteInputs    , maxReadDiscreteInputs     ());
    r.insert(s.maxReadHoldingRegisters  , maxReadHoldingRegisters   ());
    r.insert(s.maxReadInputRegisters    , maxReadInputRegisters     ());
    r.insert(s.maxWriteMultipleCoils    , maxWriteMultipleCoils     ());
    r.insert(s.maxWriteMultipleRegisters, maxWriteMultipleRegisters ());
    r.insert(s.registerOrder            , mb::toString(registerOrder()));
    r.insert(s.byteArrayFormat          , mb::enumDigitalFormatKey(byteArrayFormat()));
    r.insert(s.byteArraySeparator       , byteArraySeparatorStr());
    r.insert(s.stringLengthType         , mb::enumStringLengthTypeKey(stringLengthType()));
    r.insert(s.stringEncoding           , stringEncodingStr());

    return r;
}

bool mbCoreDevice::setSettings(const MBSETTINGS &settings)
{
    Strings s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.name);
    if (it != end)
    {
        QVariant var = it.value();
        setName(var.toString());
    }

    it = settings.find(s.maxReadCoils);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setMaxReadCoils(v);
    }

    it = settings.find(s.maxReadDiscreteInputs);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setMaxReadDiscreteInputs(v);
    }

    it = settings.find(s.maxReadHoldingRegisters);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setMaxReadHoldingRegisters(v);
    }

    it = settings.find(s.maxReadInputRegisters);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setMaxReadInputRegisters(v);
    }

    it = settings.find(s.maxWriteMultipleCoils);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setMaxWriteMultipleCoils(v);
    }

    it = settings.find(s.maxWriteMultipleRegisters);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setMaxWriteMultipleRegisters(v);
    }

    it = settings.find(s.registerOrder);
    if (it != end)
    {
        QVariant var = it.value();
        mb::RegisterOrder v = mb::toRegisterOrder(var.toString(), &ok);
        if (ok)
            setRegisterOrder(v);
    }

    it = settings.find(s.byteArrayFormat);
    if (it != end)
    {
        QVariant var = it.value();
        mb::DigitalFormat v = mb::enumDigitalFormatValue(var.toString(), &ok);
        if (ok)
            setByteArrayFormat(v);
    }

    it = settings.find(s.byteArraySeparator);
    if (it != end)
    {
        QVariant var = it.value();
        setByteArraySeparatorStr(var.toString());
    }

    it = settings.find(s.stringLengthType);
    if (it != end)
    {
        QVariant var = it.value();
        mb::StringLengthType v = mb::enumStringLengthTypeValue(var.toString(), &ok);
        if (ok)
            setStringLengthType(v);
    }

    it = settings.find(s.stringEncoding);
    if (it != end)
    {
        QVariant var = it.value();
        setStringEncodingStr(var.toString());
    }

    Q_EMIT changed();
    return true;
}
