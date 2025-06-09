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
#include "server_ui.h"
#include "ui_server_ui.h"

#include <QUndoStack>
#include <QBuffer>
#include <QClipboard>
#include <QLabel>
#include <QComboBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>

#include <project/server_project.h>
#include <project/server_port.h>
#include <project/server_deviceref.h>
#include <project/server_dataview.h>
#include <project/server_simaction.h>
#include <project/server_scriptmodule.h>
#include <project/server_builder.h>

#include "server_windowmanager.h"

#include "dialogs/server_dialogs.h"
#include "dialogs/server_dialogdevice.h"
#include "dialogs/server_dialogsimaction.h"
#include "project/server_projectui.h"

#include "device/server_devicemanager.h"
#include "script/server_scriptmanager.h"
#include "script/server_scriptmoduleeditor.h"
#include "script/server_devicescripteditor.h"

#include "device/server_deviceui.h"
#include "dataview/server_dataviewmanager.h"
#include "simactions/server_simactionsui.h"
#include "scriptmodules/server_scriptmodulesui.h"
#include "server_outputview.h"

mbServerUi::Strings::Strings() :
    cacheFormat(QStringLiteral("Ui.format"))
{
}

const mbServerUi::Strings &mbServerUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerUi::Defaults::Defaults() :
    cacheFormat(mb::Dec),
    filterFileActions(mbCoreDialogs::Filter_CsvFiles|mbCoreDialogs::Filter_XmlFiles|mbCoreDialogs::Filter_AllFiles)
{
}

const mbServerUi::Defaults &mbServerUi::Defaults::instance()
{
    static const Defaults d;
    return d;
}


