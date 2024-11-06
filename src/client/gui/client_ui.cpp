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
#include "client_ui.h"
#include "ui_client_ui.h"

#include <QUndoStack>
#include <QBuffer>
#include <QClipboard>
#include <QLabel>
#include <QMessageBox>

#include <client.h>

#include <project/client_builder.h>
#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>
#include <project/client_dataview.h>

#include "dialogs/client_dialogs.h"
#include "dialogs/client_dialogdevice.h"

#include "project/client_projectui.h"
#include "dataview/client_dataviewmanager.h"

#include "client_windowmanager.h"

#include "sendmessage/client_sendmessageui.h"
#include "scanner/client_scannerui.h"

mbClientUi::mbClientUi(mbClient *core, QWidget *parent) :
    mbCoreUi (core, parent),
    ui(new Ui::mbClientUi)
{
    ui->setupUi(this);

    m_helpFile = QStringLiteral("/help/ModbusClient.qhc");

    m_ui.menuFile                        = ui->menuFile                       ;
    m_ui.menuEdit                        = ui->menuEdit                       ;
    m_ui.menuView                        = ui->menuView                       ;
    m_ui.menuPort                        = ui->menuPort                       ;
    m_ui.menuDevice                      = ui->menuDevice                     ;
    m_ui.menuDataView                    = ui->menuDataView                   ;
    m_ui.menuTools                       = ui->menuTools                      ;
    m_ui.menuRuntime                     = ui->menuRuntime                    ;
    m_ui.menuWindow                      = ui->menuWindow                     ;
    m_ui.menuHelp                        = ui->menuHelp                       ;
    m_ui.actionFileNew                   = ui->actionFileNew                  ;
    m_ui.actionFileOpen                  = ui->actionFileOpen                 ;
    m_ui.actionFileSave                  = ui->actionFileSave                 ;
    m_ui.actionFileSaveAs                = ui->actionFileSaveAs               ;
    m_ui.actionFileEdit                  = ui->actionFileEdit                 ;
    m_ui.actionFileInfo                  = ui->actionFileInfo                 ;
    m_ui.actionFileQuit                  = ui->actionFileQuit                 ;
    m_ui.actionEditCut                   = ui->actionEditCut                  ;
    m_ui.actionEditCopy                  = ui->actionEditCopy                 ;
    m_ui.actionEditPaste                 = ui->actionEditPaste                ;
    m_ui.actionEditInsert                = ui->actionEditInsert               ;
    m_ui.actionEditEdit                  = ui->actionEditEdit                 ;
    m_ui.actionEditDelete                = ui->actionEditDelete               ;
    m_ui.actionEditSelectAll             = ui->actionEditSelectAll            ;
    m_ui.actionViewProject               = ui->actionViewProject              ;
    m_ui.actionViewLogView               = ui->actionViewLogView              ;
    m_ui.actionPortNew                   = ui->actionPortNew                  ;
    m_ui.actionPortEdit                  = ui->actionPortEdit                 ;
    m_ui.actionPortDelete                = ui->actionPortDelete               ;
    m_ui.actionPortImport                = ui->actionPortImport               ;
    m_ui.actionPortExport                = ui->actionPortExport               ;
    m_ui.actionDeviceNew                 = ui->actionDeviceNew                ;
    m_ui.actionDeviceEdit                = ui->actionDeviceEdit               ;
    m_ui.actionDeviceDelete              = ui->actionDeviceDelete             ;
    m_ui.actionDeviceImport              = ui->actionDeviceImport             ;
    m_ui.actionDeviceExport              = ui->actionDeviceExport             ;
    m_ui.actionDataViewItemNew           = ui->actionDataViewItemNew          ;
    m_ui.actionDataViewItemEdit          = ui->actionDataViewItemEdit         ;
    m_ui.actionDataViewItemInsert        = ui->actionDataViewItemInsert       ;
    m_ui.actionDataViewItemDelete        = ui->actionDataViewItemDelete       ;
    m_ui.actionDataViewImportItems       = ui->actionDataViewImportItems      ;
    m_ui.actionDataViewExportItems       = ui->actionDataViewExportItems      ;
    m_ui.actionDataViewNew               = ui->actionDataViewNew              ;
    m_ui.actionDataViewEdit              = ui->actionDataViewEdit             ;
    m_ui.actionDataViewInsert            = ui->actionDataViewInsert           ;
    m_ui.actionDataViewDelete            = ui->actionDataViewDelete           ;
    m_ui.actionDataViewImport            = ui->actionDataViewImport           ;
    m_ui.actionDataViewExport            = ui->actionDataViewExport           ;
    m_ui.actionWindowShowAll             = ui->actionWindowShowAll            ;
    m_ui.actionWindowShowActive          = ui->actionWindowShowActive         ;
    m_ui.actionWindowCloseAll            = ui->actionWindowCloseAll           ;
    m_ui.actionWindowCloseActive         = ui->actionWindowCloseActive        ;
    m_ui.actionWindowCascade             = ui->actionWindowCascade            ;
    m_ui.actionWindowTile                = ui->actionWindowTile               ;
    m_ui.actionHelpAbout                 = ui->actionHelpAbout                ;
    m_ui.actionHelpAboutQt               = ui->actionHelpAboutQt              ;
    m_ui.actionHelpContents              = ui->actionHelpContents             ;
    m_ui.actionToolsSettings             = ui->actionToolsSettings            ;
    m_ui.actionRuntimeStartStop          = ui->actionRuntimeStartStop         ;
    m_ui.dockProject                     = ui->dockProject                    ;
    m_ui.dockLogView                     = ui->dockLogView                    ;
    m_ui.statusbar                       = ui->statusbar                      ;
}

