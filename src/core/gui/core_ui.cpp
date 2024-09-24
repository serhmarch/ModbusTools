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
#include "core_ui.h"

#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QBuffer>
#include <QAction>
#include <QMenu>

#include <core.h>

#include <project/core_builder.h>
#include <project/core_project.h>
#include <project/core_port.h>
#include <project/core_device.h>
#include <project/core_dataview.h>

#include "dialogs/core_dialogs.h"
#include "dialogs/core_dialogdataviewitem.h"

#include "project/core_projectui.h"
#include "dataview/core_dataviewmanager.h"
#include "dataview/core_dataviewui.h"
#include "help/core_helpui.h"

#include "core_windowmanager.h"
#include "core_logview.h"

mbCoreUi::Strings::Strings() :
    settings_useNameWithSettings(QStringLiteral("Ui.useNameWithSettings")),
    wGeometry(QStringLiteral("Ui.geometry")),
    wState(QStringLiteral("Ui.windowState"))
{
}

const mbCoreUi::Strings &mbCoreUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreUi::Defaults::Defaults() :
    settings_useNameWithSettings(true)
{
}

const mbCoreUi::Defaults &mbCoreUi::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCoreUi::mbCoreUi(mbCore *core, QWidget *parent) :
    QMainWindow(parent),
    m_core(core)
{
    m_logView = new mbCoreLogView(this);
    m_builder = m_core->builderCore();
    m_dialogs = nullptr;
    m_windowManager = nullptr;
    m_dataViewManager = nullptr;
    m_currentPort = nullptr;
    m_projectUi = nullptr;
    m_tray = nullptr;
    m_projectFileFilter = "XML files (*.xml)";
    m_help = nullptr;
}

mbCoreUi::~mbCoreUi()
{
    delete m_dialogs;
}

QWidget *mbCoreUi::logView() const
{
    return m_logView;
}

void mbCoreUi::initialize()
{
    m_help = new mbCoreHelpUi(m_helpFile, this);

    connect(m_projectUi, &mbCoreProjectUi::currentPortChanged, this, &mbCoreUi::currentPortChanged);

    // status bar
    m_lbSystemStatus = new QLabel(m_ui.statusbar);
    m_lbSystemStatus->setFrameShape(QFrame::Panel);
    m_lbSystemStatus->setFrameStyle(QFrame::Sunken);
    m_lbSystemStatus->setAutoFillBackground(true);
    m_lbSystemStatus->setMinimumWidth(100);

    m_lbPortName = new QLabel("Port", m_ui.statusbar);
    m_lbPortName->setFrameShape(QFrame::Panel);
    m_lbPortName->setFrameStyle(QFrame::Sunken);
    m_lbPortName->setAutoFillBackground(true);
    m_lbPortName->setMinimumWidth(100);

    m_lbPortStatTx = new QLabel("0", m_ui.statusbar);
    m_lbPortStatTx->setFrameShape(QFrame::Panel);
    m_lbPortStatTx->setFrameStyle(QFrame::Sunken);
    m_lbPortStatTx->setAutoFillBackground(true);
    m_lbPortStatTx->setMinimumWidth(70);

    m_lbPortStatRx = new QLabel("0", m_ui.statusbar);
    m_lbPortStatRx->setFrameShape(QFrame::Panel);
    m_lbPortStatRx->setFrameStyle(QFrame::Sunken);
    m_lbPortStatRx->setAutoFillBackground(true);
    m_lbPortStatRx->setMinimumWidth(70);

    statusChange(m_core->status());
    m_ui.statusbar->addPermanentWidget(m_lbPortName);
    m_ui.statusbar->addPermanentWidget(new QLabel("Tx: ", m_ui.statusbar));
    m_ui.statusbar->addPermanentWidget(m_lbPortStatTx);
    m_ui.statusbar->addPermanentWidget(new QLabel("Rx: ", m_ui.statusbar));
    m_ui.statusbar->addPermanentWidget(m_lbPortStatRx);
    m_ui.statusbar->addPermanentWidget(new QLabel("Status: ", m_ui.statusbar));
    m_ui.statusbar->addPermanentWidget(m_lbSystemStatus, 1);
    refreshCurrentPortName();

    connect(m_core, SIGNAL(statusChanged(int)), this, SLOT(statusChange(int)));

    const bool tray = false;
    if (m_core->args().value(mbCore::Arg_Tray, tray).toBool())
    {
        m_tray = new QSystemTrayIcon(this);
        QMenu* menu = new QMenu(this);
        QAction* actionShow = new QAction("Show", menu);
        connect(actionShow, SIGNAL(triggered()), this, SLOT(show()));
        QAction* actionQuit = new QAction("Quit", menu);
        connect(actionShow, SIGNAL(triggered()), this, SLOT(fileQuit()));
        connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
        menu->addAction(actionShow);
        menu->addSeparator();
        menu->addAction(actionQuit);
        m_tray->setContextMenu(menu);
        m_tray->setToolTip(m_core->applicationName());
        m_tray->setIcon(this->windowIcon());
        m_tray->show();
        qApp->setQuitOnLastWindowClosed(false);
    }
}

