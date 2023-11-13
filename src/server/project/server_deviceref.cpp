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
#include "server_deviceref.h"

#include "server_port.h"
#include "server_device.h"

mbServerDeviceRef::Strings::Strings() : mbServerDevice::Strings(),
    units(QStringLiteral("units"))
{
}

const mbServerDeviceRef::Strings &mbServerDeviceRef::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDeviceRef::Defaults::Defaults() : mbServerDevice::Defaults(),
    units(QStringLiteral("0-10,255"))
{
}

const mbServerDeviceRef::Defaults &mbServerDeviceRef::Defaults::instance()
{
    static const Defaults s;
    return s;
}

mbServerDeviceRef::mbServerDeviceRef(mbServerDevice *device, QObject *parent) :
    QObject(parent),
    m_device(device)
{
    const Defaults &d = Defaults::instance();

    m_port = nullptr;
    setUnitsStr(d.units);

    connect(m_device, &mbServerDevice::nameChanged, this, &mbServerDeviceRef::setName);
}

void mbServerDeviceRef::setPort(mbServerPort *port)
{
    m_port = port;
}

void mbServerDeviceRef::setUnits(const QList<quint8> &units)
{
    // TODO: optimize comparasion
    if (m_port)
    {
        QSet<quint8> su(units.begin(), units.end());
        QList<quint8> lu = su.values();
        std::sort(lu.begin(), lu.end());
        mbServerPort *p = m_port;
        m_port = nullptr;
        lu = p->deviceSetUnits(this, lu);
        m_port = p;
    }
    else
    {
        m_units = units;
        Q_EMIT changed();
    }
}

void mbServerDeviceRef::addUnits(const QList<quint8> &units)
{
    QList<quint8> ls = m_units + units;
    setUnits(ls);
}

QString mbServerDeviceRef::unitsStr() const
{
    return mb::toUnitsString(m_units);
}

void mbServerDeviceRef::setUnitsStr(const QString &units)
{
    bool ok;
    QList<quint8> lu = mb::toUnitsList(units, &ok);
    if (ok)
        setUnits(lu);
}

void mbServerDeviceRef::addUnitsStr(const QString &units)
{
    bool ok;
    QList<quint8> lu = mb::toUnitsList(units, &ok);
    if (ok)
        addUnits(lu);
}

Modbus::Settings mbServerDeviceRef::settings() const
{
    const Strings &s = Strings::instance();

    Modbus::Settings r = m_device->settings();
    r[s.units] = unitsStr();
    return r;
}

bool mbServerDeviceRef::setSettings(const Modbus::Settings &settings)
{
    const Strings &s = Strings::instance();

    Modbus::Settings::const_iterator it;
    Modbus::Settings::const_iterator end = settings.end();

    it = settings.find(s.units);
    if (it != end)
    {
        QVariant var = it.value();
        setUnitsStr(var.toString());
    }
    return m_device->setSettings(settings);
}

void mbServerDeviceRef::setName(const QString &name)
{
    setObjectName(name);
    Q_EMIT nameChanged(name);
}
