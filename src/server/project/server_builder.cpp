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
#include "server_builder.h"

#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include <server.h>

#include "server_dom.h"
#include "server_project.h"
#include "server_port.h"
#include "server_device.h"
#include "server_deviceref.h"
#include "server_simaction.h"
#include "server_scriptmodule.h"
#include "server_dataview.h"

mbServerBuilder::Strings::Strings() :
    sep(QChar(';')),
    scriptModuleName   (QStringLiteral("# mbtools-name:")),
    scriptModuleAuthor (QStringLiteral("# mbtools-author:")),
    scriptModuleComment(QStringLiteral("# mbtools-comment:"))
{
}

const mbServerBuilder::Strings &mbServerBuilder::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerBuilder::mbServerBuilder(QObject *parent) :
    mbCoreBuilder (parent)
{
}

QStringList mbServerBuilder::csvSimActionAttributes() const
{
    const mbServerSimAction::Strings &s = mbServerSimAction::Strings::instance();
    return QStringList() << s.device
                         << s.address
                         << s.dataType
                         << s.period
                         << s.comment
                         << s.actionType
                         << s.byteOrder
                         << s.registerOrder
                         << s.extended
        ;
}

mbCoreProject *mbServerBuilder::newProject() const
{
    return new mbServerProject;
}

mbCorePort *mbServerBuilder::newPort() const
{
    return new mbServerPort;
}

mbCoreDevice *mbServerBuilder::newDevice() const
{
    return new mbServerDevice;
}

mbCoreDataView *mbServerBuilder::newDataView() const
{
    return new mbServerDataView;
}

mbCoreDataViewItem *mbServerBuilder::newDataViewItem() const
{
    return new mbServerDataViewItem;
}

mbCoreDomProject *mbServerBuilder::newDomProject() const
{
    return new mbServerDomProject;
}

mbCoreDomPort *mbServerBuilder::newDomPort() const
{
    return new mbServerDomPort;
}

mbCoreDomDevice *mbServerBuilder::newDomDevice() const
{
    return new mbServerDomDevice;
}

mbCoreDomDataView *mbServerBuilder::newDomDataView() const
{
    return new mbServerDomDataView;
}

mbCoreDomDataViewItem *mbServerBuilder::newDomDataViewItem() const
{
    return new mbServerDomDataViewItem;
}

mbServerSimAction *mbServerBuilder::newSimAction() const
{
    return new mbServerSimAction;
}

mbServerSimAction *mbServerBuilder::newSimAction(mbServerSimAction *prev) const
{
    mbServerSimAction* item = newSimAction();
    item->setSettings(prev->settings());
    mb::Address adr = prev->address();
    adr += prev->count();
    item->setAddress(adr);
    return item;
}

mbServerDomSimAction *mbServerBuilder::newDomSimAction() const
{
    return new mbServerDomSimAction;
}

mbServerScriptModule *mbServerBuilder::newScriptModule() const
{
    return new mbServerScriptModule;
}

mbServerDomScriptModule *mbServerBuilder::newDomScriptModule() const
{
    return new mbServerDomScriptModule;
}

void mbServerBuilder::fillProject(mbCoreProject *obj, const mbCoreDomProject *dom)
{
    mbCoreBuilder::fillProject(obj, dom);
    mbServerProject *project = static_cast<mbServerProject*>(obj);
    setWorkingProjectCore(project);
    project->simActionsAdd(toSimActions(static_cast<const mbServerDomProject*>(dom)->simActions()));
    Q_FOREACH(mbServerDomScriptModule *d, static_cast<const mbServerDomProject*>(dom)->scriptModules())
        project->scriptModuleAdd(toScriptModule(d));
    setWorkingProjectCore(nullptr);
}