bool mbCoreUi::useNameWithSettings() const
{
    return m_projectUi->useNameWithSettings();
}

void mbCoreUi::setUseNameWithSettings(bool use)
{
    m_projectUi->setUseNameWithSettings(use);
    refreshCurrentPortName();
}

MBSETTINGS mbCoreUi::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r = m_dialogs->cachedSettings();
    r[s.settings_useNameWithSettings] = useNameWithSettings();
    r[s.wGeometry] = this->saveGeometry();
    r[s.wState   ] = this->saveState();

    return r;
}

void mbCoreUi::setCachedSettings(const MBSETTINGS &settings)
{
    Strings s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    //bool ok;

    it = settings.find(s.settings_useNameWithSettings);
    if (it != end)
    {
        bool v = it.value().toBool();
        setUseNameWithSettings(v);
    }

    it = settings.find(s.wGeometry);
    if (it != end)
    {
        this->restoreGeometry(it.value().toByteArray());
    }

    it = settings.find(s.wState);
    if (it != end)
    {
        this->restoreState(it.value().toByteArray());
    }

    m_dialogs->setCachedSettings(settings);
}

void mbCoreUi::showMessage(const QString &message)
{
    m_logView->showMessage(message);
}

void mbCoreUi::menuSlotFileNew()
{
    if (m_core->isRunning())
        return;
    MBSETTINGS settings = m_dialogs->getProject(MBSETTINGS(), QStringLiteral("New Project"));
    if (settings.count())
    {
        mbCoreProject* p = m_builder->newProject();
        p->setSettings(settings);
        m_core->setProjectCore(p);
    }
}

void mbCoreUi::menuSlotFileOpen()
{
    if (m_core->isRunning())
        return;
    QString file = m_dialogs->getOpenFileName(this, QStringLiteral("Open project..."), QString(), m_projectFileFilter);
    if (!file.isEmpty())
    {
        mbCoreProject* p = m_core->builderCore()->loadCore(file);
        if (p)
        {
            m_core->setProjectCore(p);
        }
    }
}

void mbCoreUi::menuSlotFileSave()
{
    mbCoreProject* p = m_core->projectCore();
    if (p)
    {
        if (p->absoluteFilePath().isEmpty())
        {
            menuSlotFileSaveAs();
            return;
        }
        m_core->builderCore()->saveCore(p);
    }
}

void mbCoreUi::menuSlotFileSaveAs()
{
    mbCoreProject* p = m_core->projectCore();
    if (p)
    {
        QString file = m_dialogs->getSaveFileName(this, QStringLiteral("Save project..."), QString(), m_projectFileFilter);
        if (file.isEmpty())
            return;
        p->setAbsoluteFilePath(file);
        menuSlotFileSave();
    }
}

void mbCoreUi::menuSlotFileEdit()
{
    if (m_core->isRunning())
        return;
    mbCoreProject* p = m_core->projectCore();
    if (p)
    {
        MBSETTINGS old = p->settings();
        MBSETTINGS cur = m_dialogs->getProject(old);

        if (cur.count())
        {
            p->setSettings(cur);
        }
    }
}

void mbCoreUi::menuSlotFileQuit()
{
    m_core->application()->quit();
}

void mbCoreUi::menuSlotEditUndo()
{
    
}

void mbCoreUi::menuSlotEditRedo()
{
    
}

void mbCoreUi::menuSlotEditCut()
{
    menuSlotEditCopy();
    menuSlotEditDelete();
}

void mbCoreUi::menuSlotEditCopy()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        auto selectedItems = ui->selectedItemsCore();
        if (selectedItems.count())
        {
            QBuffer buff;
            buff.open(QIODevice::ReadWrite);
            m_builder->exportDataViewItems(&buff, selectedItems);
            buff.seek(0);
            QByteArray b = buff.readAll();
            QApplication::clipboard()->setText(QString::fromUtf8(b));
        }
    }
}

