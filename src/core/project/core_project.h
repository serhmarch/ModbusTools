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
#ifndef CORE_PROJECT_H
#define CORE_PROJECT_H

#include <QVector>
#include <QObject>

#include <mbcore.h>

class mbCorePort;
class mbCoreDevice;
class mbCoreDataView;
class mbCoreTaskInfo;

class MB_EXPORT mbCoreProject : public QObject
{
    Q_OBJECT
public:
    struct MB_EXPORT Strings
    {
        const QString name;
        const QString author;
        const QString comment;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreProject(QObject *parent = nullptr);
    virtual ~mbCoreProject();

public: // project
    inline QString name() const { return objectName(); }
    inline void setName(const QString& name) { setObjectName(name); Q_EMIT paramsChanged(); }
    inline QString author() const { return m_author; }
    inline void setAuthor(const QString& author) { m_author = author; Q_EMIT paramsChanged(); }
    inline QString comment() const { return m_comment; }
    inline void setComment(const QString& comment) { m_comment = comment; Q_EMIT paramsChanged(); }
    inline QString file() const { return m_file; }
    inline QString absoluteDirPath() const { return m_absoluteDirPath; }
    inline QString absoluteFilePath() const { return m_absoluteFilePath; }
    void setAbsoluteFilePath(const QString& file);

public: // settings
    virtual MBSETTINGS settings() const;
    virtual bool setSettings(const MBSETTINGS &settings);

public: // ports
    QString freePortName(const QString& s = QString()) const;
    inline bool hasPort(const QString& name) const { return m_hashPorts.contains(name); }
    inline bool hasPort(mbCorePort* port) const { return m_ports.contains(port); }
    inline QList<mbCorePort*> portsCore() const { return m_ports; }
    inline int portIndex(mbCorePort* port) const { return m_ports.indexOf(port); }
    inline int portIndex(const QString& name) const { return portIndex(portCore(name)); }
    inline mbCorePort* portCore(int i) const { return m_ports.value(i); }
    inline mbCorePort* portCore(const QString& name) const { return m_hashPorts.value(name, nullptr); }
    inline mbCorePort* portCoreAt(int i) const { return m_ports.at(i); }
    inline int portCount() const { return m_ports.count(); }
    int portInsert(mbCorePort* port, int index = -1);
    inline int portAdd(mbCorePort* port) { return portInsert(port); }
    int portRemove(int index);
    inline int portRemove(const QString& name) { return portRemove(portIndex(name)); }
    inline int portRemove(mbCorePort* port) { return portRemove(portIndex(port)); }
    bool portRename(mbCorePort* port, const QString& newName);

public: // devices
    QString freeDeviceName(const QString& s = QString()) const;
    inline bool hasDevice(const QString& name) const { return m_hashDevices.contains(name); }
    inline bool hasDevice(mbCoreDevice* device) const { return m_devices.contains(device); }
    inline QList<mbCoreDevice*> devicesCore() const { return m_devices; }
    inline int deviceIndex(mbCoreDevice* device) const { return m_devices.indexOf(device); }
    inline int deviceIndex(const QString& name) const { return deviceIndex(deviceCore(name)); }
    inline mbCoreDevice* deviceCore(int i) const { return m_devices.value(i); }
    inline mbCoreDevice* deviceCore(const QString& name) const { return m_hashDevices.value(name, nullptr); }
    inline mbCoreDevice* deviceCoreAt(int i) const { return m_devices.at(i); }
    inline int deviceCount() const { return m_devices.count(); }
    int deviceInsert(mbCoreDevice* device, int index = -1);
    inline int deviceAdd(mbCoreDevice* device) { return deviceInsert(device); }
    int deviceRemove(int index);
    inline int deviceRemove(const QString& name) { return deviceRemove(deviceIndex(name)); }
    inline int deviceRemove(mbCoreDevice* device) { return deviceRemove(deviceIndex(device)); }
    bool deviceRename(mbCoreDevice* device, const QString& newName);

public: // watch lists
    QString freeDataViewName(const QString &s = QString()) const;
    inline bool hasDataView(const QString &name) const { return m_hashDataViews.contains(name); }
    inline bool hasDataView(mbCoreDataView *dataView) const { return m_dataViews.contains(dataView); }
    inline QList<mbCoreDataView*> dataViewsCore() const { return m_dataViews; }
    inline int dataViewIndex(mbCoreDataView *dataView) const { return m_dataViews.indexOf(dataView); }
    inline int dataViewIndex(const QString &name) const { return dataViewIndex(dataViewCore(name)); }
    inline mbCoreDataView *dataViewCore(int i) const { return m_dataViews.value(i); }
    inline mbCoreDataView *dataViewCore(const QString &name) const { return m_hashDataViews.value(name, nullptr); }
    inline mbCoreDataView *dataViewCoreAt(int i) const { return m_dataViews.at(i); }
    inline int dataViewCount() const { return m_dataViews.count(); }
    int dataViewInsert(mbCoreDataView *dataView, int index = -1);
    inline int dataViewAdd(mbCoreDataView *dataView) { return dataViewInsert(dataView); }
    int dataViewRemove(int index);
    inline int dataViewRemove(const QString &name) { return dataViewRemove(dataViewIndex(name)); }
    inline int dataViewRemove(mbCoreDataView *dataView) { return dataViewRemove(dataViewIndex(dataView)); }
    bool dataViewRename(mbCoreDataView *dataView, const QString &newName);

public: // tasks
    inline bool hasTask(const QString& name) const { return m_hashTasks.contains(name); }
    inline bool hasTask(mbCoreTaskInfo* task) const { return m_tasks.contains(task); }
    inline QList<mbCoreTaskInfo*> tasks() const { return m_tasks.toList(); }
    inline int taskIndex(mbCoreTaskInfo* task) const { return m_tasks.indexOf(task); }
    inline int taskIndex(const QString& name) const { return taskIndex(task(name)); }
    inline mbCoreTaskInfo* task(int i) const { return m_tasks.value(i, nullptr); }
    inline mbCoreTaskInfo* task(const QString& name) const { return m_hashTasks.value(name, nullptr); }
    inline mbCoreTaskInfo* taskAt(int i) const { return m_tasks.at(i); }
    inline int taskCount() const { return m_tasks.count(); }
    virtual int taskInsert(mbCoreTaskInfo *task, int index = -1);
    inline int taskAdd(mbCoreTaskInfo *task) { return taskInsert(task); }
    virtual int taskRemove(int i);
    inline int taskRemove(const QString& name) { return taskRemove(taskIndex(name)); }
    inline int taskRemove(mbCoreTaskInfo *task) { return taskRemove(taskIndex(task)); }
    bool taskRename(mbCoreTaskInfo *task, const QString& newName);
    QString freeTaskName(const QString& name);

Q_SIGNALS:
    void paramsChanged();

Q_SIGNALS:
    void portAdded(mbCorePort*);
    void portRemoving(mbCorePort*);
    void portRemoved(mbCorePort*);
    void portRenaming(mbCorePort*,const QString&);

Q_SIGNALS:
    void deviceAdded(mbCoreDevice*);
    void deviceRemoving(mbCoreDevice*);
    void deviceRemoved(mbCoreDevice*);
    void deviceRenaming(mbCoreDevice*,const QString&);

Q_SIGNALS:
    void dataViewAdded(mbCoreDataView*);
    void dataViewRemoving(mbCoreDataView*);
    void dataViewRemoved(mbCoreDataView*);
    void dataViewRenaming(mbCoreDataView*,const QString&);

Q_SIGNALS:
    void taskAdded(mbCoreTaskInfo*);
    void taskRemoving(mbCoreTaskInfo*);
    void taskRemoved(mbCoreTaskInfo*);
    void taskRenaming(mbCoreTaskInfo*, const QString&);

protected:
    QString m_file;
    QString m_absoluteDirPath;
    QString m_absoluteFilePath;
    QString m_author;
    QString m_comment;

    struct Task
    {
        QString name;
        QString type;
        MBSETTINGS params;
    };

protected:
    typedef QList<mbCorePort*> Ports_t;
    typedef QHash<QString, mbCorePort*> HashPorts_t;
    Ports_t m_ports;
    HashPorts_t m_hashPorts;

protected: // devices
    typedef QList<mbCoreDevice*> Devices_t;
    typedef QHash<QString, mbCoreDevice*> HashDevices_t;
    Devices_t m_devices;
    HashDevices_t m_hashDevices;

protected: // dataviews
    typedef QList<mbCoreDataView*> DataViews_t;
    typedef QHash<QString, mbCoreDataView*> HashDataViews_t;
    DataViews_t m_dataViews;
    HashDataViews_t m_hashDataViews;

protected:
    typedef QVector<mbCoreTaskInfo*> Tasks_t;
    typedef QHash<QString, mbCoreTaskInfo*> HashTasks_t;

    Tasks_t m_tasks;
    HashTasks_t m_hashTasks;
};

#endif // CORE_PROJECT_H