void mbServerBuilder::fillPort(mbCorePort *obj, const mbCoreDomPort *dom)
{
    mbCoreBuilder::fillPort(obj, dom);
    mbServerPort *port = static_cast<mbServerPort*>(obj);
    Q_FOREACH (mbServerDomDeviceRef *domDeviceRef, static_cast<const mbServerDomPort*>(dom)->devices())
    {
        mbServerDevice *dev = project()->device(domDeviceRef->name());
        if (dev)
        {
            mbServerDeviceRef *ref = new mbServerDeviceRef(dev);
            ref->setUnitsStr(domDeviceRef->units());
            port->deviceAdd(ref);
        }
    }
}

void mbServerBuilder::fillDevice(mbCoreDevice *obj, const mbCoreDomDevice *dom)
{
    mbCoreBuilder::fillDevice(obj, dom);
    mbServerDevice *device = static_cast<mbServerDevice*>(obj);
    const mbServerDomDeviceData *data = &static_cast<const mbServerDomDevice*>(dom)->data0x();
    if (device->isSaveData())
        device->write_0x_bool(data->offset(), data->count(), reinterpret_cast<const bool*>(toBoolData(data->data()).constData()));

    data = &static_cast<const mbServerDomDevice*>(dom)->data1x();
    if (device->isSaveData())
        device->write_1x_bool(data->offset(), data->count(), reinterpret_cast<const bool*>(toBoolData(data->data()).constData()));

    data = &static_cast<const mbServerDomDevice*>(dom)->data3x();
    if (device->isSaveData())
        device->write_3x(data->offset(), data->count(), reinterpret_cast<const quint16*>(toUInt16Data(data->data()).constData()));

    data = &static_cast<const mbServerDomDevice*>(dom)->data4x();
    if (device->isSaveData())
        device->write_4x(data->offset(), data->count(), reinterpret_cast<const quint16*>(toUInt16Data(data->data()).constData()));
}

void mbServerBuilder::fillDomProject(mbCoreDomProject *dom, const mbCoreProject *obj)
{
    mbCoreBuilder::fillDomProject(dom, obj);
    mbServerDomProject *domProject = static_cast<mbServerDomProject*>(dom);
    setWorkingProjectCore(const_cast<mbCoreProject*>(obj));
    domProject->setSimActions(toDomSimActions(static_cast<const mbServerProject*>(obj)->simActions()));
    domProject->setScriptModules(toDomScriptModules(static_cast<const mbServerProject*>(obj)->scriptModules()));
    setWorkingProjectCore(nullptr);
}

void mbServerBuilder::fillDomPort(mbCoreDomPort *dom, const mbCorePort *obj)
{
    mbCoreBuilder::fillDomPort(dom, obj);
    mbServerDomPort *domPort = static_cast<mbServerDomPort*>(dom);
    QList<mbServerDomDeviceRef*> domDeviceRefs;
    Q_FOREACH (mbServerDeviceRef *ref, static_cast<const mbServerPort*>(obj)->devices())
    {
        mbServerDomDeviceRef *dRef = new mbServerDomDeviceRef;
        dRef->setName(ref->name());
        dRef->setUnits(ref->unitsStr());
        domDeviceRefs.append(dRef);
    }
    domPort->setDevices(domDeviceRefs);
}