mbClientUi::~mbClientUi()
{
    delete ui;
}

void mbClientUi::initialize()
{
    // Dialogs
    m_dialogs = new mbClientDialogs(this);

    // Project Ui
    m_projectUi = new mbClientProjectUi(this);
    connect(projectUi(), &mbClientProjectUi::deviceDoubleClick, this, &mbClientUi::menuSlotDeviceEdit);
    connect(projectUi(), &mbClientProjectUi::deviceContextMenu, this, &mbClientUi::contextMenuDevice );

    m_dataViewManager = new mbClientDataViewManager(this);

    m_windowManager = new mbClientWindowManager(this, dataViewManager());

    // Menu Port
    connect(ui->actionPortNewDevice      , &QAction::triggered, this, &mbClientUi::menuSlotPortNewDevice     );
    connect(ui->actionPortClearAllDevices, &QAction::triggered, this, &mbClientUi::menuSlotPortClearAllDevice);

    // Menu Tools
    connect(ui->actionToolsSendMessage, &QAction::triggered, this, &mbClientUi::menuSlotToolsSendMessage);
    connect(ui->actionToolsScanner    , &QAction::triggered, this, &mbClientUi::menuSlotToolsScanner    );

    m_sendMessageUi = new mbClientSendMessageUi(this);
    m_scannerUi     = new mbClientScannerUi(this);

    mbCoreUi::initialize();
}

MBSETTINGS mbClientUi::cachedSettings() const
{
    MBSETTINGS m = mbCoreUi::cachedSettings();
    mb::unite(m, m_sendMessageUi->cachedSettings());
    mb::unite(m, m_scannerUi->cachedSettings());
    return m;
}

void mbClientUi::setCachedSettings(const MBSETTINGS &settings)
{
    mbCoreUi::setCachedSettings(settings);
    m_sendMessageUi->setCachedSettings(settings);
    m_scannerUi->setCachedSettings(settings);
}

void mbClientUi::menuSlotViewProject()
{
    ui->dockProject->show();
}

void mbClientUi::menuSlotViewLogView()
{
    ui->dockLogView->show();
}

void mbClientUi::menuSlotEditPaste()
{
    if (core()->isRunning())
        return;
    mbCoreUi::menuSlotEditPaste();
}

void mbClientUi::menuSlotEditInsert()
{
    if (core()->isRunning())
        return;
    mbCoreUi::menuSlotEditInsert();
}

void mbClientUi::menuSlotEditEdit()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == ui->dockProject || ui->dockProject->isAncestorOf(focus))
        {
            if (m_projectUi->selectedDeviceCore())
            {
                menuSlotDeviceEdit();
                return;
            }
            if (m_projectUi->selectedPortCore())
            {
                menuSlotPortEdit();
                return;
            }
        }
    }
    mbCoreUi::menuSlotEditEdit();
}

void mbClientUi::menuSlotEditDelete()
{
    if (core()->isRunning())
        return;
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == ui->dockProject || ui->dockProject->isAncestorOf(focus))
        {
            if (m_projectUi->selectedDeviceCore())
            {
                menuSlotDeviceDelete();
                return;
            }
            if (m_projectUi->selectedPortCore())
            {
                menuSlotPortDelete();
                return;
            }
        }
    }
    mbCoreUi::menuSlotEditDelete();
}

