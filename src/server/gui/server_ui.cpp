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
}

mbServerUi::~mbServerUi()
{
    delete ui;
}

void mbServerUi::initialize()
{
    // LogView
    ui->dockLogView->setWidget(logView());

    // Dialogs
    m_dialogs = new mbServerDialogs(this);

    // Mdi Area
    m_deviceManager = new mbServerDeviceManager(this);
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiContextMenu, this, &mbServerUi::contextMenuDevice);

    m_dataViewManager = new mbServerDataViewManager(this);
    connect(m_dataViewManager, &mbServerDataViewManager::dataViewUiContextMenu, this, &mbServerUi::contextMenuDataView);

    m_windowManager = new mbServerWindowManager(this, m_deviceManager, dataViewManager());
    this->setCentralWidget(m_windowManager->centralWidget());

    // Project Inspector
    m_projectUi = new mbServerProjectUi(ui->dockProject);
    connect(projectUi(), &mbServerProjectUi::portDoubleClick  , this, &mbServerUi::editPort            );
    connect(projectUi(), &mbServerProjectUi::deviceDoubleClick, this, &mbServerUi::editDeviceRef       );
    connect(projectUi(), &mbServerProjectUi::portContextMenu  , this, &mbServerUi::contextMenuPort     );
    connect(projectUi(), &mbServerProjectUi::deviceContextMenu, this, &mbServerUi::contextMenuDeviceRef);
    ui->dockProject->setWidget(m_projectUi);

    // Action
    m_dockActions = new QDockWidget("Actions", this);
    m_actionsUi = new mbServerActionsUi(m_dockActions);
    connect(m_actionsUi, &mbServerActionsUi::actionContextMenu, this, &mbServerUi::contextMenuAction   );
    m_dockActions->setWidget(m_actionsUi);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_dockActions);
    this->tabifyDockWidget(m_dockActions, ui->dockLogView);

    // Menu File
    ui->actionFileNew   ->setShortcuts(QKeySequence::New   );
    ui->actionFileOpen  ->setShortcuts(QKeySequence::Open  );
    ui->actionFileSave  ->setShortcuts(QKeySequence::Save  );
    ui->actionFileSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionEdit      ->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_E));
    ui->actionQuit      ->setShortcuts(QKeySequence::Quit);

    connect(ui->actionFileNew   , &QAction::triggered, this, &mbServerUi::menuSlotFileNew   );
    connect(ui->actionFileOpen  , &QAction::triggered, this, &mbServerUi::menuSlotFileOpen  );
    connect(ui->actionFileSave  , &QAction::triggered, this, &mbServerUi::menuSlotFileSave  );
    connect(ui->actionFileSaveAs, &QAction::triggered, this, &mbServerUi::menuSlotFileSaveAs);
    connect(ui->actionEdit      , &QAction::triggered, this, &mbServerUi::menuSlotFileEdit  );
    connect(ui->actionQuit      , &QAction::triggered, this, &mbServerUi::menuSlotFileQuit  );
    // Menu Edit
  //ui->actionEditUndo     ->setShortcuts(QKeySequence::Undo                );
  //ui->actionEditRedo     ->setShortcuts(QKeySequence::Redo                );
    ui->actionEditCut      ->setShortcuts(QKeySequence::Cut                 );
    ui->actionEditCopy     ->setShortcuts(QKeySequence::Copy                );
    ui->actionEditPaste    ->setShortcuts(QKeySequence::Paste               );
    ui->actionEditInsert   ->setShortcut (QKeySequence(Qt::Key_Insert      ));
  //ui->actionEditEdit     ->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_E));
    ui->actionEditDelete   ->setShortcuts(QKeySequence::Delete              );
    ui->actionEditSelectAll->setShortcuts(QKeySequence::SelectAll           );

  //connect(ui->actionEditUndo      , &QAction::triggered, this, &mbServerUi::menuSlotEditUndo     );
  //connect(ui->actionEditRedo      , &QAction::triggered, this, &mbServerUi::menuSlotEditRedo     );
    connect(ui->actionEditCut       , &QAction::triggered, this, &mbServerUi::menuSlotEditCut      );
    connect(ui->actionEditCopy      , &QAction::triggered, this, &mbServerUi::menuSlotEditCopy     );
    connect(ui->actionEditPaste     , &QAction::triggered, this, &mbServerUi::menuSlotEditPaste    );
    connect(ui->actionEditInsert    , &QAction::triggered, this, &mbServerUi::menuSlotEditInsert   );
  //connect(ui->actionEditEdit      , &QAction::triggered, this, &mbServerUi::menuSlotEditEdit     );
    connect(ui->actionEditDelete    , &QAction::triggered, this, &mbServerUi::menuSlotEditDelete   );
    connect(ui->actionEditSelectAll , &QAction::triggered, this, &mbServerUi::menuSlotEditSelectAll);

    // Menu View
    connect(ui->actionViewProject, &QAction::triggered, this, &mbServerUi::menuSlotViewProject);
    connect(ui->actionViewActions, &QAction::triggered, this, &mbServerUi::menuSlotViewActions);
    connect(ui->actionViewLogView, &QAction::triggered, this, &mbServerUi::menuSlotViewLogView);

    // Menu Port
    connect(ui->actionPortNew         , &QAction::triggered, this, &mbServerUi::menuSlotPortNew         );
    connect(ui->actionPortEdit        , &QAction::triggered, this, &mbServerUi::menuSlotPortEdit        );
    connect(ui->actionPortDelete      , &QAction::triggered, this, &mbServerUi::menuSlotPortDelete      );
    connect(ui->actionPortDeviceNew   , &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceNew   );
    connect(ui->actionPortDeviceAdd   , &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceAdd   );
    connect(ui->actionPortDeviceEdit  , &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceEdit  );
    connect(ui->actionPortDeviceDelete, &QAction::triggered, this, &mbServerUi::menuSlotPortDeviceDelete);
    connect(ui->actionPortImport      , &QAction::triggered, this, &mbServerUi::menuSlotPortImport      );
    connect(ui->actionPortExport      , &QAction::triggered, this, &mbServerUi::menuSlotPortExport      );

    // Menu Device
    connect(ui->actionDeviceNew           , &QAction::triggered, this, &mbServerUi::menuSlotDeviceNew           );
    connect(ui->actionDeviceEdit          , &QAction::triggered, this, &mbServerUi::menuSlotDeviceEdit          );
    connect(ui->actionDeviceDelete        , &QAction::triggered, this, &mbServerUi::menuSlotDeviceDelete        );
    connect(ui->actionDeviceImport        , &QAction::triggered, this, &mbServerUi::menuSlotDeviceImport        );
    connect(ui->actionDeviceExport        , &QAction::triggered, this, &mbServerUi::menuSlotDeviceExport        );
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

    // Menu DataView
    connect(ui->actionDataViewItemNew    , &QAction::triggered, this, &mbServerUi::menuSlotDataViewItemNew    );
    connect(ui->actionDataViewItemEdit   , &QAction::triggered, this, &mbServerUi::menuSlotDataViewItemEdit   );
    connect(ui->actionDataViewItemInsert , &QAction::triggered, this, &mbServerUi::menuSlotDataViewItemInsert );
    connect(ui->actionDataViewItemDelete , &QAction::triggered, this, &mbServerUi::menuSlotDataViewItemDelete );
    connect(ui->actionDataViewImportItems, &QAction::triggered, this, &mbServerUi::menuSlotDataViewImportItems);
    connect(ui->actionDataViewExportItems, &QAction::triggered, this, &mbServerUi::menuSlotDataViewExportItems);
    connect(ui->actionDataViewNew        , &QAction::triggered, this, &mbServerUi::menuSlotDataViewNew        );
    connect(ui->actionDataViewEdit       , &QAction::triggered, this, &mbServerUi::menuSlotDataViewEdit       );
    connect(ui->actionDataViewInsert     , &QAction::triggered, this, &mbServerUi::menuSlotDataViewInsert     );
    connect(ui->actionDataViewDelete     , &QAction::triggered, this, &mbServerUi::menuSlotDataViewDelete     );
    connect(ui->actionDataViewImport     , &QAction::triggered, this, &mbServerUi::menuSlotDataViewImport     );
    connect(ui->actionDataViewExport     , &QAction::triggered, this, &mbServerUi::menuSlotDataViewExport     );

    // Menu Tools
    connect(ui->actionToolsSettings, &QAction::triggered, this, &mbServerUi::menuSlotToolsSettings);

    // Menu Runtime
    connect(ui->actionRuntimeStartStop  , &QAction::triggered, this, &mbServerUi::menuSlotRuntimeStartStop  );

    // Menu Window
    connect(ui->actionWindowDeviceShowAll      , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceShowAll      );
    connect(ui->actionWindowDeviceShowActive   , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceShowActive   );
    connect(ui->actionWindowDeviceCloseAll     , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceCloseAll     );
    connect(ui->actionWindowDeviceCloseActive  , &QAction::triggered, this, &mbServerUi::menuSlotWindowDeviceCloseActive  );
    connect(ui->actionWindowDataViewShowAll    , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewShowAll    );
    connect(ui->actionWindowDataViewShowActive , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewShowActive );
    connect(ui->actionWindowDataViewCloseAll   , &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewCloseAll   );
    connect(ui->actionWindowDataViewCloseActive, &QAction::triggered, this, &mbServerUi::menuSlotWindowDataViewCloseActive);
    connect(ui->actionWindowShowAll            , &QAction::triggered, this, &mbServerUi::menuSlotWindowShowAll            );
    connect(ui->actionWindowShowActive         , &QAction::triggered, this, &mbServerUi::menuSlotWindowShowActive         );
    connect(ui->actionWindowCloseAll           , &QAction::triggered, this, &mbServerUi::menuSlotWindowCloseAll           );
    connect(ui->actionWindowCloseActive        , &QAction::triggered, this, &mbServerUi::menuSlotWindowCloseActive        );
    connect(ui->actionWindowCascade            , &QAction::triggered, this, &mbServerUi::menuSlotWindowCascade            );
    connect(ui->actionWindowTile               , &QAction::triggered, this, &mbServerUi::menuSlotWindowTile               );

    // Menu Help
    connect(ui->actionHelpAbout   , &QAction::triggered, this, &mbServerUi::menuSlotHelpAbout   );
    connect(ui->actionHelpAboutQt , &QAction::triggered, this, &mbServerUi::menuSlotHelpAboutQt );
    connect(ui->actionHelpContents, &QAction::triggered, this, &mbServerUi::menuSlotHelpContents);

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

    m_ui.statusbar              = ui->statusbar             ;
    m_ui.actionRuntimeStartStop = ui->actionRuntimeStartStop;
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
    mbServerDeviceRef *device = projectUi()->currentDevice();
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
    mbServerDeviceRef *device = projectUi()->currentDevice();
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

void mbServerUi::contextMenuPort(mbCorePort * /*port*/)
{
    QMenu mn(m_projectUi);
    Q_FOREACH(QAction *a, ui->menuPort->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
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

void mbServerUi::contextMenuDataView(mbCoreDataViewUi *dataViewUi)
{
    QMenu mn(dataViewUi);  // Note: be careful to delete deviceUi while his child 'QMenu' in stack
                            //       User can choose 'actionDeleteDevice' and program can crash
                            // Solution: don't use direct 'delete deviceUi', use 'deviceUi->deleteLater'
    Q_FOREACH(QAction *a, ui->menuDataView->actions())
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