void mbServerBuilder::fillDomDevice(mbCoreDomDevice *dom, const mbCoreDevice *obj)
{
    mbCoreBuilder::fillDomDevice(dom, obj);
    mbServerDomDevice* domDevice = static_cast<mbServerDomDevice*>(dom);

    mbServerDomDeviceData* data0x = &domDevice->data0x();
    data0x->setCount(static_cast<const mbServerDevice*>(obj)->count_0x());
    // 1x
    mbServerDomDeviceData* data1x = &domDevice->data1x();
    data1x->setCount(static_cast<const mbServerDevice*>(obj)->count_1x());
    // 3x
    mbServerDomDeviceData* data3x = &domDevice->data3x();
    data3x->setCount(static_cast<const mbServerDevice*>(obj)->count_3x());
    // 4x
    mbServerDomDeviceData* data4x = &domDevice->data4x();
    data4x->setCount(static_cast<const mbServerDevice*>(obj)->count_4x());
    if (static_cast<const mbServerDevice*>(obj)->isSaveData())
    {
        // 0x
        data0x->setOffset(0);
        QByteArray v0x(static_cast<const mbServerDevice*>(obj)->count_0x(), '\0');
        static_cast<const mbServerDevice*>(obj)->read_0x_bool(0, static_cast<const mbServerDevice*>(obj)->count_0x(), reinterpret_cast<bool*>(v0x.data()));
        data0x->setData(fromBoolData(v0x));
        // 1x
        data1x->setOffset(0);
        QByteArray v1x(static_cast<const mbServerDevice*>(obj)->count_1x(), '\0');
        static_cast<const mbServerDevice*>(obj)->read_1x_bool(0, static_cast<const mbServerDevice*>(obj)->count_1x(), reinterpret_cast<bool*>(v1x.data()));
        data1x->setData(fromBoolData(v1x));
        // 3x
        data3x->setOffset(0);
        QByteArray v3x(static_cast<const mbServerDevice*>(obj)->count_3x()*MB_REGE_SZ_BYTES, '\0');
        static_cast<const mbServerDevice*>(obj)->read_3x(0, static_cast<const mbServerDevice*>(obj)->count_3x(), reinterpret_cast<quint16*>(v3x.data()));
        data3x->setData(fromUInt16Data(v3x));
        // 4x
        data4x->setOffset(0);
        QByteArray v4x(static_cast<const mbServerDevice*>(obj)->count_4x()*MB_REGE_SZ_BYTES, '\0');
        static_cast<const mbServerDevice*>(obj)->read_4x(0, static_cast<const mbServerDevice*>(obj)->count_4x(), reinterpret_cast<quint16*>(v4x.data()));
        data4x->setData(fromUInt16Data(v4x));
    }
}

mbServerSimAction *mbServerBuilder::toSimAction(mbServerDomSimAction *dom)
{
    mbServerSimAction* action = new mbServerSimAction;
    mbServerProject *project = this->project();
    if (project)
    {
        mbServerDevice *device = project->device(dom->device());
        if (device)
            action->setDevice(device);
    }
    action->setAddressStr(dom->address());
    action->setDataTypeStr(dom->dataType());
    action->setPeriod(dom->period());
    action->setComment(dom->comment());
    action->setActionTypeStr(dom->actionType());
    action->setByteOrderStr(dom->byteOrder());
    action->setRegisterOrderStr(dom->registerOrder());
    action->setExtendedSettingsStr(dom->extended());
    return action;
}

mbServerSimAction *mbServerBuilder::toSimAction(const MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &s = mbCoreDataViewItem::Strings::instance();
    MBSETTINGS sets = settings;
    mbServerSimAction *item = newSimAction();
    if (mbServerProject *project = this->project())
    {
        MBSETTINGS::Iterator it = sets.find(s.device);
        if (it != sets.end())
        {
            QString devName = it.value().toString();
            mbServerDevice *dev = project->device(devName);
            if (dev)
                item->setDevice(dev);
            sets.erase(it);
        }
    }

    item->setSettings(sets);
    return item;
}

QList<mbServerSimAction *> mbServerBuilder::toSimActions(const QList<mbServerDomSimAction *> &dom)
{
    QList<mbServerSimAction *> ls;
    Q_FOREACH(mbServerDomSimAction *domSimAction, dom)
    {
        ls.append(toSimAction(domSimAction));
    }
    return ls;
}

mbServerDomSimAction *mbServerBuilder::toDomSimAction(mbServerSimAction *action)
{
    mbServerDomSimAction* dom = new mbServerDomSimAction;
    if (action->device())
        dom->setDevice(action->device()->name());
    dom->setAddress(action->addressStr());
    dom->setDataType(action->dataTypeStr());
    dom->setPeriod(action->period());
    dom->setComment(action->comment());
    dom->setActionType(action->actionTypeStr());
    dom->setByteOrder(action->byteOrderStr());
    dom->setRegisterOrder(action->registerOrderStr());
    dom->setExtended(action->extendedSettingsStr());
    return dom;
}

