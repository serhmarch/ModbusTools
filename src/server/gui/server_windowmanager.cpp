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
#include "script/server_scriptmanager.h"
#include "script/server_scriptmoduleeditor.h"
#include "script/server_devicescripteditor.h"
#include "dataview/server_dataviewmanager.h"

#include <project/server_project.h>

mbServerWindowManager::Strings::Strings() : mbCoreWindowManager::Strings(),
    prefixDevice(QStringLiteral("dev:")),
    prefixScriptModule(QStringLiteral("module:")),
    prefixScriptInit(QStringLiteral("init:")),
    prefixScriptLoop(QStringLiteral("loop:")),
    prefixScriptFinal(QStringLiteral("final:"))
{
}

const mbServerWindowManager::Strings &mbServerWindowManager::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerWindowManager::mbServerWindowManager(mbServerUi *ui, mbServerDeviceManager *deviceManager, mbServerScriptManager *scriptManager, mbServerDataViewManager *dataViewManager) :
    mbCoreWindowManager(ui, dataViewManager)
{
    m_deviceManager = deviceManager;
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiAdd, this, &mbServerWindowManager::deviceUiAdd);
    connect(m_deviceManager, &mbServerDeviceManager::deviceUiRemove, this, &mbServerWindowManager::deviceUiRemove);
    Q_FOREACH (mbServerDeviceUi *ui, m_deviceManager->deviceUis())
        deviceUiAdd(ui);

    m_scriptManager = scriptManager;
    connect(m_scriptManager, &mbServerScriptManager::scriptEditorAdd, this, &mbServerWindowManager::scriptEditorAdd);
    connect(m_scriptManager, &mbServerScriptManager::scriptEditorRemove, this, &mbServerWindowManager::scriptEditorRemove);
    Q_FOREACH (mbServerBaseScriptEditor *ui, m_scriptManager->scriptEditors())
        scriptEditorAdd(ui);
}

QMdiSubWindow *mbServerWindowManager::getMdiSubWindowForNameWithPrefix(const QString &nameWithPrefix) const
{
    const Strings &s = Strings::instance();
    if (nameWithPrefix.startsWith(s.prefixDevice))
    {
        mbServerDeviceUi *ui = m_deviceManager->deviceUi(nameWithPrefix.mid(s.prefixDevice.size()));
        return m_hashWindows.value(ui);
    }
    if (nameWithPrefix.startsWith(s.prefixScriptModule))
    {
        QString name = nameWithPrefix.mid(s.prefixScriptModule.size());
        mbServerScriptModule *sm = ui()->project()->scriptModule(name);
        if (sm)
        {
            mbServerBaseScriptEditor *ui = m_scriptManager->getOrCreateScriptModuleEditor(sm);
            return m_hashWindows.value(ui);
        }
    }
    if (nameWithPrefix.startsWith(s.prefixScriptInit))
    {
        QString name = nameWithPrefix.mid(s.prefixScriptInit.size());
        mbServerDevice *device = ui()->project()->device(name);
        if (device)
        {
            mbServerBaseScriptEditor *ui = m_scriptManager->getOrCreateDeviceScriptEditor(device, mbServerDevice::Script_Init);
            return m_hashWindows.value(ui);
        }
    }
    if (nameWithPrefix.startsWith(s.prefixScriptLoop))
    {
        QString deviceName = nameWithPrefix.mid(s.prefixScriptLoop.size());
        mbServerDevice *device = ui()->project()->device(deviceName);
        if (device)
        {
            mbServerBaseScriptEditor *ui = m_scriptManager->getOrCreateDeviceScriptEditor(device, mbServerDevice::Script_Loop);
            return m_hashWindows.value(ui);
        }
    }
    if (nameWithPrefix.startsWith(s.prefixScriptFinal))
    {
        QString deviceName = nameWithPrefix.mid(s.prefixScriptFinal.size());
        mbServerDevice *device = ui()->project()->device(deviceName);
        if (device)
        {
            mbServerBaseScriptEditor *ui = m_scriptManager->getOrCreateDeviceScriptEditor(device, mbServerDevice::Script_Final);
            return m_hashWindows.value(ui);
        }
    }
    return mbCoreWindowManager::getMdiSubWindowForNameWithPrefix(nameWithPrefix);
}

