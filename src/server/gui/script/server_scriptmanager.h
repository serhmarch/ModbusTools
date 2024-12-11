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

class mbCoreProject;
class mbServerProject;
class mbServerDeviceScriptEditor;

class mbServerScriptManager : public QObject
{
    Q_OBJECT

public:
    explicit mbServerScriptManager(QObject *parent = nullptr);

public: // project
    inline mbServerProject *project() const { return m_project; }

public:
    inline int scriptCount() const { return m_scriptEditors.count(); }
    inline QList<mbServerDeviceScriptEditor*> scriptEditors() const { return m_scriptEditors; }
    inline int scriptEditorCount() const { return m_scriptEditors.count(); }
    inline mbServerDeviceScriptEditor *activeScriptEditor() const { return m_activeScriptEditor; }
    mbServerDeviceScriptEditor *deviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType) const;
    void addDeviceScript(mbServerDevice *device, mbServerDevice::ScriptType scriptType);
    void removeDeviceScript(mbServerDevice *device, mbServerDevice::ScriptType scriptType);

Q_SIGNALS:
    void scriptEditorAdd(mbServerDeviceScriptEditor *ui);
    void scriptEditorRemove(mbServerDeviceScriptEditor *ui);
    void scriptEditorActivated(mbServerDeviceScriptEditor*);
    void scriptEditorContextMenu(mbServerDeviceScriptEditor*);

public Q_SLOTS:
    void setActiveScriptEditor(mbServerDeviceScriptEditor *ui);

private Q_SLOTS:
    void setProject(mbCoreProject *project);
    void scriptContextMenu(const QPoint &pos);
    void setProjectModified();

private:
    mbServerProject *m_project;
    typedef QList<mbServerDeviceScriptEditor*> ScriptEditors_t;
    ScriptEditors_t m_scriptEditors;
    //typedef QHash<const mbServerScript*, mbServerDeviceScriptEditor*> HashScriptEditors_t;
    //HashScriptEditors_t m_hashScriptEditors;
    mbServerDeviceScriptEditor *m_activeScriptEditor;

};

#endif // SERVER_SCRIPTMANAGER_H
