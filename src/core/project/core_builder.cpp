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
#include "core_builder.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include <core.h>
#include <task/core_taskfactoryinfo.h>

#include "core_project.h"
#include "core_port.h"
#include "core_device.h"
#include "core_dataview.h"
#include "core_dom.h"
#include "core_taskinfo.h"

class DomDataViewItems : public mbCoreDomItems<mbCoreDomDataViewItem>
{
public:
    DomDataViewItems(mbCoreBuilder *builder) :
        mbCoreDomItems<mbCoreDomDataViewItem>("items", mbCoreDomDataViewItem::Strings::instance().tagName),
        m_builder(builder)
    {
    }

    mbCoreDomDataViewItem *newItem() const override { return m_builder->newDomDataViewItem();}

private:
    mbCoreBuilder *m_builder;
};

mbCoreBuilder::Strings::Strings() :
    xml(QStringLiteral("xml"))
{
}

const mbCoreBuilder::Strings &mbCoreBuilder::Strings::instance()
{
    static const Strings s = Strings();
    return s;
}

mbCoreBuilder::mbCoreBuilder(QObject *parent) :
    QObject (parent)
{
    m_workingProject = nullptr;
    m_project = mbCore::globalCore()->projectCore();
    connect(mbCore::globalCore(), &mbCore::projectChanged, this, &mbCoreBuilder::setProject);
}

mbCoreProject *mbCoreBuilder::loadCore(const QString &file)
{
    return loadXml(file);
}

bool mbCoreBuilder::saveCore(mbCoreProject *project)
{
    return saveXml(project);
}

mbCoreProject *mbCoreBuilder::loadXml(const QString &file)
{
    QScopedPointer<mbCoreDomProject> dom(newDomProject());
    if (loadXml(file, dom.data()))
    {
        mbCoreProject* project = toProject(dom.data());
        project->setAbsoluteFilePath(file);
        return project;
    }
    return nullptr;
}

bool mbCoreBuilder::saveXml(mbCoreProject *project)
{
    QScopedPointer<mbCoreDomProject> dom(toDomProject(project));
    if (!dom)
        return false;
    // Make sure that path is exists
    QDir dir;
    if (!dir.mkpath(project->absoluteDirPath()))
    {
        setError(QString("Can't create project directory '%1'").arg(project->absoluteDirPath()));
        return false;
    }
    return saveXml(project->absoluteFilePath(), dom.data());
}

mbCoreDataViewItem *mbCoreBuilder::newDataViewItem(mbCoreDataViewItem *prev) const
{
    mbCoreDataViewItem* item = newDataViewItem();
    item->setSettings(prev->settings());
    mb::Address adr = prev->address();
    adr.offset++;
    item->setAddress(adr);
    return item;
}

mbCoreProject *mbCoreBuilder::toProject(mbCoreDomProject *dom)
{
    mbCoreProject *project = newProject();
    m_workingProject = project;
    project->setName(dom->name());
    project->setAuthor(dom->author());
    project->setComment(dom->comment());

    Q_FOREACH(mbCoreDomDevice *d, dom->devices())
    {
        mbCoreDevice *v = toDevice(d);
        project->deviceAdd(v);
    }

    Q_FOREACH(mbCoreDomPort *d, dom->ports())
    {
        mbCorePort *v = toPort(d);
        project->portAdd(v);
    }

    Q_FOREACH(mbCoreDomDataView *d, dom->dataViews())
    {
        mbCoreDataView *v = toDataView(d);
        project->dataViewAdd(v);
    }

    Q_FOREACH (mbCoreDomTaskInfo* d, dom->tasks())
    {
        mbCoreTaskFactoryInfo *fi = mbCore::globalCore()->taskFactory(d->type());
        mbCoreTaskFactory *f = fi ? fi->taskFactory() : nullptr;
        mbCoreTaskInfo *info = new mbCoreTaskInfo(d->type(), f);
        info->setName(d->name());
        info->setParams(d->settings());
        info->setProject(project);
    }
    m_workingProject = nullptr;
    return project;
}

mbCorePort *mbCoreBuilder::toPort(mbCoreDomPort *dom)
{
    mbCorePort *e = newPort();
    e->setSettings(dom->settings());
    return e;
}

mbCoreDevice *mbCoreBuilder::toDevice(mbCoreDomDevice *dom)
{
    mbCoreDevice *e = newDevice();
    e->setSettings(dom->settings());
    return e;
}

mbCoreDataView *mbCoreBuilder::toDataView(mbCoreDomDataView *dom)
{
    mbCoreDataView *wl = newDataView();
    wl->setName(dom->name());
    wl->setPeriod(dom->period());
    Q_FOREACH (mbCoreDomDataViewItem *domItem, dom->items())
    {
        mbCoreDataViewItem *item = toDataViewItem(domItem);
        wl->itemAdd(item);
    }
    return wl;
}

