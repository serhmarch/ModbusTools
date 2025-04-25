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
#ifndef SERVER_SCRIPTMANAGER_H
#define SERVER_SCRIPTMANAGER_H

#include <QHash>
#include <QObject>

#include <project/server_device.h>
#include "editor/server_scripteditor.h"

class mbCoreProject;
class mbServerProject;
class mbServerScriptModule;
class mbServerBaseScriptEditor;
class mbServerScriptModuleEditor;
class mbServerDeviceScriptEditor;

class mbServerScriptManager : public QObject
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString settings_scriptGenerateComment;
        const QString settings_wordWrap             ;
        const QString settings_useLineNumbers       ;
        const QString settings_tabSpaces            ;
        const QString settings_font                 ;
        const QString settings_colorFormats         ;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerScriptManager(QObject *parent = nullptr);

public:
    MBSETTINGS cachedSettings() const;
    void setCachedSettings(const MBSETTINGS &settings);

public: // project
    inline mbServerProject *project() const { return m_project; }

public:
    inline int scriptCount() const { return m_scriptEditors.count(); }
    inline QList<mbServerBaseScriptEditor*> scriptEditors() const { return m_scriptEditors; }
    inline QList<mbServerScriptModuleEditor*> scriptModuleEditors() const { return m_scriptModuleEditors; }
    inline QList<mbServerDeviceScriptEditor*> deviceScriptEditors() const { return m_deviceScriptEditors; }
    inline int scriptEditorCount() const { return m_scriptEditors.count(); }
    inline mbServerBaseScriptEditor *activeScriptEditor() const { return m_activeScriptEditor; }
    mbServerScriptModuleEditor *scriptModuleEditor(mbServerScriptModule *sm);
    mbServerScriptModuleEditor *getOrCreateScriptModuleEditor(mbServerScriptModule *sm);
    mbServerScriptModuleEditor *addScriptModuleEditor(mbServerScriptModule *sm);
    mbServerDeviceScriptEditor *deviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType) const;
    mbServerDeviceScriptEditor *getOrCreateDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType);
    mbServerDeviceScriptEditor *addDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType);
    void removeDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType);
    void removeScriptEditor(mbServerBaseScriptEditor *ui);

Q_SIGNALS:
    void scriptEditorAdd(mbServerBaseScriptEditor *ui);
    void scriptEditorRemove(mbServerBaseScriptEditor *ui);
    void scriptEditorActivated(mbServerBaseScriptEditor*);
    void scriptEditorContextMenu(mbServerBaseScriptEditor*);

public Q_SLOTS:
    void setActiveScriptEditor(mbServerBaseScriptEditor *ui);
    void removeAllDeviceScripts(mbCoreDevice *device);

private Q_SLOTS:
    void setProject(mbCoreProject *project);
    void scriptModuleRemove(mbServerScriptModule*);
    void scriptContextMenu(const QPoint &pos);
    void setProjectModified();

private:
    mbServerProject *m_project;

    typedef QList<mbServerBaseScriptEditor*> ScriptEditors_t;
    ScriptEditors_t m_scriptEditors;

    typedef QList<mbServerScriptModuleEditor*> ScriptModuleEditors_t;
    //typedef QHash<mbServerScriptModule*,mbServerScriptModuleEditor*> HashScriptModuleEditors_t;
    ScriptModuleEditors_t m_scriptModuleEditors;
    //HashScriptModuleEditors_t m_hashScriptModuleEditors;

    typedef QList<mbServerDeviceScriptEditor*> DeviceScriptEditors_t;
    DeviceScriptEditors_t m_deviceScriptEditors;

    //typedef QHash<const mbServerScript*, mbServerDeviceScriptEditor*> HashScriptEditors_t;
    //HashScriptEditors_t m_hashScriptEditors;
    mbServerBaseScriptEditor *m_activeScriptEditor;

    struct
    {
        bool generateComment;
        mbServerScriptEditor::Settings editorSettings;
    } m_settings;

    QString m_generatedComment;

};

#endif // SERVER_SCRIPTMANAGER_H