void mbCoreUi::menuSlotEditPaste()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        QString text = QApplication::clipboard()->text();
        if (text.isEmpty())
            return;
        QByteArray b = text.toUtf8();
        QBuffer buff(&b);
        buff.open(QIODevice::ReadOnly);
        auto items = m_builder->importDataViewItems(&buff);
        if (items.count())
        {
            mbCoreDataView *dataView = ui->dataViewCore();
            int index = -1;
            auto selectedItems = ui->selectedItemsCore();
            if (selectedItems.count())
                index = dataView->itemIndex(selectedItems.first());
            dataView->itemsInsert(items, index);
        }
    }
}

void mbCoreUi::menuSlotEditInsert()
{
    menuSlotDataViewItemInsert();
}

void mbCoreUi::menuSlotEditEdit()
{
}

void mbCoreUi::menuSlotEditDelete()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        mbCoreDataView *dataView = ui->dataViewCore();
        auto selectedItems = ui->selectedItemsCore();
        dataView->itemsRemove(selectedItems);
    }
}

void mbCoreUi::menuSlotEditSelectAll()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
        ui->selectAll();
}

void mbCoreUi::menuSlotViewProject()
{
    
}

void mbCoreUi::menuSlotViewLogView()
{
    
}

void mbCoreUi::menuSlotPortNew()
{
}

void mbCoreUi::menuSlotPortEdit()
{
}

void mbCoreUi::menuSlotPortDelete()
{
}

void mbCoreUi::menuSlotPortImport()
{
    if (m_core->isRunning())
        return;
    mbCoreProject* project = m_core->projectCore();
    if (!project)
        return;
    QString file = m_dialogs->getImportFileName(this, QStringLiteral("Import Port"));
    if (!file.isEmpty())
    {
        if (mbCorePort *port = m_builder->importPort(file))
            project->portAdd(port);
    }
}

void mbCoreUi::menuSlotPortExport()
{
    if (mbCorePort *port = m_projectUi->currentPortCore())
    {
        QString file = m_dialogs->getExportFileName(this, QString("Export '%1'").arg(port->name()));
        if (!file.isEmpty())
            m_builder->exportPort(file, port);
    }
}

void mbCoreUi::menuSlotDeviceNew()
{
}

void mbCoreUi::menuSlotDeviceEdit()
{
}

void mbCoreUi::menuSlotDeviceDelete()
{
}

void mbCoreUi::menuSlotDeviceImport()
{
}

void mbCoreUi::menuSlotDeviceExport()
{
}

void ::mbCoreUi::menuSlotDataViewItemNew()
{
    MBSETTINGS p = dialogsCore()->getDataViewItem(MBSETTINGS(), "New Item(s)");
    if (p.count())
    {
        const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
        int count = p.value(mbCoreDialogDataViewItem::Strings::instance().count).toInt();
        if (count > 0)
        {
            mbCoreDataView *wl = m_dataViewManager->activeDataViewCore();
            if (!wl)
            {
                QList<mbCoreDataViewUi*> ls = m_dataViewManager->dataViewUisCore();
                if (ls.count())
                    wl = ls.first()->dataViewCore();
                else
                {
                    mbCoreProject *project = baseCore()->projectCore();
                    if (!project)
                        return;
                    wl = m_builder->newDataView();
                    project->dataViewAdd(wl);
                }

            }
            for (int i = 0; i < count; i++)
            {
                mbCoreDataViewItem *item = builderCore()->newDataViewItem();
                item->setSettings(p);
                wl->itemAdd(item);
                p[sItem.address] = item->addressInt() + item->length();
            }
        }
    }
}

void ::mbCoreUi::menuSlotDataViewItemEdit()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        QList<mbCoreDataViewItem*> items = ui->selectedItemsCore();
        if (!items.count())
            return;
        MBSETTINGS s = items.first()->settings();
        s[mbCoreDialogDataViewItem::Strings::instance().count] = items.count();
        MBSETTINGS p = dialogsCore()->getDataViewItem(s, "Edit Item(s)");
        if (p.count())
        {
            const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
            Q_FOREACH (mbCoreDataViewItem *item, items)
            {
                item->setSettings(p);
                p[sItem.address] = item->addressInt() + item->length();
            }
        }
    }
}

void ::mbCoreUi::menuSlotDataViewItemInsert()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        mbCoreDataView *dataView = ui->dataViewCore();
        int index = ui->currentItemIndex();
        mbCoreDataViewItem* next = dataView->itemCore(index);
        mbCoreDataViewItem* prev;
        if (next)
            prev = dataView->itemCore(index-1);
        else
            prev = dataView->itemCore(dataView->itemCount()-1);
        mbCoreDataViewItem* newItem;
        if (prev)
        {
            newItem = m_builder->newDataViewItem(prev);
            next = dataView->itemCore(index);
        }
        else
        {
            newItem = m_builder->newDataViewItem();
            newItem->setDeviceCore(m_core->projectCore()->deviceCore(0));
        }
        dataView->itemInsert(newItem, index);
        if (next)
            ui->selectItem(next);
    }
}