mbServerUi::mbServerUi(mbServer *core, QWidget *parent) :
    mbCoreUi(core, parent),
    ui(new Ui::mbServerUi)
{
    ui->setupUi(this);

    m_format = mbServerUi::Defaults::instance().cacheFormat;
    m_simActionsUi = nullptr;
    m_dockSimActions = nullptr;
    m_deviceManager = nullptr;
    m_helpFile = QStringLiteral("/help/ModbusServer.qhc");
    m_outputView = new mbServerOutputView(this);

    m_ui.menuFile                        = ui->menuFile                       ;
    m_ui.menuEdit                        = ui->menuEdit                       ;
    m_ui.menuView                        = ui->menuView                       ;
    m_ui.menuPort                        = ui->menuPort                       ;
    m_ui.menuDevice                      = ui->menuDevice                     ;
    m_ui.menuDataView                    = ui->menuDataView                   ;
    m_ui.menuTools                       = ui->menuTools                      ;
    m_ui.menuRuntime                     = ui->menuRuntime                    ;
    m_ui.menuWindow                      = ui->menuWindow                     ;
    m_ui.menuWindowDataViews             = ui->menuWindowDataViews            ;
    m_ui.menuHelp                        = ui->menuHelp                       ;
    m_ui.actionFileNew                   = ui->actionFileNew                  ;
    m_ui.actionFileOpen                  = ui->actionFileOpen                 ;
    m_ui.actionFileRecent                = ui->actionFileRecent               ;
    m_ui.actionFileClose                 = ui->actionFileClose                ;
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
    m_ui.actionWindowViewSubWindow       = ui->actionWindowViewSubWindow      ;
    m_ui.actionWindowViewTabbed          = ui->actionWindowViewTabbed         ;
    m_ui.actionWindowViewSubWindow       = ui->actionWindowViewSubWindow      ;
    m_ui.actionWindowViewTabbed          = ui->actionWindowViewTabbed         ;
    m_ui.actionWindowDataViewShowAll     = ui->actionWindowDataViewShowAll    ;
    m_ui.actionWindowDataViewCloseAll    = ui->actionWindowDataViewCloseAll   ;
    m_ui.actionWindowCloseAll            = ui->actionWindowCloseAll           ;
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

mbServerUi::~mbServerUi()
{
    delete ui;
}

QWidget *mbServerUi::outputView() const
{
    return m_outputView;
}

void mbServerUi::initialize()
{
    // Output
    m_dockOutput = new QDockWidget("Output", this);
    m_dockOutput->setObjectName(QStringLiteral("dockOutput"));
    //m_outputView = new mbCoreOutputView(m_dockOutput);
    m_dockOutput->setWidget(m_outputView);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_dockOutput);
    this->tabifyDockWidget(ui->dockLogView, m_dockOutput);

    // Dialogs
    m_dialogs = new mbServerDialogs(this);

    // Mdi Area
    m_deviceManager = new mbServerDeviceManager(this);
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiContextMenu, this, &mbServerUi::contextMenuDevice);

    m_scriptManager = new mbServerScriptManager(this);
    m_dataViewManager = new mbServerDataViewManager(this);

    m_windowManager = new mbServerWindowManager(this, m_deviceManager, m_scriptManager, dataViewManager());
    connect(windowManager(), &mbServerWindowManager::deviceWindowAdded   , this, &mbServerUi::deviceWindowAdd);
    connect(windowManager(), &mbServerWindowManager::deviceWindowRemoving, this, &mbServerUi::deviceWindowRemove);
    connect(windowManager(), &mbServerWindowManager::scriptWindowAdded   , this, &mbServerUi::scriptWindowAdd);
    connect(windowManager(), &mbServerWindowManager::scriptWindowRemoving, this, &mbServerUi::scriptWindowRemove);

    // Project Inspector
    m_projectUi = new mbServerProjectUi(ui->dockProject);
    connect(projectUi(), &mbServerProjectUi::deviceDoubleClick, this, &mbServerUi::editDeviceRef       );
    connect(projectUi(), &mbServerProjectUi::deviceContextMenu, this, &mbServerUi::contextMenuDeviceRef);

    // Simulation Action
    m_dockSimActions = new QDockWidget("Simulation", this);
    m_dockSimActions->setObjectName(QStringLiteral("dockSimActions"));
    m_simActionsUi = new mbServerSimActionsUi(m_dockSimActions);
    connect(m_simActionsUi, &mbServerSimActionsUi::simActionContextMenu, this, &mbServerUi::contextMenuSimAction);
    m_dockSimActions->setWidget(m_simActionsUi);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_dockSimActions);
    this->tabifyDockWidget(ui->dockLogView, m_dockSimActions);

    // ScriptModules
    m_dockScriptModules = new QDockWidget("Script Modules", this);
    m_dockScriptModules->setObjectName(QStringLiteral("dockScriptModules"));
    m_scriptModulesUi = new mbServerScriptModulesUi(m_dockScriptModules);
    connect(m_scriptModulesUi, &mbServerScriptModulesUi::scriptModuleContextMenu, this, &mbServerUi::contextMenuScriptModule);
    m_dockScriptModules->setWidget(m_scriptModulesUi);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_dockScriptModules);
    this->tabifyDockWidget(ui->dockLogView, m_dockScriptModules);

    ui->dockLogView->raise();

    // Menu Edit
    ui->actionEditFind   ->setShortcuts(QKeySequence::Find   );
    ui->actionEditReplace->setShortcuts(QKeySequence::Replace);

    connect(ui->actionEditFind   , &QAction::triggered, this, &mbServerUi::menuSlotEditFind   );
    connect(ui->actionEditReplace, &QAction::triggered, this, &mbServerUi::menuSlotEditReplace);

    // Menu View
    connect(ui->actionViewSimulation   , &QAction::triggered, this, &mbServerUi::menuSlotViewSimulation   );
    connect(ui->actionViewScriptModules, &QAction::triggered, this, &mbServerUi::menuSlotViewScriptModules);
    connect(ui->actionViewOutput       , &QAction::triggered, this, &mbServerUi::menuSlotViewOutput       );

    // Menu Port
    connect(ui->actionPortDeviceNew   , &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceNew   );
    connect(ui->actionPortDeviceAdd   , &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceAdd   );
    connect(ui->actionPortDeviceEdit  , &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceEdit  );
    connect(ui->actionPortDeviceDelete, &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceDelete);

    // Menu Device
    connect(ui->actionDeviceMemoryZerro   , &QAction::triggered, this, &mbServerUi::menuSlotDeviceMemoryZerro   );
    connect(ui->actionDeviceMemoryZerroAll, &QAction::triggered, this, &mbServerUi::menuSlotDeviceMemoryZerroAll);
    connect(ui->actionDeviceMemoryImport  , &QAction::triggered, this, &mbServerUi::menuSlotDeviceMemoryImport  );
    connect(ui->actionDeviceMemoryExport  , &QAction::triggered, this, &mbServerUi::menuSlotDeviceMemoryExport  );
    connect(ui->actionDeviceScriptInit    , &QAction::triggered, this, &mbServerUi::menuSlotDeviceScriptInit    );
    connect(ui->actionDeviceScriptLoop    , &QAction::triggered, this, &mbServerUi::menuSlotDeviceScriptLoop    );
    connect(ui->actionDeviceScriptFinal   , &QAction::triggered, this, &mbServerUi::menuSlotDeviceScriptFinal   );

    // Menu Sim Action
    connect(ui->actionSimActionNew          , &QAction::triggered, this, &mbServerUi::menuSlotSimActionNew   );
    connect(ui->actionSimActionEdit         , &QAction::triggered, this, &mbServerUi::menuSlotSimActionEdit  );
    connect(ui->actionSimActionInsert       , &QAction::triggered, this, &mbServerUi::menuSlotSimActionInsert);
    connect(ui->actionSimActionDelete       , &QAction::triggered, this, &mbServerUi::menuSlotSimActionDelete);
    connect(ui->actionSimActionImport       , &QAction::triggered, this, &mbServerUi::menuSlotSimActionImport);
    connect(ui->actionSimActionExport       , &QAction::triggered, this, &mbServerUi::menuSlotSimActionExport);

    // Menu Script Modules
    connect(ui->actionScriptModuleNew       , &QAction::triggered, this, &mbServerUi::menuSlotScriptModuleNew       );
    connect(ui->actionScriptModuleOpen      , &QAction::triggered, this, &mbServerUi::menuSlotScriptModuleOpen      );
    connect(ui->actionScriptModuleDelete    , &QAction::triggered, this, &mbServerUi::menuSlotScriptModuleDelete    );
    connect(ui->actionScriptModuleEditParams, &QAction::triggered, this, &mbServerUi::menuSlotScriptModuleEditParams);
    connect(ui->actionScriptModuleImport    , &QAction::triggered, this, &mbServerUi::menuSlotScriptModuleImport    );
    connect(ui->actionScriptModuleExport    , &QAction::triggered, this, &mbServerUi::menuSlotScriptModuleExport    );

    // Menu Window
    //connect(ui->actionWindowDeviceShowAll      , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceShowAll      );
    //connect(ui->actionWindowDeviceShowActive   , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceShowActive   );
    //connect(ui->actionWindowDeviceCloseAll     , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceCloseAll     );
    //connect(ui->actionWindowDeviceCloseActive  , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceCloseActive  );
    //connect(ui->actionWindowDataViewShowAll    , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewShowAll    );
    //connect(ui->actionWindowDataViewShowActive , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewShowActive );
    //connect(ui->actionWindowDataViewCloseAll   , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewCloseAll   );
    //connect(ui->actionWindowDataViewCloseActive, &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewCloseActive);

    // tool bar
    // add data view format functionality to the end of toolbar
    ui->toolBar->addSeparator();
    QLabel *lb = new QLabel("Format: ", ui->toolBar);
    QAction *a = ui->toolBar->addWidget(lb);
    a->setVisible(true);
    m_cmbFormat = new QComboBox(ui->toolBar);
    QStringList ls = mb::enumDigitalFormatKeyList();
    for (int i = 1 ; i < ls.count(); i++)
        m_cmbFormat->addItem(ls.at(i));
    m_cmbFormat->setCurrentIndex(m_format);
    connect(m_cmbFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(setFormat(int)));
    a = ui->toolBar->addWidget(m_cmbFormat);
    a->setVisible(true);

    mbCoreUi::initialize();
}

