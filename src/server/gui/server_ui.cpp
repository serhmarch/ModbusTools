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

#include <project/server_project.h>
#include <project/server_port.h>
#include <project/server_deviceref.h>
#include <project/server_dataview.h>
#include <project/server_action.h>
#include <project/server_builder.h>

#include "server_windowmanager.h"

#include "dialogs/server_dialogs.h"
#include "dialogs/server_dialogdevice.h"
#include "dialogs/server_dialogaction.h"
#include "project/server_projectui.h"

#include "device/server_devicemanager.h"
#include "device/server_deviceui.h"

#include "dataview/server_dataviewmanager.h"
#include "dataview/server_dataviewui.h"

#include "actions/server_actionsui.h"

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
    cacheFormat(mb::Dec)
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
    m_projectUi = nullptr;
    m_actionsUi = nullptr;
    m_dockActions = nullptr;
    m_deviceManager = nullptr;
    m_lbSystemName = nullptr;
    m_lbSystemStatus = nullptr;
    m_helpFile = QStringLiteral("/help/ModbusServer.qhc");

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

mbServerUi::~mbServerUi()
{
    delete ui;
}

void mbServerUi::initialize()
{
    // Dialogs
    m_dialogs = new mbServerDialogs(this);

    // Mdi Area
    m_deviceManager = new mbServerDeviceManager(this);
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiContextMenu, this, &mbServerUi::contextMenuDevice);

    m_dataViewManager = new mbServerDataViewManager(this);

    m_windowManager = new mbServerWindowManager(this, m_deviceManager, dataViewManager());

    // Project Inspector
    m_projectUi = new mbServerProjectUi(ui->dockProject);
    connect(projectUi(), &mbServerProjectUi::deviceDoubleClick, this, &mbServerUi::editDeviceRef       );
    connect(projectUi(), &mbServerProjectUi::deviceContextMenu, this, &mbServerUi::contextMenuDeviceRef);

    // Action
    m_dockActions = new QDockWidget("Actions", this);
    m_dockActions->setObjectName(QStringLiteral("dockActions"));
    m_actionsUi = new mbServerActionsUi(m_dockActions);
    connect(m_actionsUi, &mbServerActionsUi::actionContextMenu, this, &mbServerUi::contextMenuAction   );
    m_dockActions->setWidget(m_actionsUi);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_dockActions);
    this->tabifyDockWidget(m_dockActions, ui->dockLogView);

    // Menu View
    connect(ui->actionViewActions, &QAction::triggered, this, &mbServerUi::menuSlotViewActions);

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

    // Menu Action
    connect(ui->actionActionNew          , &QAction::triggered, this, &mbServerUi::menuSlotActionNew   );
    connect(ui->actionActionEdit         , &QAction::triggered, this, &mbServerUi::menuSlotActionEdit  );
    connect(ui->actionActionInsert       , &QAction::triggered, this, &mbServerUi::menuSlotActionInsert);
    connect(ui->actionActionDelete       , &QAction::triggered, this, &mbServerUi::menuSlotActionDelete);
    connect(ui->actionActionImport       , &QAction::triggered, this, &mbServerUi::menuSlotActionImport);
    connect(ui->actionActionExport       , &QAction::triggered, this, &mbServerUi::menuSlotActionExport);

    // Menu Window
    connect(ui->actionWindowDeviceShowAll      , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceShowAll      );
    connect(ui->actionWindowDeviceShowActive   , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceShowActive   );
    connect(ui->actionWindowDeviceCloseAll     , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceCloseAll     );
    connect(ui->actionWindowDeviceCloseActive  , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceCloseActive  );
    connect(ui->actionWindowDataViewShowAll    , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewShowAll    );
    connect(ui->actionWindowDataViewShowActive , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewShowActive );
    connect(ui->actionWindowDataViewCloseAll   , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewCloseAll   );
    connect(ui->actionWindowDataViewCloseActive, &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewCloseActive);

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
}

void mbServerUi::menuSlotViewProject()
{
    ui->dockProject->show();
}

void mbServerUi::menuSlotViewActions()
{
    m_dockActions->show();
    m_dockActions->setFocus();
}