void mbClientUi::menuSlotPortNew()
{
    if (core()->isRunning())
        return;
    mbClientProject* project = core()->project();
    if (project)
    {
        MBSETTINGS s = dialogs()->getPort(MBSETTINGS(), QStringLiteral("New Port"));
        if (s.count())
        {
            mbClientPort* e = new mbClientPort;
            e->setSettings(s);
            project->portAdd(e);
        }
    }
}

void mbClientUi::menuSlotPortEdit()
{
    if (core()->isRunning())
        return;
    mbClientPort *port = projectUi()->currentPort();
    if (port)
        editPort(port);
}

void mbClientUi::menuSlotPortDelete()
{
    if (core()->isRunning())
        return;
    mbClientProject* project = core()->project();
    if (project)
    {
        mbClientPort *port = projectUi()->currentPort();
        if (port)
        {
            if (port->deviceCount())
            {
                QMessageBox::information(this,
                                         QStringLiteral("Delete Port"),
                                         QString("Can't delete '%1' because it has a device(s)!").arg(port->name()),
                                         QMessageBox::Ok);
                return;
            }
            QMessageBox::StandardButton res = QMessageBox::question(this,
                                                                    QStringLiteral("Delete Port"),
                                                                    QString("Are you sure you want to delete '%1'?").arg(port->name()),
                                                                    QMessageBox::Yes|QMessageBox::No);
            if (res == QMessageBox::Yes)
            {
                project->portRemove(port);
                delete port;
            }
        }
    }
}

void mbClientUi::menuSlotPortNewDevice()
{
    if (core()->isRunning())
        return;
    mbClientPort *port = projectUi()->currentPort();
    if (!port)
        return;
    MBSETTINGS o;
    o[mbClientDialogDevice::Strings::instance().createDeviceForPort] = port->name();
    MBSETTINGS s = dialogs()->getDevice(o, QStringLiteral("New Device"));
    if (s.count())
    {
        mbClientProject* project = core()->project();
        mbClientDevice* d = new mbClientDevice;
        d->setSettings(s);
        port->deviceAdd(d);
        project->deviceAdd(d);
    }
}

void mbClientUi::menuSlotPortClearAllDevice()
{
    if (core()->isRunning())
        return;
    if (mbClientPort *port = projectUi()->currentPort())
    {
        QList<mbClientDevice*> devices = port->devices();
        if (devices.count())
        {
            QMessageBox::StandardButton res = QMessageBox::question(this,
                                                                    QStringLiteral("Clear All Devices"),
                                                                    QString("Are you sure you want to clear all devices for port '%1'?").arg(port->name()),
                                                                    QMessageBox::Yes|QMessageBox::No);
            if (res == QMessageBox::Yes)
            {

                Q_FOREACH(mbClientDevice* d, devices)
                    port->deviceRemove(d);
                mbClientProject* project = core()->project();
                Q_FOREACH(mbClientDevice* d, devices)
                    project->deviceRemove(d);
                qDeleteAll(devices);
            }
        }
    }
}

void mbClientUi::menuSlotDeviceNew()
{
    if (core()->isRunning())
        return;
    mbClientProject* project = core()->project();
    if (project)
    {
        MBSETTINGS s = dialogs()->getDevice(MBSETTINGS(), QStringLiteral("New Device"));
        if (s.count())
        {
            mbClientPort *port;
            QVariant var = s.value(mbClientDevice::Strings::instance().portName);
            if (var.isNull())
                return;
            if (var.type() == QVariant::String)
            {
                QString portName = var.toString();
                port = project->port(portName);
                if (!port)
                    return;
            }
            else
            {
                MBSETTINGS ps = var.value<MBSETTINGS>();
                if (ps.isEmpty())
                    return;
                port = new mbClientPort();
                port->setSettings(ps);
                project->portAdd(port);
            }
            mbClientDevice* d = new mbClientDevice;
            d->setSettings(s);
            port->deviceAdd(d);
            project->deviceAdd(d);
            //mbClientUndoInsertDevices* cmd = new mbClientUndoInsertDevices(project);
            //cmd->append(project->deviceCount(), d);
            //m_undoStack->push(cmd);
        }
    }
}

void mbClientUi::menuSlotDeviceEdit()
{
    if (core()->isRunning())
        return;
    mbClientProject* prj = core()->project();
    if (prj)
    {
        mbClientDevice *d = projectUi()->currentDevice();
        if (d)
        {
            editDevice(d);
        }
    }
}