MBSETTINGS mbServerUi::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r = mbCoreUi::cachedSettings();
    mb::unite(r, m_outputView->cachedSettings());
    mb::unite(r, m_scriptManager->cachedSettings());
    r[s.cacheFormat] = mb::enumDigitalFormatKey(format());
    return r;
}

void mbServerUi::setCachedSettings(const MBSETTINGS &settings)
{
    Strings s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.cacheFormat);
    if (it != end)
    {
        mb::DigitalFormat v = mb::enumDigitalFormatValue(it.value(), &ok);
        if (ok)
            setFormat(v);
    }
    mbCoreUi::setCachedSettings(settings);
    m_outputView->setCachedSettings(settings);
    m_scriptManager->setCachedSettings(settings);
}

void mbServerUi::outputMessage(const QString &message)
{
    m_outputView->showOutput(message);
}

void mbServerUi::menuSlotViewSimulation()
{
    m_dockSimActions->show();
    m_dockSimActions->setFocus();
}

void mbServerUi::menuSlotViewScriptModules()
{
    m_dockScriptModules->show();
    m_dockScriptModules->setFocus();
}

void mbServerUi::menuSlotEditCopy()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == m_dockSimActions || m_dockSimActions->isAncestorOf(focus))
        {
            slotSimActionCopy();
            return;
        }
    }
    mbCoreUi::menuSlotEditCopy();
}

void mbServerUi::menuSlotEditPaste()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == m_dockSimActions || m_dockSimActions->isAncestorOf(focus))
        {
            slotSimActionPaste();
            return;
        }
    }
    mbCoreUi::menuSlotEditPaste();
}

void mbServerUi::menuSlotEditInsert()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == m_dockSimActions || m_dockSimActions->isAncestorOf(focus))
        {
            menuSlotSimActionInsert();
            return;
        }
    }
    mbCoreUi::menuSlotEditInsert();
}

void mbServerUi::menuSlotEditEdit()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == ui->dockProject || ui->dockProject->isAncestorOf(focus))
        {
            if (m_projectUi->selectedDeviceCore())
            {
                menuSlotPortDeviceEdit();
                return;
            }
            if (m_projectUi->selectedPortCore())
            {
                menuSlotPortEdit();
                return;
            }
        }
        else if (focus == m_dockSimActions || m_dockSimActions->isAncestorOf(focus))
        {
            menuSlotSimActionEdit();
            return;
        }
    }
    mbCoreUi::menuSlotEditEdit();
}

void mbServerUi::menuSlotEditDelete()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == ui->dockProject || ui->dockProject->isAncestorOf(focus))
        {
            if (m_projectUi->selectedDeviceCore())
            {
                menuSlotPortDeviceDelete();
                return;
            }
            if (m_projectUi->selectedPortCore())
            {
                menuSlotPortDelete();
                return;
            }
        }
        else if (focus == m_dockSimActions || m_dockSimActions->isAncestorOf(focus))
        {
            menuSlotSimActionDelete();
            return;
        }
    }
    mbCoreUi::menuSlotEditDelete();
}

