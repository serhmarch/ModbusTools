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
#include "server_scriptmanager.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include <server.h>

#include "server_devicescripteditor.h"
#include <project/server_project.h>

mbServerScriptManager::mbServerScriptManager(QObject *parent) : QObject(parent)
{
    m_project = nullptr;
    m_activeScriptEditor = nullptr;

    mbServer *core = mbServer::global();
    connect(core, &mbServer::projectChanged, this, &mbServerScriptManager::setProject);
    setProject(core->project());
}

mbServerDeviceScriptEditor *mbServerScriptManager::deviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType) const
{
    Q_FOREACH (mbServerDeviceScriptEditor* ui, m_scriptEditors)
    {
        if (ui->device() == device && ui->scriptType() == scriptType)
            return ui;
    }
    return nullptr;
}

void mbServerScriptManager::setProject(mbCoreProject *p)
{
    mbServerProject *project = static_cast<mbServerProject*>(p);
    if (m_project)
    {
        Q_FOREACH (mbServerDeviceScriptEditor* ui, m_scriptEditors)
            Q_EMIT scriptEditorRemove(ui);
        m_scriptEditors.clear();
        m_activeScriptEditor = nullptr;
    }
    m_project = project;
    if (project)
    {
        //connect(project, &mbServerProject::scriptAdded   , this, &mbServerScriptManager::scriptAdd);
        //connect(project, &mbServerProject::scriptRemoving, this, &mbServerScriptManager::scriptRemove);
        //Q_FOREACH (mbServerScript* d, project->scripts())
        //    scriptAdd(d);
    }
}

void mbServerScriptManager::addDeviceScript(mbServerDevice *device, mbServerDevice::ScriptType scriptType)
{
    mbServerDeviceScriptEditor *ui = new mbServerDeviceScriptEditor(device, scriptType);
    ui->setPlainText(device->script(scriptType));
    m_scriptEditors.append(ui);
    connect(ui, &mbServerDeviceScriptEditor::customContextMenuRequested, this, &mbServerScriptManager::scriptContextMenu);
    connect(ui, &mbServerDeviceScriptEditor::textChanged, this, &mbServerScriptManager::setProjectModified);
    Q_EMIT scriptEditorAdd(ui);
}

void mbServerScriptManager::removeDeviceScript(mbServerDevice *device, mbServerDevice::ScriptType scriptType)
{
    // TODO: ASSERT ui != nullptr && m_hashScriptUis.contains(ui->script())
    mbServerDeviceScriptEditor *ui = deviceScriptEditor(device, scriptType);
    if (m_activeScriptEditor == ui)
        m_activeScriptEditor = nullptr;
    ui->disconnect(this);
    Q_EMIT scriptEditorRemove(ui);
    ui->deleteLater(); // Note: need because 'ui' can have 'QMenu'-children located in stack which is trying to delete
}

void mbServerScriptManager::setActiveScriptEditor(mbServerDeviceScriptEditor *ui)
{
    // TODO: ASSERT m_hashScriptUis.contains(ui->script())
    if (m_activeScriptEditor != ui)
    {
        m_activeScriptEditor = ui;
        Q_EMIT scriptEditorActivated(ui);
    }
}

void mbServerScriptManager::scriptContextMenu(const QPoint & /*pos*/)
{
    //mbServerScriptUi *ui = qobject_cast<mbServerScriptUi*>(sender());
    //Q_EMIT scriptUiContextMenu(ui);
}

void mbServerScriptManager::setProjectModified()
{
    m_project->setModified();
}
