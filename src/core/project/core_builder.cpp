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
    dataViewPrefix(QStringLiteral("dataview")),
    dataViewAttrNames(QStringList() << mbCoreDataView::Strings::instance().name
                                    << mbCoreDataView::Strings::instance().period),
    xml(QStringLiteral("xml")),
    csv(QStringLiteral("csv")),
    csvSep(';')
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

bool mbCoreBuilder::importProject(const QString &file)
{
    mbCoreProject *project = m_project;
    if (!project)
        return false;

    QScopedPointer<mbCoreDomProject> dom(newDomProject());
    if (loadXml(file, dom.data()))
    {
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
        importDomProject(dom.data());
        return true;
    }
    return false;
}

mbCoreProject *mbCoreBuilder::loadXml(const QString &file)
{
    QScopedPointer<mbCoreDomProject> dom(newDomProject());
    if (loadXml(file, dom.data()))
    {
        mbCoreProject* project = toProject(dom.data());
        project->setAbsoluteFilePath(file);
        refreshProjectFileInfo(project);
        return project;
    }
    return nullptr;
}

bool mbCoreBuilder::saveXml(mbCoreProject *project)
{
    beginSaveProject(project);
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
    bool r = saveXml(project->absoluteFilePath(), dom.data());
    refreshProjectFileInfo(project);
    return r;
}

void mbCoreBuilder::beginSaveProject(mbCoreProject *project)
{
    project->resetVersion();
    project->incrementEditNumber();
}

