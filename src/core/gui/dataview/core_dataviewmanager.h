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
#ifndef CORE_DATAVIEWMANAGER_H
#define CORE_DATAVIEWMANAGER_H

#include <QHash>
#include <QObject>

#include <mbcore.h>

class mbCoreProject;
class mbCoreDataView;
class mbCoreDataViewUi;

class MB_EXPORT mbCoreDataViewManager : public QObject
{
    Q_OBJECT

public:
    explicit mbCoreDataViewManager(QObject *parent = nullptr);

public: // project
    inline mbCoreProject *projectCore() const { return m_project; }
    inline QList<mbCoreDataViewUi*> dataViewUisCore() const { return m_dataViewUis; }
    inline mbCoreDataViewUi *activeDataViewUiCore() const { return m_activeDataViewUi; }
    mbCoreDataView *activeDataViewCore() const;

public: // watch list ui
    inline bool hasDataViewUi(const mbCoreDataView *dataView) const { return m_hashDataViewUis.contains(dataView); }
    inline int dataViewCount() const { return m_hashDataViewUis.count(); }
    inline mbCoreDataViewUi *dataViewUiCore(mbCoreDataView *dataView) const { return m_hashDataViewUis.value(dataView, nullptr); }
    mbCoreDataViewUi *dataViewUiCore(const QString &name) const;

Q_SIGNALS:
    void dataViewUiAdd(mbCoreDataViewUi *ui);
    void dataViewUiRemove(mbCoreDataViewUi *ui);
    void dataViewUiActivated(mbCoreDataViewUi*);
    void dataViewUiContextMenu(mbCoreDataViewUi*);

public Q_SLOTS:
    void setActiveDataViewUi(mbCoreDataViewUi *ui);

protected Q_SLOTS:
    virtual void setProject(mbCoreProject *project);
    virtual void dataViewAdd(mbCoreDataView *dataView);
    virtual void dataViewRemove(mbCoreDataView *dataView);
    virtual void dataViewContextMenu(const QPoint &pos);

protected:
    virtual mbCoreDataViewUi *createDataViewUi(mbCoreDataView *dataView) = 0;

protected:
    mbCoreProject *m_project;

    typedef QList<mbCoreDataViewUi*> DataViewUis_t;
    typedef QHash<const mbCoreDataView*, mbCoreDataViewUi*> HashDataViewUis_t;
    DataViewUis_t m_dataViewUis;
    HashDataViewUis_t m_hashDataViewUis;
    mbCoreDataViewUi *m_activeDataViewUi;

};

#endif // CORE_DATAVIEWMANAGER_H