MBSETTINGS mbServerBuilder::toSettings(const mbServerSimAction *item)
{
    const mbServerSimAction::Strings &s = mbServerSimAction::Strings::instance();
    MBSETTINGS sets = item->settings();
    mbCoreDevice *dev = item->device();
    if (dev)
        sets[s.device] = dev->name();
    return sets;
}

QList<mbServerDomSimAction *> mbServerBuilder::toDomSimActions(const QList<mbServerSimAction *> &actions)
{
    QList<mbServerDomSimAction*> domSimActions;
    Q_FOREACH(mbServerSimAction *action, actions)
    {
        domSimActions.append(toDomSimAction(action));
    }
    return domSimActions;
}

mbServerScriptModule *mbServerBuilder::toScriptModule(mbServerDomScriptModule *dom) const
{
    mbServerScriptModule *obj = newScriptModule();
    fillScriptModule(obj, dom);
    return obj;
}

void mbServerBuilder::fillScriptModule(mbServerScriptModule *obj, const mbServerDomScriptModule *dom) const
{
    obj->setSettings(dom->settings());
}

mbServerDomScriptModule *mbServerBuilder::toDomScriptModule(mbServerScriptModule *cfg) const
{
    mbServerDomScriptModule *dom = newDomScriptModule();
    dom->setSettings(cfg->settings());
    return dom;
}

void mbServerBuilder::fillDomScriptModule(mbServerDomScriptModule *dom, const mbServerScriptModule *obj) const
{
    dom->setSettings(obj->settings());
}

QList<mbServerDomScriptModule*> mbServerBuilder::toDomScriptModules(const QList<mbServerScriptModule *> &cfg) const
{
    QList<mbServerDomScriptModule*> dom;
    Q_FOREACH(mbServerScriptModule *sm, cfg)
    {
        dom.append(toDomScriptModule(sm));
    }
    return dom;
}

QList<mbServerSimAction*> mbServerBuilder::importSimActions(const QString &file)
{
    if (file.endsWith(Strings::instance().csv))
        return importSimActionsCsv(file);
    return importSimActionsXml(file);
}

QList<mbServerSimAction*> mbServerBuilder::importSimActionsXml(const QString &file)
{
    mbServerDomSimActions domSimActions;
    if (loadXml(file, &domSimActions))
        return toSimActions(domSimActions.items());
    return QList<mbServerSimAction*>();
}

QList<mbServerSimAction*> mbServerBuilder::importSimActionsCsv(const QString &file)
{
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly))
    {
        setError(qf.errorString());
        return QList<mbServerSimAction *>();
    }
    QList<mbServerSimAction *> items = importSimActionsCsv(&qf);
    qf.close();
    return items;
}

QList<mbServerSimAction *> mbServerBuilder::importSimActionsXml(QIODevice *io)
{
    mbServerDomSimActions domSimActions;
    if (loadXml(io, &domSimActions))
        return toSimActions(domSimActions.items());
    return QList<mbServerSimAction*>();
}

QList<mbServerSimAction *> mbServerBuilder::importSimActionsCsv(QIODevice *io)
{
    QList<mbServerSimAction *> items;
    QString header = QString::fromUtf8(io->readLine());
    QStringList attrNames = parseCsvRow(header);
    if (attrNames.isEmpty())
        return items;
    while (!io->atEnd())
    {
        QByteArray line = io->readLine();
        QString row = QString::fromUtf8(line);
        MBSETTINGS settings = parseCsvRowItem(attrNames, row);
        if (settings.isEmpty())
            continue;
        mbServerSimAction *item = toSimAction(settings);
        items.append(item);
    }
    return items;
}

