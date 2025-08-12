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
#include "client_port.h"

#include <QDateTime>

#include <client.h>

#include "client_device.h"

mbClientPort::mbClientPort(QObject *parent) :
    mbCorePort(parent)
{
}

mbClientPort::~mbClientPort()
{
}

QString mbClientPort::extendedName() const
{
    switch (type())
    {
    case Modbus::ASC:
    case Modbus::RTU:
        return QString("%1[%2:%3]").arg(name(), Modbus::toString(type()), serialPortName());
    default:
        return QString("%1[%2:%3:%4]").arg(name(), Modbus::toString(type()), host(), QString::number(port()));
    }
    return name();
}

mbClientDevice *mbClientPort::device(const QString &name) const
{
    Q_FOREACH(mbClientDevice *d, m_devices)
    {
        if (d->name() == name)
            return d;
    }
    return nullptr;
}

int mbClientPort::deviceInsert(mbClientDevice *device, int index)
{
    if (!hasDevice(device))
    {
        if ((index >= 0) && (index < m_devices.count()))
            m_devices.insert(index, device);
        else
        {
            index = m_devices.count();
            m_devices.append(device);
        }
        device->setPort(this);
        Q_EMIT deviceAdded(device);
        return index;
    }
    return -1;
}

int mbClientPort::deviceRemove(int index)
{
    if ((index >= 0) && (index < deviceCount()))
    {
        mbClientDevice* device = deviceAt(index);
        Q_EMIT deviceRemoving(device);
        m_devices.removeAt(index);
        device->setPort(nullptr);
        Q_EMIT deviceRemoved(device);
        return index;
    }
    return -1;
}
