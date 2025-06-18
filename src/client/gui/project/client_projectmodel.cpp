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
#include "client_projectmodel.h"

#include <QIcon>

#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>

mbClientProjectModel::mbClientProjectModel(QObject* parent) :
    mbCoreProjectModel (parent)
{
}

int mbClientProjectModel::rowCount(const QModelIndex & parent) const
{
    if (!parent.isValid())
    {
        if (project())
            return project()->portCount();
    }
    else if (mbClientPort *p = port(parent))
        return p->deviceCount();
    return 0;
}

QModelIndex mbClientProjectModel::parent(const QModelIndex &index) const
{
    if (index.isValid() && project())
    {
        mbClientDevice *d = reinterpret_cast<mbClientDevice*>(index.internalPointer());
        if (project()->hasDevice(d))
            return portIndex(d->port());
    }
    return QModelIndex();
}

QModelIndex mbClientProjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!project())
        return QModelIndex();
    if (!parent.isValid())
    {
        mbClientPort *p = project()->port(row);
        if (p)
            return createIndex(row, column, p);
    }
    else if (mbClientPort *p = port(parent))
    {
        mbClientDevice *d = p->device(row);
        if (d)
            return createIndex(row, column, d);
    }
    return QModelIndex();
}

QVariant mbClientProjectModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (mbClientPort *p = port(index))
            return portName(p);
        if (mbClientDevice *d = device(index))
            return deviceName(d);
        break;
    case  Qt::DecorationRole:
        if (mbClientPort *p = port(index))
            return QIcon(":/core/icons/port.png");
        if (mbClientDevice *d = device(index))
            return QIcon(":/core/icons/device.png");
        break;
    }
    return QVariant();
}

mbCorePort *mbClientProjectModel::getPortByIndex(const QModelIndex &index) const
{
    mbClientPort *port = this->port(index);
    if (port)
        return port;
    mbClientDevice *device = this->device(index);
    if (device)
        return device->port();
    return nullptr;
}

mbCoreDevice *mbClientProjectModel::getDeviceByIndex(const QModelIndex &index) const
{
    return device(index);
}

QModelIndex mbClientProjectModel::deviceIndex(mbClientDevice *device) const
{
    mbClientPort *port = device->port();
    int i = port->deviceIndex(device);
    return createIndex(i, 0, port);
}

mbClientDevice *mbClientProjectModel::device(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return reinterpret_cast<mbClientDevice*>(index.internalPointer());
    return nullptr;
}

QString mbClientProjectModel::deviceName(const mbClientDevice *device) const
{
    if (useNameWithSettings())
        return QString("%1[%2]").arg(device->name(), QString::number(device->unit()));
    else
        return device->name();
}

bool mbClientProjectModel::dropDevice(Qt::DropAction action, mbCorePort *sourcePort, int srcIndex, mbCorePort *destPort, int dstIndex)
{
    mbClientPort *srcPort = static_cast<mbClientPort*>(sourcePort);
    mbClientPort *dstPort = static_cast<mbClientPort*>(destPort);
    mbClientDevice *device = srcPort->device(srcIndex);
    if (dstIndex < 0 || dstIndex > dstPort->deviceCount())
        dstIndex = dstPort->deviceCount();
    if (action == Qt::MoveAction && srcPort == dstPort && srcPort->deviceIndex(device) == dstIndex)
        return false; // No move
    if (action == Qt::MoveAction)
    {
        srcPort->deviceRemove(device);
        dstPort->deviceInsert(device, dstIndex);
    }
    else if (action == Qt::CopyAction)
    {
        mbClientDevice *newDevice = new mbClientDevice();
        newDevice->setSettings(device->settings());
        dstPort->deviceInsert(newDevice, dstIndex);
        project()->deviceAdd(newDevice);
    }
    return true;
}

void mbClientProjectModel::portAdd(mbCorePort *port)
{
    int i = project()->portIndex(static_cast<mbClientPort*>(port));
    beginInsertRows(QModelIndex(), i, i);
    connect(static_cast<mbClientPort*>(port), &mbClientPort::deviceAdded   , this, &mbClientProjectModel::deviceAdd    );
    connect(static_cast<mbClientPort*>(port), &mbClientPort::deviceRemoving, this, &mbClientProjectModel::deviceRemove );
    connect(static_cast<mbClientPort*>(port), &mbClientPort::changed       , this, &mbClientProjectModel::portChanged  );
    endInsertRows();
    Q_FOREACH (mbClientDevice* d, static_cast<mbClientPort*>(port)->devices())
        deviceAdd(d);
}

void mbClientProjectModel::portRemove(mbCorePort *port)
{
    Q_FOREACH (mbClientDevice* d, static_cast<mbClientPort*>(port)->devices())
        deviceRemove(d);
    int i = project()->portIndex(static_cast<mbClientPort*>(port));
    beginRemoveRows(QModelIndex(), i, i);
    port->disconnect(this);
    endRemoveRows();
}

void mbClientProjectModel::deviceAdd(mbClientDevice *device)
{
    mbClientPort *port = device->port();
    int i = port->deviceIndex(device);
    beginInsertRows(portIndex(port), i, i);
    connect(device, &mbClientDevice::nameChanged, this, &mbClientProjectModel::deviceChanged);
    endInsertRows();
}

void mbClientProjectModel::deviceRemove(mbClientDevice *device)
{
    mbClientPort *port = device->port();
    int i = port->deviceIndex(device);
    beginRemoveRows(portIndex(port), i, i);
    device->disconnect(this);
    endRemoveRows();
}

void mbClientProjectModel::deviceChanged()
{
    mbClientDevice* d = static_cast<mbClientDevice*>(sender());
    QModelIndex index = deviceIndex(d);
    Q_EMIT dataChanged(index, index);
}