bool mbServerBuilder::exportSimActions(const QString &file, const QList<mbServerSimAction *> &actions)
{
    if (file.endsWith(Strings::instance().csv))
        return exportSimActionsCsv(file, actions);
    return exportSimActionsXml(file, actions);
}

bool mbServerBuilder::exportSimActionsXml(const QString &file, const QList<mbServerSimAction *> &actions)
{
    mbServerDomSimActions domSimActions;
    domSimActions.setItems(toDomSimActions(actions));
    return saveXml(file, &domSimActions);
}

bool mbServerBuilder::exportSimActionsCsv(const QString &file, const QList<mbServerSimAction *> &cfg)
{
    QFile qf(file);
    if (!qf.open(QIODevice::WriteOnly))
    {
        setError(qf.errorString());
        return false;
    }
    bool res = exportSimActionsCsv(&qf, cfg);
    qf.close();
    return res;
}

bool mbServerBuilder::exportSimActionsXml(QIODevice *io, const QList<mbServerSimAction *> &actions)
{
    mbServerDomSimActions domSimActions;
    domSimActions.setItems(toDomSimActions(actions));
    return saveXml(io, &domSimActions);
}

bool mbServerBuilder::exportSimActionsCsv(QIODevice *io, const QList<mbServerSimAction *> &cfg)
{
    QStringList lsHeader = csvSimActionAttributes();
    QString sHeader = makeCsvRow(lsHeader);
    io->write(sHeader.toUtf8());
    Q_FOREACH (const mbServerSimAction *item, cfg)
    {
        MBSETTINGS settings = toSettings(item);
        QString sLine = makeCsvRowItem(lsHeader, settings);
        io->write(sLine.toUtf8());
    }
    return true;;
}

mbServerScriptModule *mbServerBuilder::importScriptModule(const QString &file)
{
    return importScriptModuleTxt(file);
}

mbServerScriptModule *mbServerBuilder::importScriptModuleTxt(const QString &file)
{
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly))
    {
        setError(qf.errorString());
        return nullptr;
    }
    mbServerScriptModule* obj = importScriptModuleTxt(&qf);
    qf.close();
    return obj;
}

mbServerScriptModule *mbServerBuilder::importScriptModuleTxt(QIODevice *io)
{
    const Strings& s = Strings::instance();
    mbServerScriptModule *obj = new mbServerScriptModule;
    QTextStream stream(io);
    QString line;
    QString code;
    while (!stream.atEnd() && (line = stream.readLine()).startsWith('#'))
    {
        if (line.startsWith(s.scriptModuleName))
            obj->setName(line.mid(s.scriptModuleName.length()).trimmed());
        else if (line.startsWith(s.scriptModuleAuthor))
            obj->setAuthor(line.mid(s.scriptModuleAuthor.length()).trimmed());
        else if (line.startsWith(s.scriptModuleComment))
            obj->setComment(line.mid(s.scriptModuleComment.length()).trimmed());
        else
            code += line + QChar('\n');
    }

    while (!stream.atEnd())
    {
        code += line + QChar('\n');
        line = stream.readLine();
    }
    obj->setSourceCode(code);
    return obj;
}

bool mbServerBuilder::exportScriptModule(const QString &file, const mbServerScriptModule *obj)
{
    return exportScriptModuleTxt(file, obj);
}

bool mbServerBuilder::exportScriptModuleTxt(const QString &file, const mbServerScriptModule *obj)
{
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly))
    {
        setError(qf.errorString());
        return false;
    }
    bool res = exportScriptModuleTxt(&qf, obj);
    qf.close();
    return res;
}