void mbServerUi::menuSlotEditSelectAll()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == m_dockSimActions || m_dockSimActions->isAncestorOf(focus))
        {
            slotSimActionSelectAll();
            return;
        }
    }
    mbCoreUi::menuSlotEditSelectAll();
}

void mbServerUi::menuSlotEditFind()
{
    dialogs()->execFindReplace(false);
}

void mbServerUi::menuSlotEditReplace()
{
    dialogs()->execFindReplace(true);
}

void mbServerUi::menuSlotViewOutput()
{
    m_dockOutput->show();
}

void mbServerUi::menuSlotPortNew()
{
    if (core()->isRunning())
        return;
    mbServerProject* project = core()->project();
    if (project)
    {
        MBSETTINGS s = dialogs()->getPort(MBSETTINGS(), QStringLiteral("New Port"));
        if (s.count())
        {
            mbServerPort* port = new mbServerPort;
            port->setSettings(s);
            project->portAdd(port);
            m_project->setModifiedFlag(true);
        }
    }
}

void mbServerUi::menuSlotPortEdit()
{
    if (core()->isRunning())
        return;
    mbServerPort *port = projectUi()->currentPort();
    if (port)
        editPort(port);
}

void mbServerUi::menuSlotPortDelete()
{
    if (core()->isRunning())
        return;
    mbServerProject* project = core()->project();
    if (project)
    {
        mbServerPort *port = projectUi()->currentPort();
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
                m_project->setModifiedFlag(true);
            }
        }
    }
}

void mbServerUi::menuSlotPortDeviceNew()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    mbServerPort *port = projectUi()->currentPort();
    if (!port)
        return;
    const mbServerDialogDevice::Strings &sDialog = mbServerDialogDevice::Strings::instance();

    MBSETTINGS o;
    o[sDialog.mode] = mbServerDialogDevice::EditDeviceRef;
    MBSETTINGS v = dialogs()->getDevice(o, "New Device Reference");
    if (v.count())
    {
        mbServerDevice *device = new mbServerDevice(project);
        mbServerDeviceRef *deviceRef = new mbServerDeviceRef(device);
        deviceRef->setSettings(v);
        project->deviceAdd(device);
        port->deviceAdd(deviceRef);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotPortDeviceAdd()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    mbServerPort *port = projectUi()->currentPort();
    if (!port)
        return;

    const mbServerDialogDevice::Strings &sDialog = mbServerDialogDevice::Strings::instance();

    MBSETTINGS o;
    o[sDialog.mode] = mbServerDialogDevice::ShowDevices;
    MBSETTINGS s = dialogs()->getDevice(o, QStringLiteral("Add Device Reference"));
    if (s.count())
    {
        const mbServerDeviceRef::Strings &sDeviceRef = mbServerDeviceRef::Strings::instance();
        QString deviceName = s.value(sDeviceRef.name).toString();
        mbServerDevice *device = project->device(deviceName);
        // TODO: resolve when port have 2 or more different DeviceRef to the same Device
        if (device)
        {
            s.remove(sDeviceRef.name);
            mbServerDeviceRef *deviceRef = port->deviceRef(device);
            if (deviceRef)
            {
                QString units = s.value(sDeviceRef.units).toString();
                deviceRef->addUnitsStr(units);
            }
            else
            {
                deviceRef = new mbServerDeviceRef(device);
                deviceRef->setSettings(s);
                port->deviceAdd(deviceRef);
            }
            m_project->setModifiedFlag(true);
        }
    }
}

void mbServerUi::menuSlotPortDeviceEdit()
{
    if (core()->isRunning())
        return;
    if (!m_project)
        return;
    mbServerDeviceRef *device = projectUi()->currentDeviceRef();
    if (!device)
        return;
    editDeviceRefPrivate(device);
}

