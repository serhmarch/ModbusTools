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
#ifndef CORE_BUILDER_H
#define CORE_BUILDER_H

#include <QObject>

#include <mbcore.h>

class QIODevice;

class mbCoreProject;
class mbCorePort;
class mbCoreDevice;
class mbCoreDataView;
class mbCoreDataViewItem;
class mbCoreTaskInfo;

class mbCoreDom;
class mbCoreDomProject;
class mbCoreDomPort;
class mbCoreDomDevice;
class mbCoreDomDataView;
class mbCoreDomDataViewItem;
class mbCoreDomTaskInfo;

class DomDataViewItems;

class MB_EXPORT mbCoreBuilder : public QObject
{
    Q_OBJECT
public:
    struct MB_EXPORT Strings
    {
        const QString xml;
        const QString csv;
        const QChar csvSep;
        //----------------
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreBuilder(QObject* parent = nullptr);

public:
    inline mbCoreProject *projectCore() const { if (m_workingProject) return m_workingProject; else return m_project; }
    inline void setWorkingProjectCore(mbCoreProject *project) { m_workingProject = project; }

public: // errors
    inline bool hasError() const { return !m_errors.isEmpty(); }
    inline QStringList errors() const { return m_errors; }
    inline void setError(const QString &err) { m_errors.append(err); }
    inline void clearErrors() { m_errors.clear(); }

public:
    mbCoreProject *loadCore(const QString &file);
    bool saveCore(mbCoreProject *project);

public: // .xml project
    virtual mbCoreProject *loadXml(const QString &file);
    virtual bool saveXml(mbCoreProject *project);

public:
    virtual QStringList csvAttributes() const;

    virtual mbCoreProject         *newProject        () const = 0;
    virtual mbCorePort            *newPort           () const = 0;
    virtual mbCoreDevice          *newDevice         () const = 0;
    virtual mbCoreDataView        *newDataView       () const = 0;
    virtual mbCoreDataViewItem    *newDataViewItem   () const = 0;
    virtual mbCoreDataViewItem    *newDataViewItem   (mbCoreDataViewItem *prev) const;

    virtual mbCoreDomProject      *newDomProject     () const = 0;
    virtual mbCoreDomPort         *newDomPort        () const = 0;
    virtual mbCoreDomDevice       *newDomDevice      () const = 0;
    virtual mbCoreDomDataView     *newDomDataView    () const = 0;
    virtual mbCoreDomDataViewItem *newDomDataViewItem() const = 0;

public:
    virtual mbCoreProject         *toProject        (mbCoreDomProject      *dom);
    virtual mbCorePort            *toPort           (mbCoreDomPort         *dom);
    virtual mbCoreDevice          *toDevice         (mbCoreDomDevice       *dom);
    virtual mbCoreDataView        *toDataView       (mbCoreDomDataView     *dom);
    virtual mbCoreDataViewItem    *toDataViewItem   (mbCoreDomDataViewItem *dom);
    virtual mbCoreDataViewItem    *toDataViewItem   (const MBSETTINGS &settings, bool processValue = true);

    virtual mbCoreDomProject      *toDomProject     (mbCoreProject         *cfg);
    virtual mbCoreDomPort         *toDomPort        (mbCorePort            *cfg);
    virtual mbCoreDomDevice       *toDomDevice      (mbCoreDevice          *cfg);
    virtual mbCoreDomDataView     *toDomDataView    (mbCoreDataView        *cfg);
    virtual mbCoreDomDataViewItem *toDomDataViewItem(mbCoreDataViewItem    *cfg);
    MBSETTINGS toSettings(const mbCoreDataViewItem *item, bool processValue = true);

public:
    mbCorePort     *importPort    (const QString &file);
    mbCoreDevice   *importDevice  (const QString &file);
    mbCoreDataView *importDataView(const QString &file);
    QList<mbCoreDataViewItem*> importDataViewItems   (const QString &file);
    QList<mbCoreDataViewItem*> importDataViewItemsXml(const QString &file);
    QList<mbCoreDataViewItem*> importDataViewItemsCsv(const QString &file);

    mbCorePort     *importPort    (QIODevice *io);
    mbCoreDevice   *importDevice  (QIODevice *io);
    mbCoreDataView *importDataView(QIODevice *io);
    QList<mbCoreDataViewItem*> importDataViewItemsXml(QIODevice *io);
    QList<mbCoreDataViewItem*> importDataViewItemsCsv(QIODevice *io);

public:
    bool exportPort         (const QString &file, mbCorePort     *cfg);
    bool exportDevice       (const QString &file, mbCoreDevice   *cfg);
    bool exportDataView     (const QString &file, mbCoreDataView *cfg);
    bool exportDataViewItems(const QString &file, const QList<mbCoreDataViewItem*> &cfg);
    bool exportDataViewItemsXml(const QString &file, const QList<mbCoreDataViewItem*> &cfg);
    bool exportDataViewItemsCsv(const QString &file, const QList<mbCoreDataViewItem*> &cfg);

    bool exportPort         (QIODevice *io, mbCorePort     *cfg);
    bool exportDevice       (QIODevice *io, mbCoreDevice   *cfg);
    bool exportDataView     (QIODevice *io, mbCoreDataView *cfg);
    bool exportDataViewItemsXml(QIODevice *io, const QList<mbCoreDataViewItem*> &cfg);
    bool exportDataViewItemsCsv(QIODevice *io, const QList<mbCoreDataViewItem*> &cfg);

protected:
    QList<mbCoreDataViewItem *> toDataViewItems(DomDataViewItems *dom);
    DomDataViewItems *toDomDataViewItems(const QList<mbCoreDataViewItem *> &cfg);

protected:
    bool loadXml(const QString &file, mbCoreDom *dom);
    bool loadXml(QIODevice *io, mbCoreDom *dom);
    bool saveXml(const QString &file, const mbCoreDom *dom);
    bool saveXml(QIODevice *io, const mbCoreDom *dom);

protected Q_SLOTS:
    void setProject(mbCoreProject *project);

protected:
    MBSETTINGS parseCsvDataViewItem(const QStringList &attrNames, const QString &row);
    QString makeCsvDataViewItem(const QStringList &attrNames, const MBSETTINGS &settings);
    QStringList parseCsvRow(const QString &row);
    QString makeCsvRow(const QStringList &items);

protected:
    mbCoreProject *m_project;
    mbCoreProject *m_workingProject;
    QStringList m_errors;
};

#endif // CORE_BUILDER_H
