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
#include "core_port.h"

#include "core_project.h"

mbCorePort::Strings::Strings() :
    name(QStringLiteral("name")),
    type(QStringLiteral("type"))

{
}

const mbCorePort::Strings &mbCorePort::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCorePort::Defaults::Defaults() :
    name(QStringLiteral("Port")),
    type(Modbus::TCP)
{
}

const mbCorePort::Defaults &mbCorePort::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCorePort::mbCorePort(QObject *parent)
    : QObject{parent}
{
    const Modbus::Defaults &d = Modbus::Defaults::instance();

    // common
    m_settings.type         = d.type;
    // tcp
    m_settings.host         = d.host;
    m_settings.port         = d.port;
    m_settings.timeout      = d.timeout;
    // serial
    //m_settings.serialPortName = dSerial.serialPortName;
    m_settings.baudRate     = d.baudRate;
    m_settings.dataBits     = d.dataBits;
    m_settings.parity       = d.parity;
    m_settings.stopBits     = d.stopBits;
    m_settings.flowControl  = d.flowControl;
    m_settings.timeoutFB    = d.timeoutFirstByte;
    m_settings.timeoutIB    = d.timeoutInterByte;

    m_project = nullptr;
}

void mbCorePort::setProjectCore(mbCoreProject *project)
{
    m_project = project;
}

void mbCorePort::setName(const QString &name)
{
    QString tn = name;
    if (tn.isEmpty())
        tn = Defaults::instance().name;
    if (m_project && m_project->portCore(tn) != this)
    {
        if (m_project->hasPort(tn))
            tn = m_project->freePortName(tn);
        m_project->portRename(this, tn);
    }
    setObjectName(tn);
    Q_EMIT changed();
    Q_EMIT nameChanged(tn);
}

MBSETTINGS mbCorePort::settings() const
{
    const Strings &sPort = Strings::instance();
    const Modbus::Strings &s = Modbus::Strings::instance();

    MBSETTINGS r;
    // common
    r.insert(sPort.name, name());
    r.insert(sPort.type, Modbus::toString(type()));
    // tcp
    r.insert(s.host   , m_settings.host   );
    r.insert(s.port   , m_settings.port   );
    r.insert(s.timeout, m_settings.timeout);
    // serial
    r.insert(s.serialPortName  , m_settings.serialPortName);
    r.insert(s.baudRate        , m_settings.baudRate);
    r.insert(s.dataBits        , m_settings.dataBits);
    r.insert(s.parity          , Modbus::toString(m_settings.parity     ));
    r.insert(s.stopBits        , Modbus::toString(m_settings.stopBits   ));
    r.insert(s.flowControl     , Modbus::toString(m_settings.flowControl));
    r.insert(s.timeoutFirstByte, m_settings.timeoutFB);
    r.insert(s.timeoutInterByte, m_settings.timeoutIB);
    return r;
}

bool mbCorePort::setSettings(const MBSETTINGS &settings)
{
    const Strings &sPort = Strings::instance();
    const Modbus::Strings &s = Modbus::Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    // common
    it = settings.find(sPort.name);
    if (it != end)
    {
        QVariant var = it.value();
        setName(var.toString());
    }

    it = settings.find(sPort.type);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::ProtocolType v = Modbus::toProtocolType(var, &ok);
        if (ok)
            setType(v);
    }

    // tcp
    it = settings.find(s.host);
    if (it != end)
    {
        QVariant var = it.value();
        setHost(var.toString());
    }

    it = settings.find(s.port);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setPort(v);
    }

    it = settings.find(s.timeout);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeout(v);
    }

    // serial
    it = settings.find(s.serialPortName);
    if (it != end)
    {
        QVariant var = it.value();
        setSerialPortName(var.toString());
    }

    it = settings.find(s.baudRate);
    if (it != end)
    {
        QVariant var = it.value();
        int32_t v = static_cast<int32_t>(var.toInt(&ok));
        if (ok)
            setBaudRate(v);
    }

    it = settings.find(s.dataBits);
    if (it != end)
    {
        QVariant var = it.value();
        int8_t v = Modbus::toDataBits(var, &ok);
        if (ok)
            setDataBits(v);
    }

    it = settings.find(s.parity);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::Parity v = Modbus::toParity(var, &ok);
        if (ok)
            setParity(v);
    }

    it = settings.find(s.stopBits);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::StopBits v = Modbus::toStopBits(var, &ok);
        if (ok)
            setStopBits(v);
    }

    it = settings.find(s.flowControl);
    if (it != end)
    {
        QVariant var = it.value();
        Modbus::FlowControl v = Modbus::toFlowControl(var, &ok);
        if (ok)
            setFlowControl(v);
    }

    it = settings.find(s.timeoutFirstByte);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeoutFirstByte(v);
    }

    it = settings.find(s.timeoutInterByte);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeoutInterByte(v);
    }

    return true;
}
