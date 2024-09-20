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
#include "client_runtime.h"

#include <QCoreApplication>

#include <client.h>

#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>
#include <project/client_dataview.h>

#include "client_rundevice.h"
#include "client_runitem.h"
#include "client_runmessage.h"
#include "client_runthread.h"

mbClientRuntime::mbClientRuntime(QObject *parent)
    : mbCoreRuntime{parent}
{
}

void mbClientRuntime::createComponents()
{
    const mb::StatusCode status = mb::Status_MbInitializing;
    const mb::Timestamp_t timestamp = mb::currentTimestamp();

    QHash<mbClientDevice*, QList<mbClientDataViewItem*> > hashDevices;
    Q_FOREACH (mbClientDataView *wl, project()->dataViews())
    {
        Q_FOREACH (mbClientDataViewItem *item, wl->items())
        {
            if (item->device())
                hashDevices[item->device()].append(item);
        }
    }

    Q_FOREACH (mbClientPort *port, project()->ports())
    {
        QList<mbClientRunDevice*> runDevices;
        Q_FOREACH (mbClientDevice *device, port->devices())
        {
            const QList<mbClientDataViewItem*> &items = hashDevices[device];
            QList<mbClientRunItem*> runItems;
            Q_FOREACH (mbClientDataViewItem *item, items)
            {
                item->update(status, timestamp);
                mbClientRunItem *ri = createRunItem(item);
                runItems.append(ri);
            }
            mbClientRunDevice *rd = createRunDevice(device);
            rd->pushItemsToRead(runItems);
            runDevices.append(rd);
        }
        mbClientRunThread *rp = createRunThread(port);
        rp->pushDevices(runDevices);
    }
}

void mbClientRuntime::startComponents()
{
    mbCoreRuntime::startComponents();
    Q_FOREACH (mbClientRunThread *t, m_threads)
        t->start();
}

void mbClientRuntime::beginStopComponents()
{
    mbCoreRuntime::beginStopComponents();
    Q_FOREACH (mbClientRunThread *t, m_threads)
        t->stop();
}

bool mbClientRuntime::tryStopComponents()
{
    if (!mbCoreRuntime::tryStopComponents())
        return false;
    Q_FOREACH (mbClientRunThread *t, m_threads)
    {
        if (t->isRunning())
            return false;
    }
    return true;
}

void mbClientRuntime::clearComponents()
{
    const mb::StatusCode status = mb::Status_MbStopped;
    const mb::Timestamp_t timestamp = mb::currentTimestamp();
    QList<mbClientDataViewItem*> items = m_items.keys();
    Q_FOREACH (mbClientDataViewItem *item, items)
    {
        item->update(status, timestamp);
    }

    qDeleteAll(m_items);
    m_items.clear();

    qDeleteAll(m_devices);
    m_devices.clear();

    qDeleteAll(m_threads);
    m_threads.clear();
}

void mbClientRuntime::sendMessage(mb::Client::DeviceHandle_t handle, const mbClientRunMessagePtr &message)
{
    mbClientRunDevice *rd = m_devices.value(handle);
    if (rd)
    {
        rd->pushExternalMessage(message);
    }
    else
    {
        message->setComplete(Modbus::Status_Bad, mb::currentTimestamp());
    }
}

void mbClientRuntime::updateItem(mb::Client::ItemHandle_t handle, const QByteArray &data, mb::StatusCode status, mb::Timestamp_t timestamp)
{
    if (!m_items.contains(handle))
        return;
    mbClientDataViewItem *item = handle;
    item->update(data, status, timestamp);
}

void mbClientRuntime::writeItemData(mb::Client::ItemHandle_t handle, const QByteArray &data)
{
    if (!m_items.contains(handle))
        return;
    mbClientDataViewItem *item = handle;
    mbClientDevice *d = item->device();
    mbClientRunDevice *rd = m_devices.value(d);
    if (rd)
    {
        mbClientRunItem *ri = createRunItem(item, data);
        rd->pushItemToWrite(ri);
    }
}

mbClientRunItem *mbClientRuntime::createRunItem(mbClientDataViewItem *item)
{
    mbClientRunItem *t = new mbClientRunItem(item->handle(),
                                             item->addressType(),
                                             item->addressOffset(),
                                             item->count(),
                                             item->period(),
                                             item->sizeOf());
    m_items.insert(item, t);
    return t;
}

mbClientRunItem *mbClientRuntime::createRunItem(mbClientDataViewItem *item, const QByteArray &data)
{
    mbClientRunItem *t = new mbClientRunItem(item->handle(),
                                             item->addressType(),
                                             item->addressOffset(),
                                             item->count(),
                                             data);
    return t;
}

mbClientRunDevice *mbClientRuntime::createRunDevice(mbClientDevice *device)
{
    mbClientRunDevice *t = new mbClientRunDevice(device->settings());
    m_devices.insert(device, t);
    return t;
}

mbClientRunThread *mbClientRuntime::createRunThread(mbClientPort *port)
{
    mbClientRunThread *t = new mbClientRunThread(port);
    m_threads.insert(port, t);
    return t;
}
