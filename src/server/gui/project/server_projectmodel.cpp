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
#include "server_projectmodel.h"

#include <project/server_project.h>
#include <project/server_port.h>
#include <project/server_deviceref.h>

mbServerProjectModel::mbServerProjectModel(QObject* parent) :
    mbCoreProjectModel (parent)
{
}

int mbServerProjectModel::rowCount(const QModelIndex & parent) const
{
    if (!parent.isValid())
    {
        if (project())
            return project()->portCount();
    }
    if (mbServerPort *p = port(parent))
        return p->deviceCount();
    return 0;
}

QModelIndex mbServerProjectModel::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        mbServerDeviceRef *d = reinterpret_cast<mbServerDeviceRef*>(index.internalPointer());
        //if (m_project->hasDevice(d))
        if (d)
            return portIndex(d->port());
    }
    return QModelIndex();
}

QModelIndex mbServerProjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!project())
        return QModelIndex();
    if (!parent.isValid())
    {
        mbServerPort *p = project()->port(row);
        if (p)
            return createIndex(row, column, p);
    }
    else if (mbServerPort *p = port(parent))
    {
        mbServerDeviceRef *d = p->device(row);
        if (d)
            return createIndex(row, column, d);
    }
    return QModelIndex();
}

QVariant mbServerProjectModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (mbServerPort *p = port(index))
            return portName(p);
        if (mbServerDeviceRef *d = deviceRef(index))
            return deviceName(d);
    }
    return QVariant();
}

mbCorePort *mbServerProjectModel::getPortByIndex(const QModelIndex &index) const
{
    mbServerPort *port = this->port(index);
    if (port)
        return port;
    mbServerDeviceRef *device = this->deviceRef(index);
    if (device)
        return device->port();
    return nullptr;
}

mbCoreDevice *mbServerProjectModel::getDeviceByIndex(const QModelIndex &index) const
{
    mbServerDeviceRef *deviceRef = this->deviceRef(index);
    if (deviceRef)
        return deviceRef->device();
    return nullptr;
}

QModelIndex mbServerProjectModel::deviceIndex(mbServerDeviceRef *device) const
{
    mbServerPort *port = device->port();
    int i = port->deviceIndex(device);
    return createIndex(i, 0, port);
}

mbServerDeviceRef *mbServerProjectModel::deviceRef(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return reinterpret_cast<mbServerDeviceRef*>(index.internalPointer());
    return nullptr;
}

QString mbServerProjectModel::deviceName(const mbServerDeviceRef *device) const
{
    if (useNameWithSettings())
        return QString("%1[%2]").arg(device->name(), device->unitsStr());
    else
        return device->name();
}

void mbServerProjectModel::portAdd(mbCorePort *port)
{
    int i = m_project->portIndex(static_cast<mbServerPort*>(port));
    beginInsertRows(QModelIndex(), i, i);
    connect(static_cast<mbServerPort*>(port), &mbServerPort::deviceAdded   , this, &mbServerProjectModel::deviceAdd    );
    connect(static_cast<mbServerPort*>(port), &mbServerPort::deviceRemoving, this, &mbServerProjectModel::deviceRemove );
    connect(static_cast<mbServerPort*>(port), &mbServerPort::changed       , this, &mbServerProjectModel::portChanged  );
    endInsertRows();
    Q_FOREACH (mbServerDeviceRef* d, static_cast<mbServerPort*>(port)->devices())
        deviceAdd(d);
}

void mbServerProjectModel::portRemove(mbCorePort *port)
{
    Q_FOREACH (mbServerDeviceRef* d, static_cast<mbServerPort*>(port)->devices())
        deviceRemove(d);
    int i = m_project->portIndex(port);
    beginRemoveRows(QModelIndex(), i, i);
    port->disconnect(this);
    endRemoveRows();
}

void mbServerProjectModel::deviceAdd(mbServerDeviceRef *device)
{
    mbServerPort *port = device->port();
    int i = port->deviceIndex(device);
    beginInsertRows(portIndex(port), i, i);
    connect(device, &mbServerDeviceRef::nameChanged, this, &mbServerProjectModel::deviceChanged);
    endInsertRows();
}

void mbServerProjectModel::deviceRemove(mbServerDeviceRef *device)
{
    mbServerPort *port = device->port();
    int i = port->deviceIndex(device);
    beginRemoveRows(portIndex(port), i, i);
    device->disconnect(this);
    endRemoveRows();
}

void mbServerProjectModel::deviceChanged()
{
    mbServerDeviceRef* d = static_cast<mbServerDeviceRef*>(sender());
    QModelIndex index = deviceIndex(d);
    Q_EMIT dataChanged(index, index);
}