mbCoreDomProject *mbCoreBuilder::toDomProject(mbCoreProject *project)
{
    setWorkingProjectCore(project);
    mbCoreDomProject *dom = newDomProject();
    dom->setName(project->name());
    dom->setAuthor(project->author());
    dom->setComment(project->comment());

    QList<mbCoreDomDevice*> devices;
    Q_FOREACH(mbCoreDevice *e, project->devicesCore())
    {
        mbCoreDomDevice *d = toDomDevice(e);
        devices.append(d);
    }
    dom->setDevices(devices);

    QList<mbCoreDomPort*> ports;
    Q_FOREACH(mbCorePort *e, project->portsCore())
    {
        mbCoreDomPort *d = toDomPort(e);
        ports.append(d);
    }
    dom->setPorts(ports);

    QList<mbCoreDomDataView*> dataViews;
    Q_FOREACH(mbCoreDataView *e, project->dataViewsCore())
    {
        mbCoreDomDataView *d = toDomDataView(e);
        dataViews.append(d);
    }
    dom->setDataViews(dataViews);


    QList<mbCoreDomTaskInfo*> ds;
    Q_FOREACH (mbCoreTaskInfo* info, project->tasks())
    {
        mbCoreDomTaskInfo *d = new mbCoreDomTaskInfo();
        d->setName(info->name());
        d->setType(info->type());
        d->setSettings(info->params());
        ds.append(d);
    }
    dom->setTasks(ds);
    setWorkingProjectCore(nullptr);
    return dom;
}

mbCoreDomPort *mbCoreBuilder::toDomPort(mbCorePort *d)
{
    mbCoreDomPort *dom = newDomPort();
    dom->setSettings(d->settings());
    return dom;
}

mbCoreDomDevice *mbCoreBuilder::toDomDevice(mbCoreDevice *dev)
{
    mbCoreDomDevice* dom = newDomDevice();
    dom->setSettings(dev->settings());
    return dom;
}

mbCoreDomDataView *mbCoreBuilder::toDomDataView(mbCoreDataView *wl)
{
    mbCoreDomDataView *dom = newDomDataView();
    dom->setName(wl->name());
    dom->setPeriod(wl->period());
    QList<mbCoreDomDataViewItem*> domItems;
    Q_FOREACH(mbCoreDataViewItem *item, wl->itemsCore())
    {
        mbCoreDomDataViewItem *domItem = toDomDataViewItem(item);
        domItems.append(domItem);

    }
    dom->setItems(domItems);
    return dom;
}

mbCoreDataViewItem *mbCoreBuilder::toDataViewItem(mbCoreDomDataViewItem *dom)
{
    mbCoreDataViewItem* item = newDataViewItem();
    if (mbCoreProject *project = projectCore())
    {
        mbCoreDevice *dev = project->deviceCore(dom->device());
        item->setDeviceCore(dev);
    }
    MBSETTINGS settings = dom->settings();
    settings.remove(mbCoreDataViewItem::Strings::instance().device);
    item->setSettings(settings);
    return item;
}

mbCoreDomDataViewItem *mbCoreBuilder::toDomDataViewItem(mbCoreDataViewItem *wl)
{
    mbCoreDomDataViewItem* dom = new mbCoreDomDataViewItem;
    if (wl->deviceCore())
        dom->setDevice(wl->deviceCore()->name());
    MBSETTINGS settings = wl->settings();
    settings.remove(mbCoreDataViewItem::Strings::instance().device);
    dom->setSettings(settings);
    return dom;
}

mbCorePort *mbCoreBuilder::importPort(const QString &file)
{
    QScopedPointer<mbCoreDomPort> dom(newDomPort());
    if (loadXml(file, dom.data()))
        return toPort(dom.data());
    return nullptr;
}

mbCoreDevice *mbCoreBuilder::importDevice(const QString &file)
{
    QScopedPointer<mbCoreDomDevice> dom(newDomDevice());
    if (loadXml(file, dom.data()))
        return toDevice(dom.data());
    return nullptr;
}

mbCoreDataView *mbCoreBuilder::importDataView(const QString &file)
{
    QScopedPointer<mbCoreDomDataView> dom(newDomDataView());
    if (loadXml(file, dom.data()))
        return toDataView(dom.data());
    return nullptr;
}

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItems(const QString &file)
{
    DomDataViewItems dom(this);
    if (loadXml(file, &dom))
        return toDataViewItems(&dom);

    QScopedPointer<mbCoreDomDataView> domDataView(newDomDataView());
    QList<mbCoreDataViewItem *> ls;
    if (loadXml(file, domDataView.data()))
    {
        Q_FOREACH (mbCoreDomDataViewItem *domItem, domDataView.data()->items())
            ls.append(toDataViewItem(domItem));
    }
    return ls;
}

mbCorePort *mbCoreBuilder::importPort(QIODevice *io)
{
    QScopedPointer<mbCoreDomPort> dom(newDomPort());
    if (loadXml(io, dom.data()))
        return toPort(dom.data());
    return nullptr;
}

