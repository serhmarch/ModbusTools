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

#include <ModbusPortTCP.h>
#include <ModbusPortSerial.h>

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
    Defaults d = Defaults::instance();
    Modbus::PortTCP::Defaults dTCP = Modbus::PortTCP::Defaults::instance();
    Modbus::PortSerial::Defaults dSerial = Modbus::PortSerial::Defaults::instance();
    // common
    m_settings.type         = d.type;
    // tcp
    m_settings.host         = dTCP.host;
    m_settings.port         = dTCP.port;
    m_settings.timeout      = dTCP.timeout;
    // serial
    //m_settings.serialPortName = dSerial.serialPortName;
    m_settings.baudRate     = dSerial.baudRate;
    m_settings.dataBits     = dSerial.dataBits;
    m_settings.parity       = dSerial.parity;
    m_settings.stopBits     = dSerial.stopBits;
    m_settings.flowControl  = dSerial.flowControl;
    m_settings.timeoutFB    = dSerial.timeoutFirstByte;
    m_settings.timeoutIB    = dSerial.timeoutInterByte;

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
    Strings sPort = Strings::instance();
    Modbus::PortTCP::Strings sTCP = Modbus::PortTCP::Strings::instance();
    Modbus::PortSerial::Strings sSerial = Modbus::PortSerial::Strings::instance();

    MBSETTINGS r;
    // common
    r.insert(sPort.name, name());
    r.insert(sPort.type, mb::enumKeyTypeStr<Modbus::Type>(type()));
    // tcp
    r.insert(sTCP.host   , m_settings.host   );
    r.insert(sTCP.port   , m_settings.port   );
    r.insert(sTCP.timeout, m_settings.timeout);
    // serial
    r.insert(sSerial.serialPortName  , m_settings.serialPortName);
    r.insert(sSerial.baudRate        , m_settings.baudRate);
    r.insert(sSerial.dataBits        , mb::enumKeyTypeStr<QSerialPort::DataBits>   (m_settings.dataBits   ));
    r.insert(sSerial.parity          , mb::enumKeyTypeStr<QSerialPort::Parity>     (m_settings.parity     ));
    r.insert(sSerial.stopBits        , mb::enumKeyTypeStr<QSerialPort::StopBits>   (m_settings.stopBits   ));
    r.insert(sSerial.flowControl     , mb::enumKeyTypeStr<QSerialPort::FlowControl>(m_settings.flowControl));
    r.insert(sSerial.timeoutFirstByte, m_settings.timeoutFB);
    r.insert(sSerial.timeoutInterByte, m_settings.timeoutIB);
    return r;
}

bool mbCorePort::setSettings(const MBSETTINGS &settings)
{
    const Strings &sPort = Strings::instance();
    const Modbus::PortTCP::Strings &sTCP = Modbus::PortTCP::Strings::instance();
    const Modbus::PortSerial::Strings &sSerial = Modbus::PortSerial::Strings::instance();

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
        Modbus::Type v = mb::enumValue<Modbus::Type>(var, &ok);
        if (ok)
            setType(v);
    }

    // tcp
    it = settings.find(sTCP.host);
    if (it != end)
    {
        QVariant var = it.value();
        setHost(var.toString());
    }

    it = settings.find(sTCP.port);
    if (it != end)
    {
        QVariant var = it.value();
        uint16_t v = static_cast<uint16_t>(var.toUInt(&ok));
        if (ok)
            setPort(v);
    }

    it = settings.find(sTCP.timeout);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeout(v);
    }

    // serial
    it = settings.find(sSerial.serialPortName);
    if (it != end)
    {
        QVariant var = it.value();
        setSerialPortName(var.toString());
    }

    it = settings.find(sSerial.baudRate);
    if (it != end)
    {
        QVariant var = it.value();
        qint32 v = static_cast<qint32>(var.toInt(&ok));
        if (ok)
            setBaudRate(v);
    }

    it = settings.find(sSerial.dataBits);
    if (it != end)
    {
        QVariant var = it.value();
        QSerialPort::DataBits v = mb::enumValueTypeStr<QSerialPort::DataBits>(var.toString(), &ok);
        if (ok)
            setDataBits(v);
    }

    it = settings.find(sSerial.parity);
    if (it != end)
    {
        QVariant var = it.value();
        QSerialPort::Parity v = mb::enumValueTypeStr<QSerialPort::Parity>(var.toString(), &ok);
        if (ok)
            setParity(v);
    }

    it = settings.find(sSerial.stopBits);
    if (it != end)
    {
        QVariant var = it.value();
        QSerialPort::StopBits v = mb::enumValueTypeStr<QSerialPort::StopBits>(var.toString(), &ok);
        if (ok)
            setStopBits(v);
    }

    it = settings.find(sSerial.flowControl);
    if (it != end)
    {
        QVariant var = it.value();
        QSerialPort::FlowControl v = mb::enumValueTypeStr<QSerialPort::FlowControl>(var.toString(), &ok);
        if (ok)
            setFlowControl(v);
    }

    it = settings.find(sSerial.timeoutFirstByte);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeoutFirstByte(v);
    }

    it = settings.find(sSerial.timeoutInterByte);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setTimeoutInterByte(v);
    }

    return true;
}
