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
    unit    (QStringLiteral("unit")),
    portName(QStringLiteral("portName"))

{
}

const mbClientDevice::Strings &mbClientDevice::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDevice::Defaults::Defaults() :
    mbCoreDevice::Defaults(),
    unit(Modbus::Defaults::instance().unit),
    portName(mbClientPort::Defaults::instance().name)
{
}

const mbClientDevice::Defaults &mbClientDevice::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbClientDevice::mbClientDevice(QObject *parent) :
    mbCoreDevice(parent)
{
    Defaults d = Defaults();

    m_port = nullptr;

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

    r.insert(s.unit    , unit    ());
    r.insert(s.portName, portName());

    return r;
}

bool mbClientDevice::setSettings(const MBSETTINGS &settings)
{
    Strings s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.unit);
    if (it != end)
    {
        QVariant var = it.value();
        uint8_t v = static_cast<uint8_t>(var.toUInt(&ok));
        if (ok)
            setUnit(v);
    }

    it = settings.find(s.portName);
    if (it != end)
    {
        QVariant var = it.value();
        setPortName(var.toString());
    }

    mbCoreDevice::setSettings(settings); // Q_EMIT changed() within
    return true;
}
