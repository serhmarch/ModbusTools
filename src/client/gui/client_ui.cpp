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
#include "dataview/client_dataviewui.h"

#include "client_windowmanager.h"

#include "scanner/client_scannerui.h"

mbClientUi::mbClientUi(mbClient *core, QWidget *parent) :
    mbCoreUi (core, parent),
    ui(new Ui::mbClientUi)
{
    ui->setupUi(this);

    m_lbSystemName  = nullptr;
    m_lbSystemStatus= nullptr;
    m_projectFileFilter = "Client Project (*.pjc);;" + m_projectFileFilter;
    m_helpFile = QStringLiteral("/help/ModbusClient.qhc");
}

mbClientUi::~mbClientUi()
{
    delete ui;
}

void mbClientUi::initialize()
{
    mbCoreUi::initialize();

    // LogView
    ui->dockLogView->setWidget(logView());

    // Dialogs
    m_dialogs = new mbClientDialogs(this);

    // Project Ui
    m_projectUi = new mbClientProjectUi(this);
    connect(projectUi(), &mbClientProjectUi::portDoubleClick  , this, &mbClientUi::menuSlotPortEdit  );
    connect(projectUi(), &mbClientProjectUi::deviceDoubleClick, this, &mbClientUi::menuSlotDeviceEdit);
    connect(projectUi(), &mbClientProjectUi::portContextMenu  , this, &mbClientUi::contextMenuPort   );
    connect(projectUi(), &mbClientProjectUi::deviceContextMenu, this, &mbClientUi::contextMenuDevice );
    ui->dockProject->setWidget(m_projectUi);

    m_dataViewManager = new mbClientDataViewManager(this);
    connect(dataViewManager(), &mbClientDataViewManager::dataViewUiContextMenu, this, &mbClientUi::contextMenuDataView);

    m_windowManager = new mbClientWindowManager(this, dataViewManager());
    this->setCentralWidget(m_windowManager->centralWidget());

    // Menu File
    ui->actionFileNew   ->setShortcuts(QKeySequence::New   );
    ui->actionFileOpen  ->setShortcuts(QKeySequence::Open  );
    ui->actionFileSave  ->setShortcuts(QKeySequence::Save  );
    ui->actionFileSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionEdit      ->setShortcut (QKeySequence(Qt::CTRL | Qt::Key_E));
    ui->actionQuit      ->setShortcuts(QKeySequence::Quit);

    connect(ui->actionFileNew   , &QAction::triggered, this, &mbClientUi::menuSlotFileNew   );
    connect(ui->actionFileOpen  , &QAction::triggered, this, &mbClientUi::menuSlotFileOpen  );
    connect(ui->actionFileSave  , &QAction::triggered, this, &mbClientUi::menuSlotFileSave  );
    connect(ui->actionFileSaveAs, &QAction::triggered, this, &mbClientUi::menuSlotFileSaveAs);
    connect(ui->actionEdit      , &QAction::triggered, this, &mbClientUi::menuSlotFileEdit  );
    connect(ui->actionQuit      , &QAction::triggered, this, &mbClientUi::menuSlotFileQuit  );

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

  //connect(ui->actionEditUndo      , &QAction::triggered, this, &mbClientUi::menuSlotEditUndo     );
  //connect(ui->actionEditRedo      , &QAction::triggered, this, &mbClientUi::menuSlotEditRedo     );
    connect(ui->actionEditCut       , &QAction::triggered, this, &mbClientUi::menuSlotEditCut      );
    connect(ui->actionEditCopy      , &QAction::triggered, this, &mbClientUi::menuSlotEditCopy     );
    connect(ui->actionEditPaste     , &QAction::triggered, this, &mbClientUi::menuSlotEditPaste    );
    connect(ui->actionEditInsert    , &QAction::triggered, this, &mbClientUi::menuSlotEditInsert   );
  //connect(ui->actionEditEdit      , &QAction::triggered, this, &mbClientUi::menuSlotEditEdit     );
    connect(ui->actionEditDelete    , &QAction::triggered, this, &mbClientUi::menuSlotEditDelete   );
    connect(ui->actionEditSelectAll , &QAction::triggered, this, &mbClientUi::menuSlotEditSelectAll);

    // Menu View
    connect(ui->actionViewProject, &QAction::triggered, this, &mbClientUi::menuSlotViewProject);
    connect(ui->actionViewLogView, &QAction::triggered, this, &mbClientUi::menuSlotViewLogView);

    // Menu Port
    ui->actionPortDelete->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Delete));

    connect(ui->actionPortNew            , &QAction::triggered, this, &mbClientUi::menuSlotPortNew           );
    connect(ui->actionPortEdit           , &QAction::triggered, this, &mbClientUi::menuSlotPortEdit          );
    connect(ui->actionPortDelete         , &QAction::triggered, this, &mbClientUi::menuSlotPortDelete        );
    connect(ui->actionPortNewDevice      , &QAction::triggered, this, &mbClientUi::menuSlotPortNewDevice     );
    connect(ui->actionPortClearAllDevices, &QAction::triggered, this, &mbClientUi::menuSlotPortClearAllDevice);
    connect(ui->actionPortImport         , &QAction::triggered, this, &mbClientUi::menuSlotPortImport        );
    connect(ui->actionPortExport         , &QAction::triggered, this, &mbClientUi::menuSlotPortExport        );

    // Menu Device
    ui->actionDeviceNew   ->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_N                 ));
    ui->actionDeviceEdit  ->setShortcut(QKeySequence(Qt::CTRL  | Qt::SHIFT | Qt::Key_N     ));
    ui->actionDeviceDelete->setShortcut(QKeySequence(Qt::SHIFT | Qt::CTRL  | Qt::Key_Delete));

    connect(ui->actionDeviceNew   , &QAction::triggered, this, &mbClientUi::menuSlotDeviceNew   );
    connect(ui->actionDeviceEdit  , &QAction::triggered, this, &mbClientUi::menuSlotDeviceEdit  );
    connect(ui->actionDeviceDelete, &QAction::triggered, this, &mbClientUi::menuSlotDeviceDelete);
    connect(ui->actionDeviceImport, &QAction::triggered, this, &mbClientUi::menuSlotDeviceImport);
    connect(ui->actionDeviceExport, &QAction::triggered, this, &mbClientUi::menuSlotDeviceExport);

    // Menu DataView
    connect(ui->actionDataViewItemNew    , &QAction::triggered, this, &mbClientUi::menuSlotDataViewItemNew    );
    connect(ui->actionDataViewItemEdit   , &QAction::triggered, this, &mbClientUi::menuSlotDataViewItemEdit   );
    connect(ui->actionDataViewItemInsert , &QAction::triggered, this, &mbClientUi::menuSlotDataViewItemInsert );
    connect(ui->actionDataViewItemDelete , &QAction::triggered, this, &mbClientUi::menuSlotDataViewItemDelete );
    connect(ui->actionDataViewImportItems, &QAction::triggered, this, &mbClientUi::menuSlotDataViewImportItems);
    connect(ui->actionDataViewExportItems, &QAction::triggered, this, &mbClientUi::menuSlotDataViewExportItems);
    connect(ui->actionDataViewNew        , &QAction::triggered, this, &mbClientUi::menuSlotDataViewNew        );
    connect(ui->actionDataViewEdit       , &QAction::triggered, this, &mbClientUi::menuSlotDataViewEdit       );
    connect(ui->actionDataViewInsert     , &QAction::triggered, this, &mbClientUi::menuSlotDataViewInsert     );
    connect(ui->actionDataViewDelete     , &QAction::triggered, this, &mbClientUi::menuSlotDataViewDelete     );
    connect(ui->actionDataViewImport     , &QAction::triggered, this, &mbClientUi::menuSlotDataViewImport     );
    connect(ui->actionDataViewExport     , &QAction::triggered, this, &mbClientUi::menuSlotDataViewExport     );

    // Menu Tools
    connect(ui->actionToolsSettings, &QAction::triggered, this, &mbClientUi::menuSlotToolsSettings);
    connect(ui->actionToolsScanner , &QAction::triggered, this, &mbClientUi::menuSlotToolsScanner );

    // Menu Runtime
    connect(ui->actionRuntimeStartStop  , &QAction::triggered, this, &mbClientUi::menuSlotRuntimeStartStop  );
    connect(ui->actionRuntimeSendMessage, &QAction::triggered, this, &mbClientUi::menuSlotRuntimeSendMessage);

    // Menu Window
    connect(ui->actionWindowShowAll     , &QAction::triggered, this, &mbClientUi::menuSlotWindowShowAll    );
    connect(ui->actionWindowShowActive  , &QAction::triggered, this, &mbClientUi::menuSlotWindowShowActive );
    connect(ui->actionWindowCloseAll    , &QAction::triggered, this, &mbClientUi::menuSlotWindowCloseAll   );
    connect(ui->actionWindowCloseActive , &QAction::triggered, this, &mbClientUi::menuSlotWindowCloseActive);
    connect(ui->actionWindowCascade     , &QAction::triggered, this, &mbClientUi::menuSlotWindowCascade    );
    connect(ui->actionWindowTile        , &QAction::triggered, this, &mbClientUi::menuSlotWindowTile       );

    // Menu Help
    connect(ui->actionHelpAbout   , &QAction::triggered, this, &mbClientUi::menuSlotHelpAbout   );
    connect(ui->actionHelpAboutQt , &QAction::triggered, this, &mbClientUi::menuSlotHelpAboutQt );
    connect(ui->actionHelpContents, &QAction::triggered, this, &mbClientUi::menuSlotHelpContents);

    // status bar
    m_lbSystemName = new QLabel("Status", ui->statusbar);
    m_lbSystemStatus = new QLabel(ui->statusbar);
    m_lbSystemStatus->setFrameShape(QFrame::Panel);
    m_lbSystemStatus->setFrameStyle(QFrame::Sunken);
    m_lbSystemStatus->setAutoFillBackground(true);
    statusChange(m_core->status());
    ui->statusbar->addPermanentWidget(m_lbSystemName);
    ui->statusbar->addPermanentWidget(m_lbSystemStatus, 1);

    connect(m_core, &mbClient::statusChanged, this, &mbClientUi::statusChange);

    m_scannerUi = new mbClientScannerUi(this);
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

