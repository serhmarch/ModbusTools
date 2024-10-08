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
#include "server_devicemanager.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include <server.h>

#include "server_deviceui.h"
#include <project/server_project.h>
#include <project/server_device.h>

mbServerDeviceManager::mbServerDeviceManager(QObject *parent) : QObject(parent)
{
    m_project = nullptr;
    m_activeDeviceUi = nullptr;

    mbServer *core = mbServer::global();
    connect(core, &mbServer::projectChanged, this, &mbServerDeviceManager::setProject);
    setProject(core->project());
}

mbServerDevice *mbServerDeviceManager::activeDevice() const
{
    mbServerDeviceUi *ui = activeDeviceUi();
    if (ui)
        return ui->device();
    return nullptr;
}

mbServerDeviceUi *mbServerDeviceManager::deviceUi(const QString &name) const
{
    if (m_project)
    {
        mbServerDevice *d = m_project->device(name);
        return m_hashDeviceUis.value(d, nullptr);
    }
    return nullptr;
}

void mbServerDeviceManager::setActiveDeviceUi(mbServerDeviceUi *ui)
{
    // TODO: ASSERT m_hashDeviceUis.contains(ui->device())
    if (m_activeDeviceUi != ui)
    {
        m_activeDeviceUi = ui;
        Q_EMIT deviceUiActivated(ui);
    }
}

void mbServerDeviceManager::setProject(mbCoreProject *p)
{
    mbServerProject *project = static_cast<mbServerProject*>(p);
    if (m_project)
    {
        Q_FOREACH (mbServerDevice* d, m_project->devices())
            deviceRemove(d);
        m_project->disconnect(this);
        m_activeDeviceUi = nullptr;
    }
    m_project = project;
    if (project)
    {
        connect(project, &mbServerProject::deviceAdded   , this, &mbServerDeviceManager::deviceAdd);
        connect(project, &mbServerProject::deviceRemoving, this, &mbServerDeviceManager::deviceRemove);
        Q_FOREACH (mbServerDevice* d, project->devices())
            deviceAdd(d);
    }
}

void mbServerDeviceManager::deviceAdd(mbCoreDevice *device)
{
    // TODO: ASSERT !m_hashDeviceUis.contains(device)
    mbServerDeviceUi *ui = new mbServerDeviceUi(static_cast<mbServerDevice*>(device));
    m_deviceUis.append(ui);
    m_hashDeviceUis.insert(static_cast<mbServerDevice*>(device), ui);
    connect(ui, &mbServerDeviceUi::customContextMenuRequested, this, &mbServerDeviceManager::deviceContextMenu);
    Q_EMIT deviceUiAdd(ui);
}

void mbServerDeviceManager::deviceRemove(mbCoreDevice *device)
{
    // TODO: ASSERT ui != nullptr && m_hashDeviceUis.contains(ui->device())
    mbServerDeviceUi *ui = deviceUi(static_cast<mbServerDevice*>(device));
    if (m_activeDeviceUi == ui)
        m_activeDeviceUi = nullptr;
    ui->disconnect(this);
    Q_EMIT deviceUiRemove(ui);
    m_deviceUis.removeOne(ui);
    m_hashDeviceUis.remove(static_cast<mbServerDevice*>(device));
    ui->deleteLater(); // Note: need because 'ui' can have 'QMenu'-children located in stack which is trying to delete
}

void mbServerDeviceManager::deviceContextMenu(const QPoint & /*pos*/)
{
    mbServerDeviceUi *ui = qobject_cast<mbServerDeviceUi*>(sender());
    Q_EMIT deviceUiContextMenu(ui);
}