QString mbServerWindowManager::getMdiSubWindowNameWithPrefix(const QMdiSubWindow *sw) const
{
    if (mbServerDeviceUi *ui = qobject_cast<mbServerDeviceUi *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        return s.prefixDevice+ui->name();
    }
    if (mbServerScriptModuleEditor *ui = qobject_cast<mbServerScriptModuleEditor *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        return s.prefixScriptModule+ui->scriptModule()->name();
    }
    if (mbServerDeviceScriptEditor *ui = qobject_cast<mbServerDeviceScriptEditor *>(sw->widget()))
    {
        const Strings &s = Strings::instance();
        QString prefix;
        switch (ui->scriptType())
        {
        case mbServerDevice::Script_Init : prefix = s.prefixScriptInit ; break;
        case mbServerDevice::Script_Loop : prefix = s.prefixScriptLoop ; break;
        case mbServerDevice::Script_Final: prefix = s.prefixScriptFinal; break;
        }

        return prefix+ui->device()->name();
    }
    return mbCoreWindowManager::getMdiSubWindowNameWithPrefix(sw);
}

bool mbServerWindowManager::find(const QString &text, int findFlags)
{
    mbServerBaseScriptEditor *se = m_scriptManager->activeScriptEditor();
    if (se)
    {
        QTextDocument::FindFlags tFindFlags;
        if (findFlags & mb::FindBackward       ) tFindFlags |= QTextDocument::FindBackward       ;
        if (findFlags & mb::FindCaseSensitively) tFindFlags |= QTextDocument::FindCaseSensitively;
        if (findFlags & mb::FindWholeWords     ) tFindFlags |= QTextDocument::FindWholeWords     ;
        return se->find(text, tFindFlags);
    }
    return false;
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

void mbServerWindowManager::showScriptModule(mbServerScriptModule *sm)
{
    mbServerScriptModuleEditor *se = m_scriptManager->scriptModuleEditor(sm);
    if (se)
        setActiveScriptEditor(se);
    else
        m_scriptManager->addScriptModuleEditor(sm);
}

void mbServerWindowManager::showDeviceScript(mbServerDevice *device, mbServerDevice::ScriptType scriptType)
{
    mbServerDeviceScriptEditor *se = m_scriptManager->deviceScriptEditor(device, scriptType);
    if (se)
        setActiveScriptEditor(se);
    else
        m_scriptManager->addDeviceScriptEditor(device, scriptType);
}

void mbServerWindowManager::showScriptEditor(mbServerBaseScriptEditor *scriptEditor)
{
    showSubWindow(scriptEditor);
}

void mbServerWindowManager::setActiveScriptEditor(mbServerBaseScriptEditor *scriptEditor)
{
    QMdiSubWindow *sw = m_hashWindows.value(scriptEditor);
    if (sw)
        m_area->setActiveSubWindow(sw);
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
    QMdiSubWindow* sw = subWindowAdd(ui);
    m_devices.append(sw);
    connect(ui, &mbServerDeviceUi::nameChanged, sw, &QWidget::setWindowTitle);
    sw->setWindowTitle(ui->name());
    Q_EMIT deviceWindowAdded(ui);
}

void mbServerWindowManager::deviceUiRemove(mbServerDeviceUi *ui)
{
    ui->disconnect(this);
    QMdiSubWindow* sw = subWindowRemove(ui);
    if (sw)
    {
        Q_EMIT deviceWindowRemoving(ui);
        m_devices.removeOne(sw);
        delete sw;
    }
}

void mbServerWindowManager::scriptEditorAdd(mbServerBaseScriptEditor *ui)
{
    QMdiSubWindow* sw = subWindowAdd(ui);
    m_scriptEditors.append(sw);
    connect(ui, &mbServerBaseScriptEditor::nameChanged, sw, &QWidget::setWindowTitle);
    sw->setWindowTitle(ui->name());
    sw->installEventFilter(this);
    Q_EMIT scriptWindowAdded(ui);
}

void mbServerWindowManager::scriptEditorRemove(mbServerBaseScriptEditor *ui)
{
    ui->disconnect(this);
    QMdiSubWindow* sw = subWindowRemove(ui);
    if (sw)
    {
        Q_EMIT scriptWindowRemoving(ui);
        m_scriptEditors.removeOne(sw);
        sw->deleteLater();
    }
}

bool mbServerWindowManager::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
    case QEvent::Close:
    {
        QMdiSubWindow *sw = qobject_cast<QMdiSubWindow*>(obj);
        //Q_ASSERT (sw != nullptr);
        mbServerBaseScriptEditor *ui = qobject_cast<mbServerBaseScriptEditor*>(sw->widget());
        m_scriptManager->removeScriptEditor(ui);
        return true;
    }
    default:
        break;
    }
    return QObject::eventFilter(obj, e);
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
