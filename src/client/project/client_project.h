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
#ifndef CLIENT_PROJECT_H
#define CLIENT_PROJECT_H

#include <QObject>
#include <QHash>

#include <Modbus.h>
#include <project/core_project.h>

class mbClient;
class mbClientPort;
class mbClientDevice;
class mbClientDataView;

class mbClientProject : public mbCoreProject
{
    Q_OBJECT

public:
    explicit mbClientProject(QObject *parent = nullptr);
    virtual ~mbClientProject();

public: // ports
    using mbCoreProject::portIndex;
    using mbCoreProject::portAdd;
    using mbCoreProject::portRemove;
    inline bool hasPort(mbClientPort* port) const { return mbCoreProject::hasPort(reinterpret_cast<mbCorePort*>(port)); }
    inline QList<mbClientPort*> ports() const { return QList<mbClientPort*>(*(reinterpret_cast<const QList<mbClientPort*>*>(&m_ports))); }
    inline int portIndex(mbClientPort* port) const { return mbCoreProject::portIndex(reinterpret_cast<mbCorePort*>(port)); }
    inline int portIndex(const QString& name) const { return mbCoreProject::portIndex(name); }
    inline mbClientPort* port(int i) const { return reinterpret_cast<mbClientPort*>(mbCoreProject::portCore(i)); }
    inline mbClientPort* port(const QString& name) const { return reinterpret_cast<mbClientPort*>(mbCoreProject::portCore(name)); }
    inline mbClientPort* portAt(int i) const { return reinterpret_cast<mbClientPort*>(mbCoreProject::portCoreAt(i)); }
    inline int portInsert(mbClientPort* port, int index = -1) { return mbCoreProject::portInsert(reinterpret_cast<mbCorePort*>(port), index); }
    inline int portAdd(mbClientPort* port) { return mbCoreProject::portAdd(reinterpret_cast<mbCorePort*>(port)); }
    inline int portRemove(mbClientPort* port) { return mbCoreProject::portRemove(reinterpret_cast<mbCorePort*>(port)); }
    inline bool portRename(mbClientPort* port, const QString& newName)  { return mbCoreProject::portRename(reinterpret_cast<mbCorePort*>(port), newName); }

public: // devices
    using mbCoreProject::deviceIndex;
    using mbCoreProject::deviceAdd;
    using mbCoreProject::deviceRemove;
    inline bool hasDevice(mbClientDevice* device) const { return mbCoreProject::hasDevice(reinterpret_cast<mbCoreDevice*>(device)); }
    inline QList<mbClientDevice*> devices() const { return QList<mbClientDevice*>(*(reinterpret_cast<const QList<mbClientDevice*>*>(&m_devices))); }
    inline int deviceIndex(mbClientDevice* device) const { return mbCoreProject::deviceIndex(reinterpret_cast<mbCoreDevice*>(device)); }
    inline int deviceIndex(const QString& name) const { return mbCoreProject::deviceIndex(name); }
    inline mbClientDevice *device(int i) const { return reinterpret_cast<mbClientDevice*>(mbCoreProject::deviceCore(i)); }
    inline mbClientDevice *device(const QString& name) const { return reinterpret_cast<mbClientDevice*>(mbCoreProject::deviceCore(name)); }
    inline mbClientDevice *deviceAt(int i) const { return reinterpret_cast<mbClientDevice*>(mbCoreProject::deviceCoreAt(i)); }
    inline int deviceInsert(mbClientDevice* device, int index = -1) { return mbCoreProject::deviceInsert(reinterpret_cast<mbCoreDevice*>(device), index); }
    inline int deviceAdd(mbClientDevice* device) { return mbCoreProject::deviceAdd(reinterpret_cast<mbCoreDevice*>(device)); }
    inline int deviceRemove(mbClientDevice* device) { return mbCoreProject::deviceRemove(reinterpret_cast<mbCoreDevice*>(device)); }
    inline bool deviceRename(mbClientDevice* device, const QString& newName)  { return mbCoreProject::deviceRename(reinterpret_cast<mbCoreDevice*>(device), newName); }

public: // dataViews
    using mbCoreProject::dataViewIndex;
    using mbCoreProject::dataViewAdd;
    using mbCoreProject::dataViewRemove;
    inline bool hasDataView(mbClientDataView* dataView) const { return mbCoreProject::hasDataView(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline QList<mbClientDataView*> dataViews() const { return QList<mbClientDataView*>(*(reinterpret_cast<const QList<mbClientDataView*>*>(&m_dataViews))); }
    inline int dataViewIndex(mbClientDataView* dataView) const { return mbCoreProject::dataViewIndex(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline int dataViewIndex(const QString& name) const { return mbCoreProject::dataViewIndex(name); }
    inline mbClientDataView* dataView(int i) const { return reinterpret_cast<mbClientDataView*>(mbCoreProject::dataViewCore(i)); }
    inline mbClientDataView* dataView(const QString& name) const { return reinterpret_cast<mbClientDataView*>(mbCoreProject::dataViewCore(name)); }
    inline mbClientDataView* dataViewAt(int i) const { return reinterpret_cast<mbClientDataView*>(mbCoreProject::dataViewCoreAt(i)); }
    inline int dataViewInsert(mbClientDataView* dataView, int index = -1) { return mbCoreProject::dataViewInsert(reinterpret_cast<mbCoreDataView*>(dataView), index); }
    inline int dataViewAdd(mbClientDataView* dataView) { return mbCoreProject::dataViewAdd(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline int dataViewRemove(mbClientDataView* dataView) { return mbCoreProject::dataViewRemove(reinterpret_cast<mbCoreDataView*>(dataView)); }
    inline bool dataViewRename(mbClientDataView* dataView, const QString& newName)  { return mbCoreProject::dataViewRename(reinterpret_cast<mbCoreDataView*>(dataView), newName); }
};

#endif // CLIENT_PROJECT_H