bool mbServerBuilder::exportScriptModuleTxt(QIODevice *io, const mbServerScriptModule *obj)
{
    const Strings& s = Strings::instance();
    QTextStream stream(io);
    QString code = obj->sourceCode();
    QTextStream scode(&code);
    QString line;
    bool bName = true, bAuthor = true, bComment = true;
    while (!scode.atEnd() && (line = stream.readLine()).startsWith('#'))
    {
        if (line.startsWith(s.scriptModuleName) && bName)
        {
            stream << s.scriptModuleName << obj->name() << QChar('\n');
            bName = false;
        }
        if (line.startsWith(s.scriptModuleAuthor) && bAuthor)
        {
            stream << s.scriptModuleAuthor << obj->author() << QChar('\n');
            bAuthor = false;
        }
        else if (line.startsWith(s.scriptModuleComment) && bComment)
        {
            stream << s.scriptModuleComment << obj->comment() << QChar('\n');
            bComment = false;
        }
        else
            stream << line << QChar('\n');
    }
    if (bName)
        stream << s.scriptModuleName << obj->name() << QChar('\n');
    if (bAuthor)
        stream << s.scriptModuleAuthor << obj->author() << QChar('\n');
    if (bComment)
        stream << s.scriptModuleComment << obj->comment() << QChar('\n');
    while (!scode.atEnd())
        stream << scode.readLine() << QChar('\n');
    return true;
}

bool mbServerBuilder::importBoolData(const QString &file, QByteArray &data, const QChar &sep)
{
    QFile f(file);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        return importBoolData(&f, data, sep);
    setError(f.errorString());
    return false;
}

bool mbServerBuilder::importBoolData(QIODevice *buff, QByteArray &data, const QChar &sep)
{
    QList<bool> ls;
    while (!buff->atEnd())
    {
        QString line = QString::fromUtf8(buff->readLine());
        int b = 0;
        for (int i = 0; b < line.size();)
        {
            for (;i < line.size() && line.at(i) != sep; i++) {}
            QString v = line.mid(b, i-b).trimmed();
            ls.append(static_cast<bool>(v.toInt()));
            i++;
            b = i;
        }
    }
    data.resize(ls.size());
    int i = 0;
    Q_FOREACH (bool v, ls)
    {
        data[i] = v;
        i++;
    }
    return true;
}

bool mbServerBuilder::exportBoolData(const QString &file, const QByteArray &data, int columns, const QChar &sep)
{
    QFileInfo info(file);
    QDir dir = info.absoluteDir();
    if (!dir.mkpath(dir.absolutePath()))
    {
        setError(QString("Can't create directory '%1'").arg(dir.absolutePath()));
        return false;
    }
    // Saving project file
    QFile f(info.absoluteFilePath());
    if (f.open(QIODevice::WriteOnly))
        return exportBoolData(&f, data, columns, sep);
    setError(f.errorString());
    return false;
}

bool mbServerBuilder::exportBoolData(QIODevice *buff, const QByteArray &data, int columns, const QChar &sep)
{
    int c = data.size();
    const bool* v = reinterpret_cast<const bool*>(data.constData());
    for (int i = 0; i < c; i++)
    {
        QByteArray line;
        line.reserve(columns*2-1);
        for (int j = 0; (i < (c-1)) && (j < (columns-1)); i++, j++)
        {
            line.append('0'+v[i]);
            line.append(sep.toLatin1());
        }
        line.append('0'+v[i]);
        line.append('\n');
        buff->write(line);
    }
    return true;
}

bool mbServerBuilder::importUInt16Data(const QString &file, QByteArray &data, const QChar &sep)
{
    QFile f(file);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        return importUInt16Data(&f, data, sep);
    setError(f.errorString());
    return false;
}

bool mbServerBuilder::importUInt16Data(QIODevice *buff, QByteArray &data, const QChar &sep)
{
    QList<quint16> ls;
    while (!buff->atEnd())
    {
        QString line = QString::fromUtf8(buff->readLine());
        int b = 0;
        for (int i = 0; b < line.size();)
        {
            for (;i < line.size() && line.at(i) != sep; i++) {}
            QString v = line.mid(b, i-b).trimmed();
            ls.append(v.toUShort());
            i++;
            b = i;
        }
    }
    data.resize(ls.size()*static_cast<int>(sizeof(quint16)));
    quint16 *d = reinterpret_cast<quint16*>(data.data());
    int i = 0;
    Q_FOREACH (quint16 v, ls)
    {
        d[i] = v;
        i++;
    }
    return true;
}