void ::mbCoreUi::menuSlotDataViewItemDelete()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        QList<mbCoreDataViewItem*> items = ui->selectedItemsCore();
        if (items.count())
        {
            mbCoreDataView *wl = ui->dataViewCore();
            Q_FOREACH (mbCoreDataViewItem *item, items)
            {
                wl->itemRemove(item);
                delete item;
            }
        }
    }
}

void mbCoreUi::menuSlotDataViewImportItems()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        QString file = m_dialogs->getImportFileName(this, QStringLiteral("Import Items"));
        if (!file.isEmpty())
        {
            auto items = m_builder->importDataViewItems(file);
            if (items.count())
            {
                mbCoreDataView *dataView = ui->dataViewCore();
                int index = ui->currentItemIndex();
                dataView->itemsInsert(items, index);
            }
        }
    }
}

void mbCoreUi::menuSlotDataViewExportItems()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
    {
        auto selectedItems = ui->selectedItemsCore();
        if (selectedItems.count())
        {
            QString file = m_dialogs->getExportFileName(this, QStringLiteral("Export Items"));
            if (!file.isEmpty())
                m_builder->exportDataViewItems(file, selectedItems);
        }
    }
}

void mbCoreUi::menuSlotDataViewNew()
{
    mbCoreProject *project = baseCore()->projectCore();
    if (!project)
        return;
    MBSETTINGS s = dialogsCore()->getDataView(MBSETTINGS(), "New Data View");
    if (s.count())
    {
        mbCoreDataView *wl = m_builder->newDataView();
        wl->setSettings(s);
        project->dataViewAdd(wl);
    }
}

void mbCoreUi::menuSlotDataViewEdit()
{
    mbCoreDataView *wl = m_dataViewManager->activeDataViewCore();
    if (wl)
    {
        MBSETTINGS p = dialogsCore()->getDataView(wl->settings(), "Edit Data View");
        if (p.count())
            wl->setSettings(p);
    }
}

void mbCoreUi::menuSlotDataViewInsert()
{
    mbCoreProject *project = baseCore()->projectCore();
    if (!project)
        return;
    mbCoreDataView *wl = m_builder->newDataView();
    project->dataViewAdd(wl);
}

void ::mbCoreUi::menuSlotDataViewDelete()
{
    mbCoreProject *project = baseCore()->projectCore();
    if (!project)
        return;
    mbCoreDataView *d = m_dataViewManager->activeDataViewCore();
    if (!d)
        return;
    project->dataViewRemove(d);
    delete d;
}

void mbCoreUi::menuSlotDataViewImport()
{
    mbCoreProject* project = m_core->projectCore();
    if (!project)
        return;
    QString file = m_dialogs->getImportFileName(this, QStringLiteral("Import Data View"));
    if (!file.isEmpty())
    {
        if (mbCoreDataView *current = m_builder->importDataView(file))
            project->dataViewAdd(current);
    }
}

void mbCoreUi::menuSlotDataViewExport()
{
    if (mbCoreDataView *current = m_dataViewManager->activeDataViewCore())
    {
        QString file = m_dialogs->getExportFileName(this, QString("Export Data View '%1'").arg(current->name()));
        if (!file.isEmpty())
            m_builder->exportDataView(file, current);
    }
}

void mbCoreUi::menuSlotToolsSettings()
{
    //if (m_core->isRunning())
    //    return;
    m_dialogs->editSystemSettings();
}

void mbCoreUi::menuSlotRuntimeStartStop()
{
    if (m_core->isRunning())
        m_core->stop();
    else
        m_core->start();
}

void mbCoreUi::menuSlotWindowDataViewShowAll()
{
    m_windowManager->actionWindowDataViewShowAll();
}

void mbCoreUi::menuSlotWindowDataViewShowActive()
{
    m_windowManager->actionWindowDataViewShowActive();
}

void mbCoreUi::menuSlotWindowDataViewCloseAll()
{
    m_windowManager->actionWindowDataViewCloseAll();
}

void mbCoreUi::menuSlotWindowDataViewCloseActive()
{
    m_windowManager->actionWindowDataViewCloseActive();
}

void mbCoreUi::menuSlotWindowShowAll()
{
    m_windowManager->actionWindowShowAll();
}