void mbServerUi::menuSlotPortDeviceDelete()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    mbServerDeviceRef *device = projectUi()->currentDeviceRef();
    if (!device)
        return;
    QMessageBox::StandardButton res = QMessageBox::question(this,
                                                            QStringLiteral("Delete Device"),
                                                            QString("Are you sure you want to delete '%1' from port?").arg(device->name()),
                                                            QMessageBox::Yes|QMessageBox::No);
    if (res == QMessageBox::Yes)
    {
        mbServerPort *port = device->port();
        port->deviceRemove(device);
        delete device;
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotDeviceNew()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    MBSETTINGS v = dialogs()->getDevice(MBSETTINGS(), "New Device");
    if (v.count())
    {
        mbServerDevice *d = new mbServerDevice(project);
        d->setSettings(v);
        project->deviceAdd(d);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotDeviceEdit()
{
    mbServerDeviceUi *deviceUi = this->activeDeviceUi();
    if (!deviceUi)
        return;
    editDevice(deviceUi->device());
}

void mbServerUi::menuSlotDeviceDelete()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    mbServerDeviceUi *deviceUi = this->activeDeviceUi();
    if (!deviceUi)
        return;
    mbServerDevice *device = deviceUi->device();
    QMessageBox::StandardButton res = QMessageBox::question(this,
                                                            QStringLiteral("Delete Device"),
                                                            QString("Are you sure you want to delete '%1'?").arg(device->name()),
                                                            QMessageBox::Yes|QMessageBox::No);
    if (res == QMessageBox::Yes)
    {
        Q_FOREACH(mbServerPort *port, project->ports())
            port->deviceRemove(device);
        project->deviceRemove(device);
        delete device;
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotDeviceImport()
{
    mbServerProject *project = core()->project();
    if (!project)
        return;
    QString file = m_dialogs->getOpenFileName(this,
                                              QStringLiteral("Import Device ..."),
                                              QString(),
                                              m_dialogs->getFilterString(mbCoreDialogs::Filter_DeviceAll));
    if (!file.isEmpty())
    {
        if (mbServerDevice *device = static_cast<mbServerDevice*>(m_builder->importDevice(file)))
        {
            project->deviceAdd(device);
            m_project->setModifiedFlag(true);
        }
    }
}

void mbServerUi::menuSlotDeviceExport()
{
    mbServerProject *project = core()->project();
    mbServerDeviceUi *deviceUi = this->activeDeviceUi();
    if (project && deviceUi)
    {
        mbServerDevice *current = deviceUi->device();
        QString file = m_dialogs->getSaveFileName(this,
                                                  QString("Export Device '%1'").arg(current->name()),
                                                  QString(),
                                                  m_dialogs->getFilterString(mbCoreDialogs::Filter_DeviceAll));
        if (!file.isEmpty())
            m_builder->exportDevice(file, current);
    }
}

void mbServerUi::menuSlotDeviceMemoryZerro()
{
    if (mbServerDeviceUi *deviceUi = this->activeDeviceUi())
    {
        deviceUi->slotMemoryZerro();
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotDeviceMemoryZerroAll()
{
    if (mbServerDeviceUi *deviceUi = this->activeDeviceUi())
    {
        deviceUi->slotMemoryZerroAll();
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotDeviceMemoryImport()
{
    if (mbServerDeviceUi *deviceUi = this->activeDeviceUi())
    {
        QString file = dialogs()->getOpenFileName(this,
                                                  QStringLiteral("Import memory values ..."),
                                                  QString(),
                                                  m_dialogs->getFilterString(mbCoreDialogs::Filter_CsvFiles));
        if (file.isEmpty())
            return;
        Modbus::MemoryType memoryType = deviceUi->currentMemoryType();
        QByteArray data;
        switch (memoryType)
        {
        case Modbus::Memory_0x:
        case Modbus::Memory_1x:
            if (!builder()->importBoolData(file, data))
                return;
            break;
        default:
            if (!builder()->importUInt16Data(file, data))
                return;
            break;
        }
        deviceUi->setData(memoryType, data);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotDeviceMemoryExport()
{
    const int columnCount = 10;
    if (mbServerDeviceUi *deviceUi = this->activeDeviceUi())
    {
        QString file = dialogs()->getSaveFileName(this,
                                                  QStringLiteral("Export memory values ..."),
                                                  QString(),
                                                  m_dialogs->getFilterString(mbCoreDialogs::Filter_CsvFiles));
        if (file.isEmpty())
            return;
        Modbus::MemoryType memoryType = deviceUi->currentMemoryType();
        QByteArray data = deviceUi->getData(memoryType);
        switch (memoryType)
        {
        case Modbus::Memory_0x:
        case Modbus::Memory_1x:
            builder()->exportBoolData(file, data, columnCount);
            break;
        default:
            builder()->exportUInt16Data(file, data, columnCount);
            break;
        }
    }
}

void mbServerUi::menuSlotDeviceScriptInit()
{
    mbServerDeviceUi *deviceUi = this->activeDeviceUi();
    if (deviceUi)
        windowManager()->showDeviceScript(deviceUi->device(), mbServerDevice::Script_Init);
}

void mbServerUi::menuSlotDeviceScriptLoop()
{
    mbServerDeviceUi *deviceUi = this->activeDeviceUi();
    if (deviceUi)
        windowManager()->showDeviceScript(deviceUi->device(), mbServerDevice::Script_Loop);
}

void mbServerUi::menuSlotDeviceScriptFinal()
{
    mbServerDeviceUi *deviceUi = this->activeDeviceUi();
    if (deviceUi)
        windowManager()->showDeviceScript(deviceUi->device(), mbServerDevice::Script_Final);
}

void mbServerUi::menuSlotSimActionNew()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        MBSETTINGS p = dialogs()->getSimAction(MBSETTINGS(), "New Sim Action(s)");
        if (p.count())
        {
            const mbServerSimAction::Strings &sAction = mbServerSimAction::Strings::instance();
            int count = p.value(mbServerDialogSimAction::Strings::instance().count).toInt();
            if (count > 0)
            {
                for (int i = 0; i < count; i++)
                {
                    mbServerSimAction *action = new mbServerSimAction();
                    action->setSettings(p);
                    project->simActionAdd(action);
                    p[sAction.address] = action->addressInt() + action->length();
                }
                m_project->setModifiedFlag(true);
            }
        }
    }
}

void mbServerUi::menuSlotSimActionEdit()
{
    if (core()->isRunning())
        return;
    QList<mbServerSimAction*> actions = m_simActionsUi->selectedItems();
    if (!actions.count())
        return;
    editActions(actions);
}

void mbServerUi::menuSlotSimActionInsert()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        int index = m_simActionsUi->currentItemIndex();
        mbServerSimAction* next = project->simAction(index);
        mbServerSimAction* prev;
        if (next)
            prev = project->simAction(index-1);
        else
            prev = project->simAction(project->simActionCount()-1);
        mbServerSimAction* newItem;
        if (prev)
        {
            newItem = builder()->newSimAction(prev);
            next = project->simAction(index);
        }
        else
        {
            newItem = builder()->newSimAction();
            newItem->setDevice(project->device(0));
        }
        project->simActionInsert(newItem, index);
        if (next)
            m_simActionsUi->selectItem(next);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotSimActionDelete()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        QList<mbServerSimAction*> items = m_simActionsUi->selectedItems();
        project->simActionsRemove(items);
        project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotSimActionImport()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        QString file = m_dialogs->getOpenFileName(this,
                                                  QStringLiteral("Import Actions ..."),
                                                  QString(),
                                                  m_dialogs->getFilterString(Defaults::instance().filterFileActions));
        if (!file.isEmpty())
        {
            auto actions = builder()->importSimActions(file);
            if (actions.count())
            {
                int index = m_simActionsUi->currentItemIndex();
                project->simActionsInsert(actions, index);
                m_project->setModifiedFlag(true);
            }
        }
    }
}

void mbServerUi::menuSlotSimActionExport()
{
    mbServerProject *project = core()->project();
    if (project)
    {
        auto items = m_simActionsUi->selectedItems();
        if (items.isEmpty())
            items = project->simActions();
        if (items.count())
        {
            QString file = m_dialogs->getSaveFileName(this,
                                                      QStringLiteral("Export Actions ..."),
                                                      QString(),
                                                      m_dialogs->getFilterString(Defaults::instance().filterFileActions));
            if (!file.isEmpty())
                builder()->exportSimActions(file, items);
        }
    }
}

void mbServerUi::menuSlotScriptModuleNew()
{
    mbServerProject *project = core()->project();
    if (project)
    {
        MBSETTINGS p = dialogs()->getScriptModule(MBSETTINGS(), "New Script Module");
        if (p.count())
        {
            mbServerScriptModule *sm = new mbServerScriptModule();
            sm->setSettings(p);
            project->scriptModuleAdd(sm);
            project->setModifiedFlag(true);
            windowManager()->showScriptModule(sm);
        }
    }
}

void mbServerUi::menuSlotScriptModuleOpen()
{
    QList<mbServerScriptModule*> modules = m_scriptModulesUi->selectedItems();
    if (!modules.count())
        return;
    mbServerScriptModule *sm = modules.first();
    windowManager()->showScriptModule(sm);
}

void mbServerUi::menuSlotScriptModuleDelete()
{
    mbServerProject *project = core()->project();
    if (project)
    {
        QList<mbServerScriptModule*> modules = m_scriptModulesUi->selectedItems();
        Q_FOREACH (mbServerScriptModule *sm, modules)
            project->scriptModuleRemove(sm);
        project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotScriptModuleEditParams()
{
    QList<mbServerScriptModule*> modules = m_scriptModulesUi->selectedItems();
    if (!modules.count())
        return;
    mbServerScriptModule *sm = modules.first();
    MBSETTINGS p = dialogs()->getScriptModule(sm->settings(), "Edit Script Module");
    if (p.count())
    {
        sm->setSettings(p);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::menuSlotScriptModuleImport()
{
    mbServerProject *project = core()->project();
    if (project)
    {
        QString fileName = m_dialogs->getOpenFileName(this,
                                                      QStringLiteral("Import Module ..."),
                                                      QString(),
                                                      QString("Python files (*.py);;All files (*.*)"));
        if (fileName.length())
        {
            QFile file(fileName);
            if (file.open((QIODevice::ReadOnly)))
            {
                QByteArray data = file.readAll();
                file.close();
                QString code = QString::fromUtf8(data);
                mbServerScriptModule *sm = new mbServerScriptModule;
                QFileInfo fi(file);
                sm->setName(fi.baseName());
                sm->setSourceCode(code);
                project->scriptModuleAdd(sm);
                windowManager()->showScriptModule(sm);
            }
        }
    }
}

void mbServerUi::menuSlotScriptModuleExport()
{
    QList<mbServerScriptModule*> modules = m_scriptModulesUi->selectedItems();
    if (!modules.count())
        return;
    mbServerScriptModule *sm = modules.first();
    QString fileName = m_dialogs->getSaveFileName(this,
                                                  QStringLiteral("Export Module ..."),
                                                  QString(),
                                                  QString("Python files (*.py);;All files (*.*)"));
    if (fileName.length())
    {
        QByteArray data = sm->getSourceCode().toUtf8();
        QFile file(fileName);
        if (file.open((QIODevice::WriteOnly)))
        {
            file.write(data);
            file.close();
        }
    }
}

void mbServerUi::menuSlotWindowDeviceShowAll()
{
    windowManager()->actionWindowDeviceShowAll();
}

void mbServerUi::menuSlotWindowDeviceShowActive()
{
    windowManager()->actionWindowDeviceShowActive();
}

void mbServerUi::menuSlotWindowDeviceCloseAll()
{
    windowManager()->actionWindowDeviceCloseAll();
}

void mbServerUi::menuSlotWindowDeviceCloseActive()
{
    windowManager()->actionWindowDeviceCloseActive();
}

void mbServerUi::slotSimActionCopy()
{
    QList<mbServerSimAction*> selectedItems = m_simActionsUi->selectedItems();
    if (selectedItems.count())
    {
        QBuffer buff;
        buff.open(QIODevice::ReadWrite);
        builder()->exportSimActionsXml(&buff, selectedItems);
        buff.seek(0);
        QByteArray b = buff.readAll();
        QApplication::clipboard()->setText(QString::fromUtf8(b));
    }
}

void mbServerUi::slotSimActionPaste()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        QString text = QApplication::clipboard()->text();
        if (text.isEmpty())
            return;
        QByteArray b = text.toUtf8();
        QBuffer buff(&b);
        buff.open(QIODevice::ReadOnly);
        QList<mbServerSimAction*> items = builder()->importSimActionsXml(&buff);
        if (items.count())
        {
            int index = -1;
            QList<mbServerSimAction*> selectedItems = m_simActionsUi->selectedItems();
            if (selectedItems.count())
                index = project->simActionIndex(selectedItems.first());
            project->simActionsInsert(items, index);
            m_project->setModifiedFlag(true);
        }
    }
}

void mbServerUi::slotSimActionSelectAll()
{
    m_simActionsUi->selectAll();
}

void mbServerUi::setFormat(int format)
{
    // no need to set format to model_0x and model_1x because it's always Bin16
    if (m_format != format)
    {
        m_format = static_cast<mb::DigitalFormat>(format);
        if (m_format != m_cmbFormat->currentIndex())
            m_cmbFormat->setCurrentIndex(m_format);
        Q_EMIT formatChanged(m_format);
    }
}

void mbServerUi::editPort(mbCorePort *port)
{
    if (core()->isRunning())
        return;
    if (!m_project)
        return;
    editPortPrivate(static_cast<mbServerPort*>(port));
}

void mbServerUi::editDeviceRef(mbServerDeviceRef *device)
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    windowManager()->setActiveDevice(device->device());
    editDeviceRefPrivate(device);
}

void mbServerUi::editDevice(mbServerDevice *device)
{
    mbServerProject *project = core()->project();
    if (!project)
        return;
    editDevicePrivate(device);
}

void mbServerUi::editAction(mbServerSimAction *action)
{
    QList<mbServerSimAction*> actions;
    actions.append(action);
    editActions(actions);
}

void mbServerUi::editActions(const QList<mbServerSimAction*> &actions)
{
    MBSETTINGS s = actions.first()->settings();
    s[mbServerDialogSimAction::Strings::instance().count] = actions.count();
    MBSETTINGS p = dialogs()->getSimAction(s, "Edit Sim Actions");
    if (p.count())
    {
        Q_FOREACH (mbServerSimAction *action, actions)
        {
            action->setSettings(p);
            p[mbServerSimAction::Strings::instance().address] = action->addressInt() + action->length();
            m_project->setModifiedFlag(true);
        }
    }
}

void mbServerUi::contextMenuDevice(mbServerDeviceUi * deviceUi)
{
    QMenu mn(deviceUi); // Note: be careful to delete deviceUi while his child 'QMenu' in stack
                        //       User can choose 'actionDeleteDevice' and program can crash
                        // Solution: don't use direct 'delete deviceUi', use 'deviceUi->deleteLater'
    Q_FOREACH(QAction *a, ui->menuDevice->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbServerUi::contextMenuDeviceRef(mbServerDeviceRef */*device*/)
{
    //mbServerPort *port = nullptr;
    //if (device)
    //    port = device->port();
    //contextMenuPort(port);
    QMenu mn(m_projectUi); // Note: be careful to delete deviceUi while his child 'QMenu' in stack
        //       User can choose 'actionDeleteDevice' and program can crash
        // Solution: don't use direct 'delete deviceUi', use 'deviceUi->deleteLater'
    Q_FOREACH(QAction *a, ui->menuDevice->actions())
        mn.addAction(a);
    mn.addSeparator();
    mn.addAction(ui->actionPortDeviceNew);
    mn.addAction(ui->actionPortDeviceAdd);
    mn.addAction(ui->actionPortDeviceEdit);
    mn.addAction(ui->actionPortDeviceDelete);
    mn.exec(QCursor::pos());

}

void mbServerUi::contextMenuSimAction(mbServerSimAction * /*action*/)
{
    QMenu mn(m_simActionsUi);
    Q_FOREACH(QAction *a, ui->menuAction->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbServerUi::contextMenuScriptModule(mbServerScriptModule * /*scriptModule*/)
{
    QMenu mn(m_scriptModulesUi);
    Q_FOREACH(QAction *a, ui->menuScripting->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbServerUi::deviceWindowAdd(mbServerDeviceUi *ui)
{
    QAction *a = new QAction(ui->name());
    a->setData(QVariant::fromValue<void*>(ui));
    m_deviceActions.insert(ui, a);
    this->ui->menuWindowDevices->addAction(a);
    connect(ui, &mbServerDeviceUi::nameChanged, this, &mbServerUi::deviceWindowRename);
    connect(a, &QAction::triggered, this, &mbServerUi::deviceWindowShow);
}

void mbServerUi::deviceWindowRemove(mbServerDeviceUi *ui)
{
    const auto it = m_deviceActions.find(ui);
    if (it != m_deviceActions.end())
    {
        ui->disconnect();
        QAction *a = it.value();
        m_deviceActions.erase(it);
        delete a;
    }
}

void mbServerUi::deviceWindowRename(const QString &name)
{
    mbServerDeviceUi *ui = qobject_cast<mbServerDeviceUi*>(sender());
    QAction *a = m_deviceActions.value(ui);
    if (a)
    {
        a->setText(name);
    }
}

void mbServerUi::deviceWindowShow()
{
    QAction *a = qobject_cast<QAction*>(sender());
    if (a)
    {
        mbServerDeviceUi *ui = reinterpret_cast<mbServerDeviceUi*>(a->data().value<void*>());
        windowManager()->showDeviceUi(ui);
    }
}

void mbServerUi::scriptWindowAdd(mbServerBaseScriptEditor *se)
{
    QAction *a = new QAction(se->name());
    a->setData(QVariant::fromValue<void*>(se));
    m_scriptActions.insert(se, a);
    this->ui->menuWindowScripts->addAction(a);
    connect(se, &mbServerBaseScriptEditor::nameChanged, this, &mbServerUi::scriptWindowRename);
    connect(a, &QAction::triggered, this, &mbServerUi::scriptWindowShow);
}

void mbServerUi::scriptWindowRemove(mbServerBaseScriptEditor *se)
{
    const auto it = m_scriptActions.find(se);
    if (it != m_scriptActions.end())
    {
        se->disconnect();
        QAction *a = it.value();
        m_scriptActions.erase(it);
        delete a;
    }
}

void mbServerUi::scriptWindowRename(const QString &name)
{
    mbServerBaseScriptEditor *se = qobject_cast<mbServerBaseScriptEditor*>(sender());
    QAction *a = m_scriptActions.value(se);
    if (a)
    {
        a->setText(name);
    }
}

void mbServerUi::scriptWindowShow()
{
    QAction *a = qobject_cast<QAction*>(sender());
    if (a)
    {
        mbServerBaseScriptEditor *se = reinterpret_cast<mbServerBaseScriptEditor*>(a->data().value<void*>());
        windowManager()->showScriptEditor(se);
    }
}

void mbServerUi::editPortPrivate(mbServerPort *port)
{
    MBSETTINGS o = port->settings();
    MBSETTINGS s = dialogs()->getPort(o);
    if (s.count())
    {
        port->setSettings(s);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::editDeviceRefPrivate(mbServerDeviceRef *device)
{
    const mbServerDialogDevice::Strings &sDialog = mbServerDialogDevice::Strings::instance();

    MBSETTINGS o = device->settings();
    o[sDialog.mode] = mbServerDialogDevice::EditDeviceRef;
    MBSETTINGS s = dialogs()->getDevice(o, QStringLiteral("Edit Device Reference"));
    if (s.count())
    {
        device->setSettings(s);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::editDevicePrivate(mbServerDevice *device)
{
    MBSETTINGS o = device->settings();
    MBSETTINGS s = dialogs()->getDevice(o, QStringLiteral("Edit Device"));
    if (s.count())
    {
        device->setSettings(s);
        m_project->setModifiedFlag(true);
    }
}

void mbServerUi::saveProjectInner()
{
    Q_FOREACH(mbServerScriptModuleEditor *se, m_scriptManager->scriptModuleEditors())
    {
        mbServerScriptModule *sm = se->scriptModule();
        sm->setSourceCode(se->toPlainText());
    }
    Q_FOREACH(mbServerDeviceScriptEditor *se, m_scriptManager->deviceScriptEditors())
    {
        mbServerDevice *device = se->device();
        device->setScript(se->scriptType(), se->toPlainText());
    }
}

mbServerDeviceUi *mbServerUi::activeDeviceUi() const
{
    mbServerProjectUi *project = projectUi();
    QWidget* focusWidget = QApplication::focusWidget();
    if (focusWidget && (project == focusWidget || project->isAncestorOf(focusWidget)))
        return m_deviceManager->deviceUi(project->currentDevice());
    return m_deviceManager->activeDeviceUi();
}