mbCoreDevice *mbCoreBuilder::importDevice(QIODevice *io)
{
    QScopedPointer<mbCoreDomDevice> dom(newDomDevice());
    if (loadXml(io, dom.data()))
        return toDevice(dom.data());
    return nullptr;
}

mbCoreDataView *mbCoreBuilder::importDataView(QIODevice *io)
{
    QScopedPointer<mbCoreDomDataView> dom(newDomDataView());
    if (loadXml(io, dom.data()))
        return toDataView(dom.data());
    return nullptr;
}

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItems(QIODevice *io)
{
    DomDataViewItems dom(this);
    if (loadXml(io, &dom))
        return toDataViewItems(&dom);
    return QList<mbCoreDataViewItem*>();
}

bool mbCoreBuilder::exportPort(const QString &file, mbCorePort *cfg)
{
    QScopedPointer<mbCoreDomPort> dom(toDomPort(cfg));
    return saveXml(file, dom.data());
}

bool mbCoreBuilder::exportDevice(const QString &file, mbCoreDevice *cfg)
{
    QScopedPointer<mbCoreDomDevice> dom(toDomDevice(cfg));
    return saveXml(file, dom.data());
}

bool mbCoreBuilder::exportDataView(const QString &file, mbCoreDataView *cfg)
{
    QScopedPointer<mbCoreDomDataView> dom(toDomDataView(cfg));
    return saveXml(file, dom.data());
}

bool mbCoreBuilder::exportDataViewItems(const QString &file, const QList<mbCoreDataViewItem *> &cfg)
{
    QScopedPointer<DomDataViewItems> dom(toDomDataViewItems(cfg));
    return saveXml(file, dom.data());
}

bool mbCoreBuilder::exportPort(QIODevice *io, mbCorePort *cfg)
{
    QScopedPointer<mbCoreDomPort> dom(toDomPort(cfg));
    return saveXml(io, dom.data());
}

bool mbCoreBuilder::exportDevice(QIODevice *io, mbCoreDevice *cfg)
{
    QScopedPointer<mbCoreDomDevice> dom(toDomDevice(cfg));
    return saveXml(io, dom.data());
}

bool mbCoreBuilder::exportDataView(QIODevice *io, mbCoreDataView *cfg)
{
    QScopedPointer<mbCoreDomDataView> dom(toDomDataView(cfg));
    return saveXml(io, dom.data());
}

bool mbCoreBuilder::exportDataViewItems(QIODevice *io, const QList<mbCoreDataViewItem *> &cfg)
{
    QScopedPointer<DomDataViewItems> dom(toDomDataViewItems(cfg));
    return saveXml(io, dom.data());
}

QList<mbCoreDataViewItem *> mbCoreBuilder::toDataViewItems(DomDataViewItems *dom)
{
    QList<mbCoreDataViewItem *> ls;
    Q_FOREACH (mbCoreDomDataViewItem *di, dom->items())
    {
        mbCoreDataViewItem *item = toDataViewItem(di);
        if (item)
            ls.append(item);
    }
    return ls;
}

DomDataViewItems *mbCoreBuilder::toDomDataViewItems(const QList<mbCoreDataViewItem *> &cfg)
{
    DomDataViewItems *dom = new DomDataViewItems(this);
    QList<mbCoreDomDataViewItem*> domItems;
    Q_FOREACH(mbCoreDataViewItem *item, cfg)
    {
        mbCoreDomDataViewItem *domItem = toDomDataViewItem(item);
        domItems.append(domItem);

    }
    dom->setItems(domItems);
    return dom;
}

bool mbCoreBuilder::loadXml(const QString &file, mbCoreDom *dom)
{
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly))
    {
        setError(qf.errorString());
        return false;
    }
    bool r = loadXml(&qf, dom);
    qf.close();
    return r;
}

bool mbCoreBuilder::loadXml(QIODevice *io, mbCoreDom *dom)
{
    QXmlStreamReader reader(io);
    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement:
        {
            const QString tag = reader.name().toString().toLower();
            if (tag == dom->tagName())
                dom->read(reader);
            else
                reader.raiseError(QString("<%1>-tag not found").arg(dom->tagName()));
            finished = true;
        }
        break;
        case QXmlStreamReader::EndDocument:
            reader.raiseError(QString("<%1>-tag not found").arg(dom->tagName()));
            finished = true;
            break;
        default:
            break;
        }
    }
    if (reader.hasError())
    {
        setError(reader.errorString());
        return false;
    }
    return true;
}

bool mbCoreBuilder::saveXml(const QString &file, const mbCoreDom *dom)
{
    QFile qf(file);
    if (!qf.open(QIODevice::WriteOnly))
    {
        setError(qf.errorString());
        return false;
    }
    bool r = saveXml(&qf, dom);
    qf.close();
    return r;

}

bool mbCoreBuilder::saveXml(QIODevice *io, const mbCoreDom *dom)
{
    QXmlStreamWriter writer(io);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    dom->write(writer);
    writer.writeEndDocument();
    return true;
}

void mbCoreBuilder::setProject(mbCoreProject *project)
{
    m_project = project;
}