void mbCoreUi::menuSlotWindowShowActive()
{
    m_windowManager->actionWindowShowActive();
}

void mbCoreUi::menuSlotWindowCloseAll()
{
    m_windowManager->actionWindowCloseAll();
}

void mbCoreUi::menuSlotWindowCloseActive()
{
    m_windowManager->actionWindowCloseActive();
}

void mbCoreUi::menuSlotWindowCascade()
{
    m_windowManager->actionWindowCascade();
}

void mbCoreUi::menuSlotWindowTile()
{
    m_windowManager->actionWindowTile();
}

void mbCoreUi::menuSlotHelpAbout()
{
    QMessageBox::about(this, m_core->applicationName(),
                       QStringLiteral("Version: " MBTOOLS_VERSION_STR "\n") +
                       QStringLiteral("ModbusLib Version: ") + QString(Modbus::modbusLibVersionStr()) + QStringLiteral("\n")+
                       QStringLiteral("Developed by:\nSerhii Marchuk, Kyiv, Ukraine, 2023\nhttps://github.com/serhmarch"));
}

void mbCoreUi::menuSlotHelpAboutQt()
{
    QApplication::aboutQt();
}

void mbCoreUi::menuSlotHelpContents()
{
    m_help->show();
}

void mbCoreUi::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    default:
        break;
    }
}

void mbCoreUi::currentPortChanged(mbCorePort *port)
{
    mbCorePort *old = m_currentPort;
    if (old)
        old->disconnect(this);
    m_currentPort = port;
    refreshCurrentPortName();
    mbCorePort::Statistic stat = port->statistic();
    if (port)
    {
        connect(port, &mbCorePort::changed           , this, &mbCoreUi::refreshCurrentPortName);
        connect(port, &mbCorePort::statCountTxChanged, this, &mbCoreUi::setStatTx             );
        connect(port, &mbCorePort::statCountRxChanged, this, &mbCoreUi::setStatRx             );
        setStatTx(stat.countTx);
        setStatRx(stat.countRx);
    }
    else
    {
        setStatTx(stat.countTx);
        setStatRx(stat.countRx);
    }
}

void mbCoreUi::refreshCurrentPortName()
{
    if (m_currentPort)
    {
        if (useNameWithSettings())
            m_lbPortName->setText(m_currentPort->extendedName());
        else
            m_lbPortName->setText(m_currentPort->name());
    }
    else
        m_lbPortName->setText(QStringLiteral("-"));
    
}

void mbCoreUi::setStatTx(quint32 count)
{
    m_lbPortStatTx->setText(QString::number(count));
}

void mbCoreUi::setStatRx(quint32 count)
{
    m_lbPortStatRx->setText(QString::number(count));
}

void mbCoreUi::statusChange(int status)
{
    switch (status)
    {
    case mbCore::Running:
        //break; no need break
    case mbCore::Stopping:
    {
        //QPalette palette = m_lbSystemStatus->palette();
        QPalette palette = this->palette();
        palette.setColor(m_lbSystemStatus->backgroundRole(), Qt::green);
        //palette.setColor(m_lbSystemStatus->foregroundRole(), Qt::green);
        m_lbSystemStatus->setPalette(palette);
        m_ui.actionRuntimeStartStop->setText("Stop");
        m_ui.actionRuntimeStartStop->setIcon(QIcon(":/core/icons/stop.png"));
    }
    break;
    case mbCore::Stopped:
    {
        //QPalette palette = m_lbSystemStatus->palette();
        QPalette palette = this->palette();
        palette.setColor(m_lbSystemStatus->backgroundRole(), Qt::gray);
        //palette.setColor(m_lbSystemStatus->foregroundRole(), Qt::gray);
        m_lbSystemStatus->setPalette(palette);
        m_ui.actionRuntimeStartStop->setText("Start");
        m_ui.actionRuntimeStartStop->setIcon(QIcon(":/core/icons/play.png"));
    }
    break;
    case mbCore::NoProject:
    {
        //QPalette palette = m_lbSystemStatus->palette();
        QPalette palette = this->palette();
        palette.setColor(m_lbSystemStatus->backgroundRole(), Qt::yellow);
        //palette.setColor(m_lbSystemStatus->foregroundRole(), Qt::yellow);
        m_lbSystemStatus->setPalette(palette);
        m_ui.actionRuntimeStartStop->setText("Start");
        m_ui.actionRuntimeStartStop->setIcon(QIcon(":/core/icons/play.png"));
    }
    break;
    }
    m_lbSystemStatus->setText(mb::enumKeyTypeStr<mbCore::Status>(status));
}

