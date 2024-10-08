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
#include "server_windowmanager.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include "server_ui.h"
#include "device/server_devicemanager.h"
#include "device/server_deviceui.h"
#include "dataview/server_dataviewmanager.h"

mbServerWindowManager::Strings::Strings() : mbCoreWindowManager::Strings(),
    prefixDevice(QStringLiteral("dev:"))
{
}

const mbServerWindowManager::Strings &mbServerWindowManager::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerWindowManager::mbServerWindowManager(mbServerUi *ui, mbServerDeviceManager *deviceManager, mbServerDataViewManager *dataViewManager) :
    mbCoreWindowManager(ui, dataViewManager)
{
    m_deviceManager = deviceManager;
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiAdd, this, &mbServerWindowManager::deviceUiAdd);
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiRemove, this, &mbServerWindowManager::deviceUiRemove);
    Q_FOREACH (mbServerDeviceUi *ui, m_deviceManager->deviceUis())
        deviceUiAdd(ui);
}

QMdiSubWindow *mbServerWindowManager::getMdiSubWindowForNameWithPrefix(const QString &nameWithPrefix) const
{
    const Strings &s = Strings::instance();
    if (nameWithPrefix.startsWith(s.prefixDevice))
    {
        mbServerDeviceUi *ui = m_deviceManager->deviceUi(nameWithPrefix.mid(s.prefixDevice.size()));
        return m_hashWindows.value(ui);
    }
    return mbCoreWindowManager::getMdiSubWindowForNameWithPrefix(nameWithPrefix);
}

QString mbServerWindowManager::getMdiSubWindowNameWithPrefix(const QMdiSubWindow *sw) const
{
    if (mbServerDeviceUi *ui = qobject_cast<mbServerDeviceUi *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        return s.prefixDataView+ui->name();
    }
    return mbCoreWindowManager::getMdiSubWindowNameWithPrefix(sw);
}

mbServerDevice *mbServerWindowManager::activeDevice() const
{
    return m_deviceManager->activeDevice();
}

void mbServerWindowManager::setActiveDevice(mbServerDevice *device)
{
    mbServerDeviceUi *ui = m_deviceManager->deviceUi(device);
    if (ui)
    {
        QMdiSubWindow *sw = m_hashWindows.value(ui);
        if (sw)
            m_area->setActiveSubWindow(sw);
    }
}

void mbServerWindowManager::showDeviceUi(const mbServerDeviceUi *ui)
{
    showSubWindow(ui);
}

void mbServerWindowManager::actionWindowDeviceShowAll()
{
    Q_FOREACH(QMdiSubWindow *sw, m_devices)
    {
        sw->show();
        sw->widget()->show();
    }
}

void mbServerWindowManager::actionWindowDeviceShowActive()
{
    mbServerDeviceUi *ui = m_deviceManager->activeDeviceUi();
    if (ui)
        showSubWindow(ui);
}

void mbServerWindowManager::actionWindowDeviceCloseAll()
{
    Q_FOREACH(QMdiSubWindow *sw, m_devices)
    {
        sw->close();
    }
}

void mbServerWindowManager::actionWindowDeviceCloseActive()
{
    mbServerDeviceUi *ui = m_deviceManager->activeDeviceUi();
    if (ui)
        closeSubWindow(ui);
}

void mbServerWindowManager::actionWindowShowAll()
{
    actionWindowDeviceShowAll();
    actionWindowDataViewShowAll();
}

void mbServerWindowManager::actionWindowCloseAll()
{
    actionWindowDeviceCloseAll();
    mbCoreWindowManager::actionWindowCloseAll();
}

void mbServerWindowManager::deviceUiAdd(mbServerDeviceUi *ui)
{
    QMdiSubWindow* sw = new QMdiSubWindow(m_area);
    sw->setWidget(ui);
    //sw->setAttribute(Qt::WA_DeleteOnClose, false);
    m_devices.append(sw);
    m_hashWindows.insert(ui, sw);
    connect(ui, &mbServerDeviceUi::nameChanged, sw, &QWidget::setWindowTitle);
    sw->setWindowTitle(ui->name());
    m_area->addSubWindow(sw);
    sw->show();
}

void mbServerWindowManager::deviceUiRemove(mbServerDeviceUi *ui)
{
    ui->disconnect(this);
    QMdiSubWindow* sw = m_hashWindows.value(ui, nullptr);
    if (sw)
    {
        m_devices.removeOne(sw);
        m_hashWindows.remove(ui);
        m_area->removeSubWindow(sw);
        sw->setWidget(nullptr);
        ui->setParent(nullptr);
        delete sw;
    }
}

void mbServerWindowManager::subWindowActivated(QMdiSubWindow *sw)
{
    if (sw)
    {
        QWidget *w = sw->widget();
        if (mbServerDeviceUi *ui = qobject_cast<mbServerDeviceUi*>(w))
        {
            ui->blockSignals(true);
            m_deviceManager->setActiveDeviceUi(ui);
            ui->blockSignals(false);
            return;
        }
        else
            mbCoreWindowManager::subWindowActivated(sw);
    }
}