void mbClientUi::menuSlotEditDelete()
{
    if (core()->isRunning())
        return;
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
                return;
            project->portRemove(port);
            delete port;
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
            Q_FOREACH(mbClientDevice* d, devices)
                port->deviceRemove(d);
            mbClientProject* project = core()->project();
            Q_FOREACH(mbClientDevice* d, devices)
                project->deviceRemove(d);
            qDeleteAll(devices);
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
            mbClientPort *port = d->port();
            if (port)
                port->deviceRemove(d);
            prj->deviceRemove(d);
            delete d;
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
    QString file = m_dialogs->getImportFileName(this, QStringLiteral("Import Device"));
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
        QString file = m_dialogs->getExportFileName(this, QString("Export Device '%1'").arg(current->name()));
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

void mbClientUi::menuSlotToolsScanner()
{
    m_scannerUi->show();
}

void mbClientUi::menuSlotRuntimeSendMessage()
{
    dialogs()->sendMessage();
}

void mbClientUi::statusChange(int status)
{
    switch (status)
    {
    case mbClient::Running:
        //break; no need break
    case mbClient::Stopping:
    {
        //QPalette palette = m_lbSystemStatus->palette();
        QPalette palette = this->palette();
        palette.setColor(m_lbSystemStatus->backgroundRole(), Qt::green);
        //palette.setColor(m_lbSystemStatus->foregroundRole(), Qt::green);
        m_lbSystemStatus->setPalette(palette);
        ui->actionRuntimeStartStop->setText("Stop");
        ui->actionRuntimeStartStop->setIcon(QIcon(":/core/icons/stop.png"));
    }
        break;
    case mbClient::Stopped:
    {
        //QPalette palette = m_lbSystemStatus->palette();
        QPalette palette = this->palette();
        palette.setColor(m_lbSystemStatus->backgroundRole(), Qt::gray);
        //palette.setColor(m_lbSystemStatus->foregroundRole(), Qt::yellow);
        m_lbSystemStatus->setPalette(palette);
        ui->actionRuntimeStartStop->setText("Start");
        ui->actionRuntimeStartStop->setIcon(QIcon(":/core/icons/play.png"));
    }
        break;
    case mbClient::NoProject:
    {
        //QPalette palette = m_lbSystemStatus->palette();
        QPalette palette = this->palette();
        palette.setColor(m_lbSystemStatus->backgroundRole(), Qt::yellow);
        //palette.setColor(m_lbSystemStatus->foregroundRole(), Qt::yellow);
        m_lbSystemStatus->setPalette(palette);
        ui->actionRuntimeStartStop->setText("Start");
        ui->actionRuntimeStartStop->setIcon(QIcon(":/core/icons/play.png"));
    }
        break;
    }
    m_lbSystemStatus->setText(mb::enumKeyTypeStr<mbClient::Status>(status));
}

void mbClientUi::contextMenuPort(mbCorePort */*port*/)
{
    QMenu mn(m_projectUi);
    Q_FOREACH(QAction *a, ui->menuPort->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbClientUi::contextMenuDevice(mbClientDevice */*device*/)
{
    QMenu mn(m_projectUi);
    Q_FOREACH(QAction *a, ui->menuDevice->actions())
        mn.addAction(a);
    mn.exec(QCursor::pos());
}

void mbClientUi::contextMenuDataView(mbCoreDataViewUi *dataViewUi)
{
    QMenu mn(dataViewUi);  // Note: be careful to delete deviceUi while his child 'QMenu' in stack
        //       User can choose 'actionDeleteDevice' and program can crash
        // Solution: don't use direct 'delete deviceUi', use 'deviceUi->deleteLater'
    Q_FOREACH(QAction *a, ui->menuDataView->actions())
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
