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
#include "server_port.h"

#include <QDateTime>

#include <ModbusPortTCP.h>
#include <ModbusPortSerial.h>
#include <server.h>

#include "server_deviceref.h"

#define MAX_DISCRETS 1600
#define MAX_REGISTERS 100

mbServerPort::mbServerPort(QObject *parent) :
    mbCorePort(parent)
{
    memset(m_units, 0, sizeof(m_units));
}

mbServerPort::~mbServerPort()
{
}

int mbServerPort::freeDeviceUnit() const
{
    for (int i = 1; i < 255; i++)
    {
        if (m_units[i] == nullptr)
            return i;
    }
    if (m_units[0] == nullptr)
        return 0;
    return -1;
}

mbServerDeviceRef *mbServerPort::deviceRef(mbServerDevice *device) const
{
    Q_FOREACH (mbServerDeviceRef *ref, m_devices)
    {
        if (ref->device() == device)
            return ref;
    }
    return nullptr;
}

int mbServerPort::deviceInsert(mbServerDeviceRef *device, int index)
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
        deviceSetUnits(device, device->units());
        device->setPort(this);
        device->setParent(this);
        Q_EMIT deviceAdded(device);
        return index;
    }
    return -1;
}

int mbServerPort::deviceRemove(int index)
{
    if ((index >= 0) && (index < deviceCount()))
    {
        mbServerDeviceRef *device = deviceAt(index);
        Q_EMIT deviceRemoving(device);
        m_devices.removeAt(index);
        deviceRemoveUnits(device);
        device->setPort(nullptr);
        device->setParent(nullptr);
        Q_EMIT deviceRemoved(device);
        return index;
    }
    return -1;
}

void mbServerPort::deviceRemove(mbServerDevice *device)
{
    for (int i = 0; i < m_devices.count(); )
    {
        mbServerDeviceRef *ref = m_devices.at(i);
        if (device == ref->device())
        {
            deviceRemove(i);
            delete ref;
            continue;
        }
        //--------------------------------------
        i++;
    }
}

QList<quint8> mbServerPort::deviceFilterUnits(mbServerDeviceRef *device, const QList<quint8> &units) const
{
    QList<quint8> res;
    Q_FOREACH (quint8 unit, units)
    {
        mbServerDeviceRef *d = deviceByUnit(unit);
        if (!d || (d == device))
            res.append(unit);
    }
    return res;
}

QList<quint8> mbServerPort::deviceSetUnits(mbServerDeviceRef *device, const QList<quint8> &units)
{
    // TODO: optimize comparasion
    deviceRemoveUnits(device);
    QList<quint8> filtered = deviceFilterUnits(device, units);
    if (filtered.count())
    {
        Q_FOREACH (quint8 unit, filtered)
            m_units[unit] = device;
    }
    else
    {
        int unit = freeDeviceUnit();
        if (unit >= 0)
            filtered.append(static_cast<quint8>(unit));
    }
    device->setUnits(filtered);
    return filtered;
}

mbServerDevice *mbServerPort::device(uint8_t unit) const
{
    mbServerDeviceRef *ref = m_units[unit];
    if (ref)
        return ref->device();
    return nullptr;
}

void mbServerPort::deviceRemoveUnits(mbServerDeviceRef *device)
{
    Q_FOREACH (quint8 unit, device->units())
    {
        mbServerDeviceRef *d = deviceByUnit(unit);
        if (d == device)
            m_units[unit] = nullptr;
    }
}