void mbServerUi::menuSlotViewLogView()
{
    ui->dockLogView->show();
}

void mbServerUi::menuSlotEditCopy()
{
    QWidget* focus = QApplication::focusWidget();
    if (focus)
    {
        if (focus == m_dockActions || m_dockActions->isAncestorOf(focus))
        {
            slotActionCopy();
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
        if (focus == m_dockActions || m_dockActions->isAncestorOf(focus))
        {
            slotActionPaste();
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
        if (focus == m_dockActions || m_dockActions->isAncestorOf(focus))
        {
            menuSlotActionInsert();
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
        else if (focus == m_dockActions || m_dockActions->isAncestorOf(focus))
        {
            menuSlotActionEdit();
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
                menuSlotDeviceDelete();
                return;
            }
            if (m_projectUi->selectedPortCore())
            {
                menuSlotPortDelete();
                return;
            }
        }
        else if (focus == m_dockActions || m_dockActions->isAncestorOf(focus))
        {
            menuSlotActionDelete();
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
        if (focus == m_dockActions || m_dockActions->isAncestorOf(focus))
        {
            slotActionSelectAll();
            return;
        }
    }
    mbCoreUi::menuSlotEditSelectAll();
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
                return;
            project->portRemove(port);
            delete port;
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
        }
    }
}

void mbServerUi::menuSlotPortDeviceEdit()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
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
    mbServerPort *port = device->port();
    port->deviceRemove(device);
    delete device;
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
    }
}

void mbServerUi::menuSlotDeviceEdit()
{
    mbServerDevice *d = m_deviceManager->activeDevice();
    if (!d)
        return;
    editDevice(d);
}

void mbServerUi::menuSlotDeviceDelete()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (!project)
        return;
    mbServerDevice *d = m_deviceManager->activeDevice();
    if (!d)
        return;
    Q_FOREACH(mbServerPort *port, project->ports())
        port->deviceRemove(d);
    project->deviceRemove(d);
    delete d;
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
            project->deviceAdd(device);
    }
}

void mbServerUi::menuSlotDeviceExport()
{
    mbServerProject *project = core()->project();
    if (project && m_deviceManager->activeDevice())
    {
        mbServerDevice *current = m_deviceManager->activeDevice();
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
    if (mbServerDeviceUi *deviceUi = m_deviceManager->activeDeviceUi())
        deviceUi->slotMemoryZerro();
}

void mbServerUi::menuSlotDeviceMemoryZerroAll()
{
    if (mbServerDeviceUi *deviceUi = m_deviceManager->activeDeviceUi())
        deviceUi->slotMemoryZerroAll();
}

void mbServerUi::menuSlotDeviceMemoryImport()
{
    if (mbServerDeviceUi *deviceUi = m_deviceManager->activeDeviceUi())
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
    }
}

void mbServerUi::menuSlotDeviceMemoryExport()
{
    const int columnCount = 10;
    if (mbServerDeviceUi *deviceUi = m_deviceManager->activeDeviceUi())
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

void mbServerUi::menuSlotActionNew()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        MBSETTINGS p = dialogs()->getAction(MBSETTINGS(), "New Action(s)");
        if (p.count())
        {
            const mbServerAction::Strings &sAction = mbServerAction::Strings::instance();
            int count = p.value(mbServerDialogAction::Strings::instance().count).toInt();
            if (count > 0)
            {
                for (int i = 0; i < count; i++)
                {
                    mbServerAction *action = new mbServerAction();
                    action->setSettings(p);
                    project->actionAdd(action);
                    p[sAction.address] = action->addressInt() + action->length();
                }
            }
        }
    }
}

void mbServerUi::menuSlotActionEdit()
{
    if (core()->isRunning())
        return;
    QList<mbServerAction*> actions = m_actionsUi->selectedItems();
    if (!actions.count())
        return;
    editActions(actions);
}

void mbServerUi::menuSlotActionInsert()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        int index = m_actionsUi->currentItemIndex();
        mbServerAction* next = project->action(index);
        mbServerAction* prev;
        if (next)
            prev = project->action(index-1);
        else
            prev = project->action(project->actionCount()-1);
        mbServerAction* newItem;
        if (prev)
        {
            newItem = builder()->newAction(prev);
            next = project->action(index);
        }
        else
        {
            newItem = builder()->newAction();
            newItem->setDevice(project->device(0));
        }
        project->actionInsert(newItem, index);
        if (next)
            m_actionsUi->selectItem(next);
    }
}