void mbCoreBuilder::refreshProjectFileInfo(mbCoreProject *project)
{
    project->setModifiedFlag(false);

    QFileInfo fi(project->absoluteFilePath());
    project->setFileSize(fi.size());
    project->setFileCreated(fi.birthTime());
    project->setFileModified(fi.lastModified());
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
    mbCoreXmlStreamReader reader(io);
    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case mbCoreXmlStreamReader::StartElement:
        {
            const QString tag = reader.name().toString().toLower();
            if (tag == dom->tagName())
                dom->read(reader);
            else
                reader.raiseError(QString("<%1>-tag not found").arg(dom->tagName()));
            finished = true;
        }
        break;
        case mbCoreXmlStreamReader::EndDocument:
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
    if (reader.hasWarning())
    {
        Q_FOREACH(const QString text, reader.warnings())
            mbCore::LogWarning(QStringLiteral("Builder"), text);
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
    mbCoreXmlStreamWriter writer(io);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    dom->write(writer);
    writer.writeEndDocument();
    return true;
}

QStringList mbCoreBuilder::csvDataViewItemAttributes() const
{
    const mbCoreDataViewItem::Strings &s = mbCoreDataViewItem::Strings::instance();
    return QStringList() << s.device            
                         << s.address           
                         << s.format            
                         << s.comment           
                         << s.variableLength    
                         << s.byteOrder         
                         << s.registerOrder     
                         << s.byteArrayFormat   
                         << s.byteArraySeparator
                         << s.stringLengthType  
                         << s.stringEncoding    
                         << s.value     
                         ;          
}

mbCoreDataViewItem *mbCoreBuilder::newDataViewItem(mbCoreDataViewItem *prev) const
{
    mbCoreDataViewItem* item = newDataViewItem();
    item->setSettings(prev->settings());
    mb::Address adr = prev->address();
    adr += prev->count();
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
    project->setVersionStr(dom->versionStr());
    project->setEditNumber(dom->editNumber());
    project->setWindowsData(dom->windowsData());

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
    wl->setAddressNotation(mb::toAddressNotation(dom->addressNotation()));
    wl->setUseDefaultColumns(dom->useDefaultColumns());
    wl->setColumnNames(dom->columns());
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
    dom->setVersionStr(project->versionStr());
    dom->setEditNumber(project->editNumber());
    dom->setWindowsData(project->windowsData());

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
    dom->setAddressNotation(mb::toString(wl->addressNotation()));
    dom->setUseDefaultColumns(wl->useDefaultColumns());
    dom->setColumns(wl->columnNames());
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

mbCoreDataViewItem *mbCoreBuilder::toDataViewItem(const MBSETTINGS &settings, bool processValue)
{
    const mbCoreDataViewItem::Strings &s = mbCoreDataViewItem::Strings::instance();
    MBSETTINGS sets = settings;
    mbCoreDataViewItem *item = newDataViewItem();
    if (mbCoreProject *project = projectCore())
    {
        MBSETTINGS::Iterator it = sets.find(s.device);
        if (it != sets.end())
        {
            QString devName = it.value().toString();
            mbCoreDevice *dev = project->deviceCore(devName);
            if (dev)
                item->setDeviceCore(dev);
            sets.erase(it);
        }
    }

    item->setSettings(sets);
    if (processValue)
    {
        QVariant v = sets.value(s.value);
        item->setValue(v);
    }
    return item;
}

mbCoreDomDataViewItem *mbCoreBuilder::toDomDataViewItem(mbCoreDataViewItem *wl)
{
    mbCoreDomDataViewItem* dom = new mbCoreDomDataViewItem;
    mbCoreDevice *dev = wl->deviceCore();
    if (dev)
        dom->setDevice(dev->name());
    MBSETTINGS settings = wl->settings();
    settings.remove(mbCoreDataViewItem::Strings::instance().device);
    dom->setSettings(settings);
    return dom;
}

MBSETTINGS mbCoreBuilder::toSettings(const mbCoreDataViewItem *item, bool processValue)
{
    const mbCoreDataViewItem::Strings &s = mbCoreDataViewItem::Strings::instance();
    MBSETTINGS sets = item->settings();
    mbCoreDevice *dev = item->deviceCore();
    if (dev)
        sets[s.device] = dev->name();
    if (processValue)
        sets[s.value] = item->value();
    return sets;
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
    if (file.endsWith(Strings::instance().csv))
        return importDataViewCsv(file);
    return importDataViewXml(file);
}

mbCoreDataView *mbCoreBuilder::importDataViewXml(const QString &file)
{
    QScopedPointer<mbCoreDomDataView> dom(newDomDataView());
    if (loadXml(file, dom.data()))
        return toDataView(dom.data());
    return nullptr;
}

mbCoreDataView *mbCoreBuilder::importDataViewCsv(const QString &file)
{
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly))
    {
        setError(qf.errorString());
        return nullptr;
    }
    mbCoreDataView* view = importDataViewCsv(&qf);
    qf.close();
    return view;
}

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItems(const QString &file)
{
    if (file.endsWith(Strings::instance().csv))
        return importDataViewItemsCsv(file);
    return importDataViewItemsXml(file);
}

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItemsXml(const QString &file)
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

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItemsCsv(const QString &file)
{
    QFile qf(file);
    if (!qf.open(QIODevice::ReadOnly))
    {
        setError(qf.errorString());
        return QList<mbCoreDataViewItem *>();
    }
    QList<mbCoreDataViewItem *> items = importDataViewItemsCsv(&qf);
    qf.close();
    return items;
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

mbCoreDataView *mbCoreBuilder::importDataViewXml(QIODevice *io)
{
    QScopedPointer<mbCoreDomDataView> dom(newDomDataView());
    if (loadXml(io, dom.data()))
        return toDataView(dom.data());
    return nullptr;
}

mbCoreDataView *mbCoreBuilder::importDataViewCsv(QIODevice *io)
{
    qint64 mempos = io->pos();
    QByteArray headerBytes = io->readLine();
    QString header = QString::fromUtf8(headerBytes);
    MBSETTINGS settings;
    if (header.startsWith(Strings::instance().dataViewPrefix))
        settings = parseCsvDataViewSettings(header);
    else
        io->seek(mempos);
    QList<mbCoreDataViewItem*> items = importDataViewItemsCsv(io);
    mbCoreDataView *view = newDataView();
    if (settings.count())
        view->setSettings(settings);
    if (items.count())
        view->itemsInsert(items);
    return view;
}

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItemsXml(QIODevice *io)
{
    DomDataViewItems dom(this);
    if (loadXml(io, &dom))
        return toDataViewItems(&dom);
    return QList<mbCoreDataViewItem*>();
}

QList<mbCoreDataViewItem *> mbCoreBuilder::importDataViewItemsCsv(QIODevice *io)
{
    QList<mbCoreDataViewItem *> items;
    QString header = QString::fromUtf8(io->readLine());
    if (header.startsWith(Strings::instance().dataViewPrefix))
        header = QString::fromUtf8(io->readLine());
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
        mbCoreDataViewItem *item = toDataViewItem(settings);
        items.append(item);
    }
    return items;
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
    if (file.endsWith(Strings::instance().csv))
        return exportDataViewCsv(file, cfg);
    return exportDataViewXml(file, cfg);
}

