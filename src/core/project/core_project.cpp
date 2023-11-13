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
#include "core_project.h"

#include <QFileInfo>

#include "core_port.h"
#include "core_device.h"
#include "core_dataview.h"
#include "core_taskinfo.h"

mbCoreProject::Strings::Strings() :
    name   (QStringLiteral("name")),
    author (QStringLiteral("author")),
    comment(QStringLiteral("comment"))
{
}

const mbCoreProject::Strings &mbCoreProject::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreProject::mbCoreProject(QObject *parent) :
    QObject(parent)
{
}

mbCoreProject::~mbCoreProject()
{
    qDeleteAll(m_ports);
    qDeleteAll(m_devices);
    qDeleteAll(m_dataViews);
}

void mbCoreProject::setAbsoluteFilePath(const QString &file)
{
    QFileInfo fi(file);
    m_absoluteFilePath = fi.absoluteFilePath();
    m_file = fi.fileName();
    m_absoluteDirPath = fi.absolutePath();
}

MBSETTINGS mbCoreProject::settings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS r;
    r.insert(s.name   , name   ());
    r.insert(s.author , author ());
    r.insert(s.comment, comment());
    return r;
}

bool mbCoreProject::setSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();

    it = settings.find(s.name);
    if (it != end)
    {
        QVariant var = it.value();
        setName(var.toString());
    }

    it = settings.find(s.author);
    if (it != end)
    {
        QVariant var = it.value();
        setAuthor(var.toString());
    }

    it = settings.find(s.comment);
    if (it != end)
    {
        QVariant var = it.value();
        setComment(var.toString());
    }

    return true;
}

QString mbCoreProject::freePortName(const QString &s) const
{
    QString ret = s.trimmed();
    if (ret.isEmpty())
        ret = mbCorePort::Defaults::instance().name;
    QRegExp re("^(.*)(\\d+)$");
    QString tn = ret;
    if (ret.contains(re))
        tn = re.cap(1);
    int c = 1;

    while (hasPort(ret))
    {
        ret = tn + QString::number(c);
        c++;
    }
    return ret;
}

int mbCoreProject::portInsert(mbCorePort *port, int index)
{
    if (!hasPort(port))
    {
        QString portName = port->name();
        if (portName.isEmpty() || hasPort(portName))
            port->setName(freePortName(portName));
        if ((index >= 0) && (index < m_ports.count()))
            m_ports.insert(index, port);
        else
        {
            index = m_ports.count();
            m_ports.append(port);
        }
        m_hashPorts.insert(port->name(), port);
        port->setProjectCore(this);
        Q_EMIT portAdded(port);
        return index;
    }
    return -1;
}

int mbCoreProject::portRemove(int index)
{
    if ((index >= 0) && (index < portCount()))
    {
        mbCorePort* d = portCoreAt(index);
        Q_EMIT portRemoving(d);
        m_hashPorts.remove(d->name());
        m_ports.removeAt(index);
        d->setProjectCore(nullptr);
        Q_EMIT portRemoved(d);
        return index;
    }
    return -1;
}

bool mbCoreProject::portRename(mbCorePort *d, const QString &newName)
{
    if (portCore(d->name()) == d && !portCore(newName))
    {
        Q_EMIT portRenaming(d, newName);
        m_hashPorts.remove(d->name());
        m_hashPorts.insert(newName, d);
        return true;
    }
    return false;
}

QString mbCoreProject::freeDeviceName(const QString &s) const
{
    QString ret = s.trimmed();
    if (ret.isEmpty())
        ret = mbCoreDevice::Defaults::instance().name;
    QRegExp re("^(.*)(\\d+)$");
    QString tn = ret;
    if (ret.contains(re))
        tn = re.cap(1);
    int c = 1;

    while (hasDevice(ret))
    {
        ret = tn + QString::number(c);
        c++;
    }
    return ret;
}

int mbCoreProject::deviceInsert(mbCoreDevice *device, int index)
{
    if (!hasDevice(device))
    {
        QString name = device->name();
        if (name.isEmpty() || hasDevice(name))
            device->setName(freeDeviceName(name));
        if ((index >= 0) && (index < m_devices.count()))
            m_devices.insert(index, device);
        else
        {
            index = m_devices.count();
            m_devices.append(device);
        }
        m_hashDevices.insert(device->name(), device);
        device->setProjectCore(this);
        Q_EMIT deviceAdded(device);
        return index;
    }
    return -1;
}

