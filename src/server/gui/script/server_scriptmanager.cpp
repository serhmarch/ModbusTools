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

#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <server.h>

#include "server_scriptmoduleeditor.h"
#include "server_devicescripteditor.h"
#include <project/server_project.h>

mbServerScriptManager::Strings::Strings() :
    settings_scriptGenerateComment(QStringLiteral("Script.Editor.generateComment")),
    settings_wordWrap             (QStringLiteral("Script.Editor.wordWrap")),
    settings_useLineNumbers       (QStringLiteral("Script.Editor.useLineNumbers")),
    settings_tabSpaces            (QStringLiteral("Script.Editor.tabSpaces")),
    settings_font                 (QStringLiteral("Script.Editor.font")),
    settings_colorFormats         (QStringLiteral("Script.Editor.colorFormats"))
{
}

const mbServerScriptManager::Strings &mbServerScriptManager::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerScriptManager::mbServerScriptManager(QObject *parent) : QObject(parent)
{
    const mbServerScriptEditor::Defaults &d = mbServerScriptEditor::Defaults::instance();
    m_project = nullptr;
    m_activeScriptEditor = nullptr;

    m_settings.generateComment = true;
    m_settings.editorSettings = d.settings;

    mbServer *core = mbServer::global();
    connect(core, &mbServer::projectChanged, this, &mbServerScriptManager::setProject);
    setProject(core->project());

    QFile qrcfile(":/server/python/pytips.py");
    qrcfile.open(QIODevice::ReadOnly  | QIODevice::Text);
    m_generatedComment = QString::fromUtf8(qrcfile.readAll());

    m_settings.editorSettings.colorFormats = mbServerScriptHighlighter::Defaults::instance().colorFormats;
    //m_settings.colorFormats.insert(mbServerScriptHighlighter::TextFormat, QApplication::palette().windowText().color());
}

MBSETTINGS mbServerScriptManager::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r;
    r[s.settings_scriptGenerateComment] = m_settings.generateComment              ;
    r[s.settings_wordWrap             ] = m_settings.editorSettings.wordWrap      ;
    r[s.settings_useLineNumbers       ] = m_settings.editorSettings.useLineNumbers;
    r[s.settings_tabSpaces            ] = m_settings.editorSettings.tabSpaces     ;
    r[s.settings_font                 ] = m_settings.editorSettings.font          ;
    r[s.settings_colorFormats         ] = mbServerScriptHighlighter::toString(m_settings.editorSettings.colorFormats);
    return r;
}

void mbServerScriptManager::setCachedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();

    bool needSetSettings = false;

    it = settings.find(s.settings_scriptGenerateComment);
    if (it != end)
        m_settings.generateComment = it.value().toBool();

    it = settings.find(s.settings_wordWrap);
    if (it != end)
    {
        m_settings.editorSettings.wordWrap = it.value().toBool();
        needSetSettings = true;
    }

    it = settings.find(s.settings_useLineNumbers);
    if (it != end)
    {
        m_settings.editorSettings.useLineNumbers = it.value().toBool();
        needSetSettings = true;
    }

    it = settings.find(s.settings_tabSpaces);
    if (it != end)
    {
        m_settings.editorSettings.tabSpaces = it.value().toInt();
        needSetSettings = true;
    }

    it = settings.find(s.settings_font);
    if (it != end)
    {
        m_settings.editorSettings.font = it.value().toString();
        needSetSettings = true;
    }

    it = settings.find(s.settings_colorFormats);
    if (it != end)
    {
        mbServerScriptHighlighter::ColorFormats formats = mbServerScriptHighlighter::toColorFormats(it.value().toString());
        for (auto it = formats.begin(); it != formats.end(); ++it)
            m_settings.editorSettings.colorFormats[it.key()] = it.value();
        needSetSettings = true;
    }

    if (needSetSettings)
    {
        Q_FOREACH (mbServerBaseScriptEditor* ui, m_scriptEditors)
            ui->setSettings(m_settings.editorSettings);
    }
}

mbServerScriptModuleEditor *mbServerScriptManager::scriptModuleEditor(mbServerScriptModule *sm)
{
    Q_FOREACH(mbServerScriptModuleEditor *sme, m_scriptModuleEditors)
    {
        if (sme->scriptModule() == sm)
            return sme;
    }
    return nullptr;
}

mbServerScriptModuleEditor *mbServerScriptManager::getOrCreateScriptModuleEditor(mbServerScriptModule *sm)
{
    mbServerScriptModuleEditor *se = scriptModuleEditor(sm);
    if (!se)
        se = addScriptModuleEditor(sm);
    return se;
}