bool mbServerBuilder::exportUInt16Data(const QString &file, const QByteArray &data, int columns, const QChar &sep)
{
    QFileInfo info(file);
    QDir dir = info.absoluteDir();
    if (!dir.mkpath(dir.absolutePath()))
    {
        setError(QString("Can't create directory '%1'").arg(dir.absolutePath()));
        return false;
    }
    // Saving project file
    QFile f(info.absoluteFilePath());
    if (f.open(QIODevice::WriteOnly))
        return exportUInt16Data(&f, data, columns, sep);
    setError(f.errorString());
    return false;
}

bool mbServerBuilder::exportUInt16Data(QIODevice *buff, const QByteArray &data, int columns, const QChar &sep)
{
    int c = data.size() / static_cast<int>(sizeof(quint16));
    const quint16* v = reinterpret_cast<const quint16*>(data.constData());
    for (int i = 0; i < c; i++)
    {
        QByteArray line;
        line.reserve(columns*2-1);
        for (int j = 0; (i < (c-1)) && (j < (columns-1)); i++, j++)
        {
            line.append(QByteArray::number(v[i]));
            line.append(sep.toLatin1());
        }
        line.append(QByteArray::number(v[i]));
        line.append('\n');
        buff->write(line);
    }
    return true;
}

void mbServerBuilder::importDomProject(mbCoreDomProject *dom)
{
}

mbServerBuilder::BoolData_t mbServerBuilder::toBoolData(const QString &str, int reserve)
{
    Strings s = Strings::instance();

    QByteArray r;
    r.reserve(reserve);
    int i = 0, b = i;
    for (; i < str.size(); i++)
    {
        if (str.at(i) == s.sep)
        {
            r.append(str.midRef(b, i-b).toInt() != 0);
            b = i+1;
        }
    }
    r.append(str.midRef(b).toInt() != 0);
    return r;
}

mbServerBuilder::UInt16Data_t mbServerBuilder::toUInt16Data(const QString &str, int reserve)
{
    Strings s = Strings::instance();

    QByteArray r;
    r.reserve(reserve*MB_REGE_SZ_BYTES);
    int i = 0, b = i;
    for (; i < str.size(); i++)
    {
        if (str.at(i) == s.sep)
        {
            quint16 v = str.midRef(b, i-b).toUShort();
            r.append(reinterpret_cast<char*>(&v), sizeof(v));
            b = i+1;
        }
    }
    quint16 v = str.midRef(b).toUShort();
    r.append(reinterpret_cast<char*>(&v), sizeof(v));
    return r;
}

QString mbServerBuilder::fromBoolData(const mbServerBuilder::BoolData_t &data)
{
    QString r;
    if (data.size())
    {
        Strings s = Strings::instance();

        r.reserve(data.size()*2-1);
        r.append(QString::number(data.at(0) != 0));
        for (int i = 1; i < data.size(); i++)
        {
            r.append(s.sep);
            r.append(QString::number(data.at(i) != 0));
        }
    }
    return r;
}

QString mbServerBuilder::fromUInt16Data(const mbServerBuilder::UInt16Data_t &data)
{
    const int maxCountDigits = 5;

    QString r;

    if (data.size())
    {
        Strings s = Strings::instance();

        r.reserve(data.size()*maxCountDigits+data.size()-1);
        r.append(QString::number(reinterpret_cast<const quint16*>(data.constData())[0]));
        for (int i = 1; i < data.size()/MB_REGE_SZ_BYTES; i++)
        {
            r.append(s.sep);
            r.append(QString::number(reinterpret_cast<const quint16*>(data.constData())[i]));
        }
    }
    return r;
}