int mbCoreProject::deviceRemove(int index)
{
    if ((index >= 0) && (index < deviceCount()))
    {
        mbCoreDevice* d = deviceCoreAt(index);
        Q_EMIT deviceRemoving(d);
        m_hashDevices.remove(d->name());
        m_devices.removeAt(index);
        d->setProjectCore(nullptr);
        Q_EMIT deviceRemoved(d);
        return index;
    }
    return -1;
}

bool mbCoreProject::deviceRename(mbCoreDevice *d, const QString &newName)
{
    if (deviceCore(d->name()) == d && !deviceCore(newName))
    {
        Q_EMIT deviceRenaming(d, newName);
        m_hashDevices.remove(d->name());
        m_hashDevices.insert(newName, d);
        return true;
    }
    return false;
}

QString mbCoreProject::freeDataViewName(const QString &s) const
{
    QString ret = s.trimmed();
    if (ret.isEmpty())
        ret = mbCoreDataView::Defaults::instance().name;
    QRegExp re("^(.*)(\\d+)$");
    QString tn = ret;
    if (ret.contains(re))
        tn = re.cap(1);
    int c = 1;

    while (hasDataView(ret))
    {
        ret = tn + QString::number(c);
        c++;
    }
    return ret;
}

int mbCoreProject::dataViewInsert(mbCoreDataView *dataView, int index)
{
    if (!hasDataView(dataView))
    {
        QString name = dataView->name().trimmed();
        if (hasDataView(name) || name.isEmpty())
            dataView->setName(freeDataViewName(name));
        if ((index >= 0) && (index < m_dataViews.count()))
            m_dataViews.insert(index, dataView);
        else
        {
            index = m_dataViews.count();
            m_dataViews.append(dataView);
        }
        m_hashDataViews.insert(dataView->name(), dataView);
        dataView->setProjectCore(this);
        Q_EMIT dataViewAdded(dataView);
        return index;
    }
    return -1;
}

int mbCoreProject::dataViewRemove(int index)
{
    if ((index >= 0) && (index < dataViewCount()))
    {
        mbCoreDataView *dataView = dataViewCoreAt(index);
        Q_EMIT dataViewRemoving(dataView);
        m_hashDataViews.remove(dataView->name());
        m_dataViews.removeAt(index);
        dataView->setProjectCore(nullptr);
        Q_EMIT dataViewRemoved(dataView);
        return index;
    }
    return -1;
}

bool mbCoreProject::dataViewRename(mbCoreDataView *wl, const QString &newName)
{
    if (dataViewCore(wl->name()) == wl && !dataViewCore(newName))
    {
        Q_EMIT dataViewRenaming(wl, newName);
        m_hashDataViews.remove(wl->name());
        m_hashDataViews.insert(newName, wl);
        return true;
    }
    return false;
}

int mbCoreProject::taskInsert(mbCoreTaskInfo *task, int index)
{
    if (!hasTask(task))
    {
        task->setName(freeTaskName(task->name()));
        if ((index >= 0) && (index < taskCount()))
            m_tasks.insert(index, task);
        else
        {
            index = m_tasks.count();
            m_tasks.append(task);
        }
        m_hashTasks.insert(task->name(), task);
        task->setProject(this);
        Q_EMIT taskAdded(task);
        return index;
    }
    return -1;
}

int mbCoreProject::taskRemove(int index)
{
    if ((index >= 0) && (index < taskCount()))
    {
        mbCoreTaskInfo *task = taskAt(index);
        Q_EMIT taskRemoving(task);
        task->setProject(nullptr);
        m_tasks.remove(index);
        m_hashTasks.remove(task->name());
        Q_EMIT taskRemoved(task);
        return index;
    }
    return -1;
}

bool mbCoreProject::taskRename(mbCoreTaskInfo *t, const QString &newName)
{
    if (task(t->name()) == t && !task(newName))
    {
        Q_EMIT taskRenaming(t, newName);
        m_hashTasks.remove(t->name());
        m_hashTasks.insert(newName, t);
        return true;
    }
    return false;
}

QString mbCoreProject::freeTaskName(const QString &s)
{
    QString ret = s.trimmed();
    if (ret.isEmpty())
        ret = "dev";
    QRegExp re("^(.*)(\\d+)$");
    QString tn = ret;
    if (ret.contains(re))
        tn = re.cap(1);
    int c = 1;

    while (hasTask(ret))
    {
        ret = tn + QString::number(c);
        c++;
    }
    return ret;
}