void mbClientUi::menuSlotDeviceDelete()
{
    if (core()->isRunning())
        return;
    mbClientProject* prj = core()->project();
    if (prj)
    {
        mbClientDevice *d = projectUi()->currentDevice();
        if (d)
        {
            QMessageBox::StandardButton res = QMessageBox::question(this,
                                                                    QStringLiteral("Delete Device"),
                                                                    QString("Are you sure you want to delete '%1'?").arg(d->name()),
                                                                    QMessageBox::Yes|QMessageBox::No);
            if (res == QMessageBox::Yes)
            {
                mbClientPort *port = d->port();
                if (port)
                    port->deviceRemove(d);
                prj->deviceRemove(d);
                delete d;
            }
        }
    }
}

void mbClientUi::menuSlotDeviceImport()
{
    if (core()->isRunning())
        return;
    mbClientProject* project = core()->project();
    if (!project)
        return;
    QString file = m_dialogs->getOpenFileName(this,
                                              QStringLiteral("Import Device ..."),
                                              QString(),
                                              m_dialogs->getFilterString(mbCoreDialogs::Filter_DeviceAll));
    if (!file.isEmpty())
    {
        if (mbClientDevice *device = static_cast<mbClientDevice*>(m_builder->importDevice(file)))
        {
            mbClientPort *port;
            port = project->port(device->portName());
            if (!port)
                port = projectUi()->currentPort();
            if (!port)
                port = project->port(0);
            if (!port)
            {
                delete device;
                return;
            }
            project->deviceAdd(device);
            port->deviceAdd(device);
        }
    }
}

void mbClientUi::menuSlotDeviceExport()
{
    if (mbClientDevice *current = projectUi()->currentDevice())
    {
        QString file = m_dialogs->getSaveFileName(this,
                                                  QString("Export Device '%1'").arg(current->name()),
                                                  QString(),
                                                  m_dialogs->getFilterString(mbCoreDialogs::Filter_DeviceAll));
        if (!file.isEmpty())
            m_builder->exportDevice(file, current);
    }
}

void mbClientUi::menuSlotDataViewItemNew()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewItemNew();
}

void mbClientUi::menuSlotDataViewItemEdit()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewItemEdit();
}

void mbClientUi::menuSlotDataViewItemInsert()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewItemInsert();
}

void mbClientUi::menuSlotDataViewItemDelete()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewItemDelete();
}

void mbClientUi::menuSlotDataViewImportItems()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewImportItems();
}

void mbClientUi::menuSlotDataViewExportItems()
{
    mbCoreUi::menuSlotDataViewExportItems();
}

void mbClientUi::menuSlotDataViewNew()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewNew();
}

void mbClientUi::menuSlotDataViewEdit()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewEdit();
}

void mbClientUi::menuSlotDataViewInsert()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewInsert();
}

void mbClientUi::menuSlotDataViewDelete()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewDelete();
}

void mbClientUi::menuSlotDataViewImport()
{
    if (!core()->isRunning())
        mbCoreUi::menuSlotDataViewImport();
}

void mbClientUi::menuSlotDataViewExport()
{
    mbCoreUi::menuSlotDataViewExport();
}

void mbClientUi::menuSlotToolsSendMessage()
{
    m_sendMessageUi->show();
}

void mbClientUi::menuSlotToolsScanner()
{
    m_scannerUi->show();
}

void mbClientUi::contextMenuDevice(mbClientDevice */*device*/)
{
    QMenu mn(m_projectUi);
    Q_FOREACH(QAction *a, ui->menuDevice->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbClientUi::editPort(mbCorePort *port)
{
    MBSETTINGS o = port->settings();
    MBSETTINGS s = dialogs()->getPort(o, QStringLiteral("Edit Port"));
    if (s.count())
    {
        port->setSettings(s);
    }
}

void mbClientUi::editDevice(mbClientDevice *device)
{
    MBSETTINGS o = device->settings();
    MBSETTINGS s = dialogs()->getDevice(o, QStringLiteral("Edit Device"));
    if (s.count())
    {
        mbClientProject* project = core()->project();
        QVariant var = s.value(mbClientDevice::Strings::instance().portName);
        MBSETTINGS ps = var.value<MBSETTINGS>();
        if (!ps.isEmpty()) // create new port
        {
            mbClientPort *newPort = new mbClientPort();
            newPort->setSettings(ps);
            project->portAdd(newPort);
            mbClientPort *oldPort = device->port();
            if (oldPort)
                oldPort->deviceRemove(device);
            newPort->deviceAdd(device);
        }
        else
        {
            mbClientPort *newPort = project->port(var.toString());
            mbClientPort *oldPort = device->port();
            if (newPort && (oldPort != newPort))
            {
                if (oldPort)
                    oldPort->deviceRemove(device);
                newPort->deviceAdd(device);
            }
        }
        device->setSettings(s);
    }
}
