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
#include "client_device.h"

#include "client_port.h"
#include "client_project.h"

mbClientDevice::Strings::Strings() :
    mbCoreDevice::Strings(),
    portName(QStringLiteral("portName")),
    unit    (QStringLiteral("unit")),
    funcWriteSingleCoil    (QStringLiteral("funcWriteSingleCoil")),
    funcWriteSingleRegister(QStringLiteral("funcWriteSingleRegister"))
{
}

const mbClientDevice::Strings &mbClientDevice::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDevice::Defaults::Defaults() :
    mbCoreDevice::Defaults(),
    portName               (mbClientPort::Defaults::instance().name),
    unit                   (Modbus::Defaults::instance().unit),
    funcWriteSingleCoil    (MBF_WRITE_MULTIPLE_COILS),
    funcWriteSingleRegister(MBF_WRITE_MULTIPLE_REGISTERS)
{
}

const mbClientDevice::Defaults &mbClientDevice::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbClientDevice::Statistics::Statistics() :
    CoreStatistics()
{
    countBadConnection = 0;
    countBadTimeout    = 0;
    countBadCRC        = 0;
}

mbClientDevice::mbClientDevice(QObject *parent) :
    mbCoreDevice(parent)
{
    Defaults d = Defaults();

    m_port = nullptr;

    m_stat = new Statistics;

    m_settings.unit = d.unit;
}

mbClientDevice::~mbClientDevice()
{
}

void mbClientDevice::setPort(mbClientPort *port)
{
    m_port = port;
}

void mbClientDevice::setName(const QString & /*name*/)
{

}

QString mbClientDevice::portName() const
{
    if (m_port)
        return m_port->name();
    return m_settings.portName;
}

void mbClientDevice::setPortName(const QString &portName)
{
    m_settings.portName = portName;
    Q_EMIT changed();
}

MBSETTINGS mbClientDevice::settings() const
{
    Strings s = Strings();

    MBSETTINGS r = mbCoreDevice::settings();

    r.insert(s.portName               , portName               ());
    r.insert(s.unit                   , unit                   ());
    r.insert(s.funcWriteSingleCoil    , funcWriteSingleCoil    ());
    r.insert(s.funcWriteSingleRegister, funcWriteSingleRegister());

    return r;
}

bool mbClientDevice::setSettings(const MBSETTINGS &settings)
{
    Strings s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.portName);
    if (it != end)
    {
        QVariant var = it.value();
        setPortName(var.toString());
    }

    it = settings.find(s.unit);
    if (it != end)
    {
        QVariant var = it.value();
        uint8_t v = static_cast<uint8_t>(var.toUInt(&ok));
        if (ok)
            setUnit(v);
    }

    it = settings.find(s.funcWriteSingleCoil);
    if (it != end)
    {
        QVariant var = it.value();
        uint8_t v = static_cast<uint8_t>(var.toUInt(&ok));
        if (ok)
            setFuncWriteSingleCoil(v);
    }

    it = settings.find(s.funcWriteSingleRegister);
    if (it != end)
    {
        QVariant var = it.value();
        uint8_t v = static_cast<uint8_t>(var.toUInt(&ok));
        if (ok)
            setFuncWriteSingleRegister(v);
    }

    mbCoreDevice::setSettings(settings); // Q_EMIT changed() within
    return true;
}

void mbClientDevice::resetStatisticsInner()
{
    *static_cast<Statistics*>(m_stat) = Statistics();
}

void mbClientDevice::setStatStatusInner(Modbus::StatusCode status, mb::Timestamp_t timestamp, const QString &err)
{
    switch (status)
    {
    case Modbus::Status_BadTcpCreate:
    case Modbus::Status_BadTcpConnect:
    case Modbus::Status_BadUdpCreate:
    case Modbus::Status_BadSerialOpen:
        static_cast<Statistics*>(m_stat)->countBadConnection++;
        break;
    case Modbus::Status_BadSerialReadTimeout:
    case Modbus::Status_BadTcpReadTimeout:
    case Modbus::Status_BadUdpReadTimeout:
        static_cast<Statistics*>(m_stat)->countBadTimeout++;
        break;
    case Modbus::Status_BadCrc:
    case Modbus::Status_BadLrc:
        static_cast<Statistics*>(m_stat)->countBadCRC++;
        break;
    default:
        break;
    }
}