mbServerScriptModuleEditor *mbServerScriptManager::addScriptModuleEditor(mbServerScriptModule *sm)
{
    mbServerScriptModuleEditor *ui = new mbServerScriptModuleEditor(sm, m_settings.editorSettings);
    m_scriptEditors.append(ui);
    m_scriptModuleEditors.append(ui);
    QString code = sm->sourceCode();
    ui->setPlainText(code);
    connect(ui, &mbServerDeviceScriptEditor::customContextMenuRequested, this, &mbServerScriptManager::scriptContextMenu);
    connect(ui, &mbServerDeviceScriptEditor::textChanged, this, &mbServerScriptManager::setProjectModified);
    Q_EMIT scriptEditorAdd(ui);
    return ui;
}

mbServerDeviceScriptEditor *mbServerScriptManager::deviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType) const
{
    Q_FOREACH (mbServerDeviceScriptEditor* ui, m_deviceScriptEditors)
    {
        if (ui->device() == device && ui->scriptType() == scriptType)
            return ui;
    }
    return nullptr;
}

mbServerDeviceScriptEditor *mbServerScriptManager::getOrCreateDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType)
{
    mbServerDeviceScriptEditor *se = deviceScriptEditor(device, scriptType);
    if (!se)
        se = addDeviceScriptEditor(device, scriptType);
    return se;
}

mbServerDeviceScriptEditor *mbServerScriptManager::addDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType)
{
    mbServerDeviceScriptEditor *ui = new mbServerDeviceScriptEditor(device, scriptType, m_settings.editorSettings);
    m_scriptEditors.append(ui);
    m_deviceScriptEditors.append(ui);
    QString code = device->script(scriptType);
    if (code.isEmpty() && m_settings.generateComment)
        ui->setPlainText(m_generatedComment);
    else
        ui->setPlainText(code);
    connect(ui, &mbServerDeviceScriptEditor::customContextMenuRequested, this, &mbServerScriptManager::scriptContextMenu);
    connect(ui, &mbServerDeviceScriptEditor::textChanged, this, &mbServerScriptManager::setProjectModified);
    Q_EMIT scriptEditorAdd(ui);
    return ui;
}

void mbServerScriptManager::setProject(mbCoreProject *p)
{
    mbServerProject *project = static_cast<mbServerProject*>(p);
    if (m_project)
    {
        Q_FOREACH (mbServerBaseScriptEditor* ui, m_scriptEditors)
            Q_EMIT scriptEditorRemove(ui);
        m_scriptEditors.clear();
        m_deviceScriptEditors.clear();
        m_scriptModuleEditors.clear();
        m_activeScriptEditor = nullptr;
    }
    m_project = project;
    if (project)
    {
        connect(project, &mbServerProject::deviceRemoving, this, &mbServerScriptManager::removeAllDeviceScripts);
        connect(project, &mbServerProject::scriptModuleRemoving, this, &mbServerScriptManager::scriptModuleRemove);
    }
}

void mbServerScriptManager::scriptModuleRemove(mbServerScriptModule *sm)
{
    mbServerScriptModuleEditor *ui = scriptModuleEditor(sm);
    if (ui)
        removeScriptEditor(ui);
}

void mbServerScriptManager::removeDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType)
{
    // TODO: ASSERT ui != nullptr && m_hashScriptUis.contains(ui->script())
    mbServerDeviceScriptEditor *ui = deviceScriptEditor(device, scriptType);
    if (ui)
        removeScriptEditor(ui);
}

void mbServerScriptManager::removeScriptEditor(mbServerBaseScriptEditor *ui)
{
    if (m_activeScriptEditor == ui)
        m_activeScriptEditor = nullptr;
    ui->disconnect(this);
    Q_EMIT scriptEditorRemove(ui);
    m_scriptEditors.removeOne(ui);
    if (mbServerScriptModuleEditor *sme = qobject_cast<mbServerScriptModuleEditor*>(ui))
    {
        sme->synchSourceCode();
        m_scriptModuleEditors.removeOne(sme);
    }
    else if (mbServerDeviceScriptEditor *dse = qobject_cast<mbServerDeviceScriptEditor*>(ui))
    {
        dse->synchSourceCode();
        m_deviceScriptEditors.removeOne(dse);
    }
    // Must be deleted together with QMdiSubWindow
    //ui->deleteLater(); // Note: need because 'ui' can have 'QMenu'-children located in stack which is trying to delete
}

void mbServerScriptManager::setActiveScriptEditor(mbServerBaseScriptEditor *ui)
{
    // TODO: ASSERT m_hashScriptUis.contains(ui->script())
    if (m_activeScriptEditor != ui)
    {
        m_activeScriptEditor = ui;
        Q_EMIT scriptEditorActivated(ui);
    }
}

void mbServerScriptManager::removeAllDeviceScripts(mbCoreDevice *device)
{
    removeDeviceScriptEditor(static_cast<mbServerDevice*>(device), mbServerDevice::Script_Init );
    removeDeviceScriptEditor(static_cast<mbServerDevice*>(device), mbServerDevice::Script_Loop );
    removeDeviceScriptEditor(static_cast<mbServerDevice*>(device), mbServerDevice::Script_Final);
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
