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
#ifndef SERVER_PROJECT_H
#define SERVER_PROJECT_H

#include <QObject>

#include <project/core_project.h>

class mbServerPort;
class mbServerDevice;
class mbServerDataView;
class mbServerAction;

class mbServerProject : public mbCoreProject
{
    Q_OBJECT

public:
    explicit mbServerProject(QObject *parent = nullptr);
    ~mbServerProject();

public: // ports
    using mbCoreProject::portIndex;
    using mbCoreProject::portAdd;
    using mbCoreProject::portRemove;
    inline bool hasPort(mbServerPort* port) const { return mbCoreProject::hasPort(reinterpret_cast<mbCorePort*>(port)); }
    inline QList<mbServerPort*> ports() const { return QList<mbServerPort*>(*(reinterpret_cast<const QList<mbServerPort*>*>(&m_ports))); }
    inline int portIndex(mbServerPort* port) const { return mbCoreProject::portIndex(reinterpret_cast<mbCorePort*>(port)); }
    inline int portIndex(const QString& name) const { return mbCoreProject::portIndex(name); }
    inline mbServerPort* port(int i) const { return reinterpret_cast<mbServerPort*>(mbCoreProject::portCore(i)); }
    inline mbServerPort* port(const QString& name) const { return reinterpret_cast<mbServerPort*>(mbCoreProject::portCore(name)); }
    inline mbServerPort* portAt(int i) const { return reinterpret_cast<mbServerPort*>(mbCoreProject::portCoreAt(i)); }
    inline int portInsert(mbServerPort* port, int index = -1) { return mbCoreProject::portInsert(reinterpret_cast<mbCorePort*>(port), index); }
    inline int portAdd(mbServerPort* port) { return mbCoreProject::portAdd(reinterpret_cast<mbCorePort*>(port)); }
    inline int portRemove(mbServerPort* port) { return mbCoreProject::portRemove(reinterpret_cast<mbCorePort*>(port)); }
    inline bool portRename(mbServerPort* port, const QString& newName)  { return mbCoreProject::portRename(reinterpret_cast<mbCorePort*>(port), newName); }

public: // devices
    using mbCoreProject::deviceIndex;
    using mbCoreProject::deviceAdd;
    using mbCoreProject::deviceRemove;
    inline bool hasDevice(mbServerDevice* device) const { return mbCoreProject::hasDevice(reinterpret_cast<mbCoreDevice*>(device)); }
    inline QList<mbServerDevice*> devices() const { return QList<mbServerDevice*>(*(reinterpret_cast<const QList<mbServerDevice*>*>(&m_devices))); }
    inline int deviceIndex(mbServerDevice* device) const { return mbCoreProject::deviceIndex(reinterpret_cast<mbCoreDevice*>(device)); }
    inline int deviceIndex(const QString& name) const { return mbCoreProject::deviceIndex(name); }
    inline mbServerDevice *device(int i) const { return reinterpret_cast<mbServerDevice*>(mbCoreProject::deviceCore(i)); }
    inline mbServerDevice *device(const QString& name) const { return reinterpret_cast<mbServerDevice*>(mbCoreProject::deviceCore(name)); }
    inline mbServerDevice *deviceAt(int i) const { return reinterpret_cast<mbServerDevice*>(mbCoreProject::deviceCoreAt(i)); }
    inline int deviceInsert(mbServerDevice* device, int index = -1) { return mbCoreProject::deviceInsert(reinterpret_cast<mbCoreDevice*>(device), index); }
    inline int deviceAdd(mbServerDevice* device) { return mbCoreProject::deviceAdd(reinterpret_cast<mbCoreDevice*>(device)); }
    inline int deviceRemove(mbServerDevice* device) { return mbCoreProject::deviceRemove(reinterpret_cast<mbCoreDevice*>(device)); }
    inline bool deviceRename(mbServerDevice* device, const QString& newName)  { return mbCoreProject::deviceRename(reinterpret_cast<mbCoreDevice*>(device), newName); }

public: // dataViews
    using mbCoreProject::dataViewIndex;
    using mbCoreProject::dataViewAdd;
    using mbCoreProject::dataViewRemove;
    inline bool hasDataView(mbServerDataView* dataView) const { return mbCoreProject::hasDataView(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline QList<mbServerDataView*> dataViews() const { return QList<mbServerDataView*>(*(reinterpret_cast<const QList<mbServerDataView*>*>(&m_dataViews))); }
    inline int dataViewIndex(mbServerDataView* dataView) const { return mbCoreProject::dataViewIndex(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline int dataViewIndex(const QString& name) const { return mbCoreProject::dataViewIndex(name); }
    inline mbServerDataView* dataView(int i) const { return reinterpret_cast<mbServerDataView*>(mbCoreProject::dataViewCore(i)); }
    inline mbServerDataView* dataView(const QString& name) const { return reinterpret_cast<mbServerDataView*>(mbCoreProject::dataViewCore(name)); }
    inline mbServerDataView* dataViewAt(int i) const { return reinterpret_cast<mbServerDataView*>(mbCoreProject::dataViewCoreAt(i)); }
    inline int dataViewInsert(mbServerDataView* dataView, int index = -1) { return mbCoreProject::dataViewInsert(reinterpret_cast<mbCoreDataView*>(dataView), index); }
    inline int dataViewAdd(mbServerDataView* dataView) { return mbCoreProject::dataViewAdd(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline int dataViewRemove(mbServerDataView* dataView) { return mbCoreProject::dataViewRemove(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline bool dataViewRename(mbServerDataView* dataView, const QString& newName)  { return mbCoreProject::dataViewRename(reinterpret_cast<mbCoreDataView*>(dataView), newName); }

public: // actions
    inline bool hasAction(mbServerAction *action) const { return m_actions.contains(action); }
    inline QList<mbServerAction*> actions() const { return m_actions; }
    inline int actionIndex(mbServerAction *action) const { return m_actions.indexOf(action); }
    inline mbServerAction *action(int i) const { return m_actions.value(i); }
    inline mbServerAction *actionAt(int i) const { return m_actions.at(i); }
    inline int actionCount() const { return m_actions.count(); }
    int actionInsert(mbServerAction *action, int index = -1);
    inline int actionAdd(mbServerAction *action) { return actionInsert(action); }
    void actionsInsert(const QList<mbServerAction*> &actions, int index = -1);
    inline void actionsAdd(const QList<mbServerAction*> &actions) { actionsInsert(actions); }
    void actionsRemove(const QList<mbServerAction*> &actions);
    int actionRemove(int index);
    inline int actionRemove(mbServerAction *action) { return actionRemove(actionIndex(action)); }

Q_SIGNALS:
    void actionAdded(mbServerAction *action);
    void actionRemoving(mbServerAction *action);
    void actionRemoved(mbServerAction *action);
    void actionChanged(mbServerAction *action);

private Q_SLOTS:
    void slotActionChanged();

private: // actions
    QList<mbServerAction*> m_actions;
};

#endif // SERVER_PROJECT_H