bool mbCoreBuilder::exportDataViewXml(const QString &file, mbCoreDataView *cfg)
{
    QScopedPointer<mbCoreDomDataView> dom(toDomDataView(cfg));
    return saveXml(file, dom.data());
}

bool mbCoreBuilder::exportDataViewCsv(const QString &file, mbCoreDataView *cfg)
{
    QFile qf(file);
    if (!qf.open(QIODevice::WriteOnly))
    {
        setError(qf.errorString());
        return false;
    }
    bool res = exportDataViewCsv(&qf, cfg);
    qf.close();
    return res;
}

bool mbCoreBuilder::exportDataViewItems(const QString &file, const QList<mbCoreDataViewItem *> &cfg)
{
    if (file.endsWith(Strings::instance().csv))
        return exportDataViewItemsCsv(file, cfg);
    return exportDataViewItemsXml(file, cfg);
}

bool mbCoreBuilder::exportDataViewItemsXml(const QString &file, const QList<mbCoreDataViewItem *> &cfg)
{
    QScopedPointer<DomDataViewItems> dom(toDomDataViewItems(cfg));
    return saveXml(file, dom.data());
}

bool mbCoreBuilder::exportDataViewItemsCsv(const QString &file, const QList<mbCoreDataViewItem *> &cfg)
{
    QFile qf(file);
    if (!qf.open(QIODevice::WriteOnly))
    {
        setError(qf.errorString());
        return false;
    }
    bool res = exportDataViewItemsCsv(&qf, cfg);
    qf.close();
    return res;
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

bool mbCoreBuilder::exportDataViewXml(QIODevice *io, mbCoreDataView *cfg)
{
    QScopedPointer<mbCoreDomDataView> dom(toDomDataView(cfg));
    return saveXml(io, dom.data());
}

bool mbCoreBuilder::exportDataViewCsv(QIODevice *io, mbCoreDataView *cfg)
{
    QString s = makeCsvDataViewSettings(Strings::instance().dataViewAttrNames, cfg->settings());
    io->write(s.toUtf8());
    return exportDataViewItemsCsv(io, cfg->itemsCore());
}

bool mbCoreBuilder::exportDataViewItemsXml(QIODevice *io, const QList<mbCoreDataViewItem *> &cfg)
{
    QScopedPointer<DomDataViewItems> dom(toDomDataViewItems(cfg));
    return saveXml(io, dom.data());
}

bool mbCoreBuilder::exportDataViewItemsCsv(QIODevice *io, const QList<mbCoreDataViewItem *> &cfg)
{
    QStringList lsHeader = csvDataViewItemAttributes();
    QString sHeader = makeCsvRow(lsHeader);
    io->write(sHeader.toUtf8());
    Q_FOREACH (const mbCoreDataViewItem *item, cfg)
    {
        MBSETTINGS settings = toSettings(item);
        QString sLine = makeCsvRowItem(lsHeader, settings);
        io->write(sLine.toUtf8());
    }
    return true;;
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

void mbCoreBuilder::importDomProject(mbCoreDomProject * /*dom*/)
{
    // Note: Base implementation does nothing
}

void mbCoreBuilder::setProject(mbCoreProject *project)
{
    m_project = project;
}

MBSETTINGS mbCoreBuilder::parseCsvDataViewSettings(const QString &row)
{
    MBSETTINGS settings;
    QStringList attrs = parseCsvRow(row);

    // Note: pass attrs[0] that must be equal to "dataview"
    for (int i = 0; i < attrs.count() /2; i++)
    {
        QString name = attrs.at(i*2+1);
        QString value = attrs.at(i*2+2);
        settings[name] = value;
    }
    return settings;
}

QString mbCoreBuilder::makeCsvDataViewSettings(const QStringList &attrNames, const MBSETTINGS &settings)
{
    QStringList attrs;

    attrs.append(Strings::instance().dataViewPrefix);
    Q_FOREACH (const QString attrName, attrNames)
    {
        attrs.append(attrName);
        attrs.append(settings.value(attrName).toString());
    }

    QString v = makeCsvRow(attrs);
    return v;
}

MBSETTINGS mbCoreBuilder::parseCsvRowItem(const QStringList &attrNames, const QString &row)
{
    MBSETTINGS settings;
    QStringList attrs = parseCsvRow(row);
    QStringList::ConstIterator it = attrs.constBegin();
    QStringList::ConstIterator end = attrs.constEnd();

    Q_FOREACH (const QString attrName, attrNames)
    {
        if (it == end)
            break;
        settings[attrName] = *it;
        it++;
    }
    return settings;
}

QString mbCoreBuilder::makeCsvRowItem(const QStringList &attrNames, const MBSETTINGS &settings)
{
    QStringList attrs;

    Q_FOREACH (const QString attrName, attrNames)
    {
        attrs.append(settings.value(attrName).toString());
    }

    QString v = makeCsvRow(attrs);
    return v;
}

QStringList mbCoreBuilder::parseCsvRow(const QString &row)
{
    QString sr = row;

    // remove CR LF symbols from the end of row
    int i = sr.length() - 1;
    if ((i >= 0) && ((sr.at(i).unicode() == QChar::CarriageReturn) || (sr.at(i).unicode() == QChar::LineFeed)))
    {
        int ln = i;
        i--;
        while ((i >= 0) && ((sr.at(i).unicode() == QChar::CarriageReturn) || (sr.at(i).unicode() == QChar::LineFeed)))
        {
            i--;
            ln--;
        }
        sr = sr.left(ln);
    }

    // Begin parsing
    QChar sep = Strings::instance().csvSep;
    QStringList result;
    QString currentField;
    bool inQuotes = false;
    i = 0;
    while (i < sr.length())
    {
        QChar ch = sr[i];
        if (ch == '"') 
        {
            if (inQuotes && i + 1 < sr.length() && sr[i + 1] == '"')
            {
                // Two consecutive quotes inside quoted field -> append a single quote
                currentField.append('"');
                ++i; // skip the next quote
            } 
            else 
            {
                // Toggle inQuotes status
                inQuotes = !inQuotes;
            }
        } 
        else if (ch == sep && !inQuotes) 
        {
            // Separator outside quotes -> end of current field
            result.append(currentField);
            currentField.clear();
        } 
        else 
        {
            // Regular character
            currentField.append(ch);
        }
        ++i;
    }
    // Add the last field
    result.append(currentField);
    return result;
}

QString mbCoreBuilder::makeCsvRow(const QStringList &items)
{
    QChar sep = Strings::instance().csvSep;
    QString result;
    for (int i = 0; i < items.size(); ++i) 
    {
        QString item = items[i];
        bool requiresQuotes = item.contains(sep) || item.contains('"') || item.contains('\n');

        if (requiresQuotes)
        {
            result.append('"');
            // Escape quotes by doubling them
            Q_FOREACH (QChar ch, item)
            {
                if (ch == '"') 
                    result.append("\"\"");
                else 
                    result.append(ch);
            }
            result.append('"');
        } 
        else 
        {
            result.append(item);
        }

        // Append separator unless it’s the last item
        if (i < items.size() - 1)
            result.append(sep);
    }
    result.append(QChar::LineFeed);
    return result;
}