void mbServerUi::menuSlotActionDelete()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        QList<mbServerAction*> items = m_actionsUi->selectedItems();
        project->actionsRemove(items);
    }
}

void mbServerUi::menuSlotActionImport()
{
    if (core()->isRunning())
        return;
    mbServerProject *project = core()->project();
    if (project)
    {
        QString file = m_dialogs->getOpenFileName(this,
                                                  QStringLiteral("Import Actions ..."),
                                                  QString(),
                                                  m_dialogs->getFilterString(mbCoreDialogs::Filter_XmlFiles|mbCoreDialogs::Filter_AllFiles));
        if (!file.isEmpty())
        {
            auto actions = builder()->importActions(file);
            if (actions.count())
            {
                int index = m_actionsUi->currentItemIndex();
                project->actionsInsert(actions, index);
            }
        }
    }
}

void mbServerUi::menuSlotActionExport()
{
    mbServerProject *project = core()->project();
    if (project)
    {
        auto selected = m_actionsUi->selectedItems();
        if (selected.count())
        {
            QString file = m_dialogs->getSaveFileName(this,
                                                      QStringLiteral("Export Actions ..."),
                                                      QString(),
                                                      m_dialogs->getFilterString(mbCoreDialogs::Filter_XmlFiles|mbCoreDialogs::Filter_AllFiles));
            if (!file.isEmpty())
                builder()->exportActions(file, selected);
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

void mbServerUi::slotActionCopy()
{
    QList<mbServerAction*> selectedItems = m_actionsUi->selectedItems();
    if (selectedItems.count())
    {
        QBuffer buff;
        buff.open(QIODevice::ReadWrite);
        builder()->exportActions(&buff, selectedItems);
        buff.seek(0);
        QByteArray b = buff.readAll();
        QApplication::clipboard()->setText(QString::fromUtf8(b));
    }
}

void mbServerUi::slotActionPaste()
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
        QList<mbServerAction*> items = builder()->importActions(&buff);
        if (items.count())
        {
            int index = -1;
            QList<mbServerAction*> selectedItems = m_actionsUi->selectedItems();
            if (selectedItems.count())
                index = project->actionIndex(selectedItems.first());
            project->actionsInsert(items, index);
        }
    }
}

void mbServerUi::slotActionSelectAll()
{
    m_actionsUi->selectAll();
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
    mbServerProject *project = core()->project();
    if (!project)
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

void mbServerUi::editAction(mbServerAction *action)
{
    QList<mbServerAction*> actions;
    actions.append(action);
    editActions(actions);
}

void mbServerUi::editActions(const QList<mbServerAction*> &actions)
{
    MBSETTINGS s = actions.first()->settings();
    s[mbServerDialogAction::Strings::instance().count] = actions.count();
    MBSETTINGS p = dialogs()->getAction(s, "Edit Actions");
    if (p.count())
    {
        Q_FOREACH (mbServerAction *action, actions)
        {
            action->setSettings(p);
            p[mbServerAction::Strings::instance().address] = action->addressInt() + action->length();
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

void mbServerUi::contextMenuDeviceRef(mbServerDeviceRef *device)
{
    mbServerPort *port = nullptr;
    if (device)
        port = device->port();
    contextMenuPort(port);
}

void mbServerUi::contextMenuAction(mbServerAction * /*action*/)
{
    QMenu mn(m_actionsUi);
    Q_FOREACH(QAction *a, ui->menuAction->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbServerUi::editPortPrivate(mbServerPort *port)
{
    MBSETTINGS o = port->settings();
    MBSETTINGS s = dialogs()->getPort(o);
    if (s.count())
    {
        port->setSettings(s);
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
    }
}

void mbServerUi::editDevicePrivate(mbServerDevice *device)
{
    MBSETTINGS o = device->settings();
    MBSETTINGS s = dialogs()->getDevice(o, QStringLiteral("Edit Device"));
    if (s.count())
    {
        device->setSettings(s);
    }
}
